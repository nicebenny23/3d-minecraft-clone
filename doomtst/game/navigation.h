#pragma once
#include "../math/mathutil.h"
#include "../world/grid.h"
#include "../math/dir.h"
#include <unordered_set>
#include <queue>
#include "../game/time.h"
#include "ecs/component.h"
namespace navigation {

	template<typename Node>
	concept NodeLike = std::equality_comparable<Node> &&
		requires(Node & node) {
			{
				Node::apx_distance(node, node)
			}->std::convertible_to<double>;
	};
	template<typename Edge, typename Node>
	concept EdgeLike = NodeLike<Node> &&
		requires(const Edge & edge, const Node & node) {
			{
				edge.cost()
			}->std::convertible_to<double>;
			{
				edge.apply(node)
			}->std::same_as<Node>;

	};



	template<typename NodeContext>
	concept NodeContextLike =
		requires{

		typename NodeContext::edge;
		typename NodeContext::node;
	}

	&& EdgeLike< typename NodeContext::edge, typename NodeContext::node>&&
		requires(NodeContext& context, typename NodeContext::node& node) {
			{
				context.moves(node)
			}->std::same_as<stn::array<typename NodeContext::edge>>;
	};

	template<NodeContextLike NodeContext>
	using ContextNodeType = typename NodeContext::node;

	template<NodeContextLike NodeContext>
	using ContextEdgeType = typename NodeContext::edge;

	template<NodeLike Node, typename Edge> requires EdgeLike<Edge, Node>
	struct NodeResult {
		Node current;
		Edge move;
		Node result() const {
			return move.apply(current);
		}
	};

	template<NodeContextLike T>
	using ContextResultType = NodeResult< ContextNodeType<T>, ContextEdgeType<T>>;
	template<NodeContextLike T>
	struct MarkedNode {
		using node = ContextNodeType<T>;
		using edge = ContextEdgeType<T>;
		node value;
		double f_cost() const {
			return g_cost + h_cost;
		}
		std::partial_ordering operator<=>(const MarkedNode& other) const {
			return f_cost() <=> other.f_cost();
		}
		stn::Option<size_t> parent;
		stn::Option<edge> parent_edge;
		double g_cost;
		double h_cost;
	};
	
	template<NodeContextLike Context>
	stn::Option<stn::array<ContextResultType<Context>>> a_star(ContextNodeType<Context> start, ContextNodeType<Context> end, Context ctx) {
		using NodeType = ContextNodeType<Context>;
		
		using EdgeType = ContextEdgeType<Context>;
		using MarkedNodeType = MarkedNode<Context>;

		stn::array<size_t> open;
		std::unordered_set<NodeType> closed;
		stn::array<MarkedNodeType> nodes;
		open.push(0);
		nodes.push(MarkedNodeType{ .value = start,.g_cost = 0,.h_cost = NodeType::apx_distance(start,end) });

		const int maxiter = 200;
		int iter = 0;
		while (open.non_empty()) {
			iter += 1;
			// Find the node with the lowest f cost
			stn::Option<size_t> shortest_ind = stn::None;
			float shortestcost = std::numeric_limits<float>().infinity();
			for (int i = 0; i < open.length(); ++i) {
				float nodecost = nodes[open[i]].f_cost();
				if (nodecost < shortestcost) {
					shortestcost = nodecost;
					shortest_ind = i;
				}
			}
			if (shortest_ind == stn::None) {
				break;
			}
			size_t open_list_index = shortest_ind.unwrap();
			size_t current_index = open[open_list_index];
			MarkedNodeType current = nodes[current_index];
			open.swap_drop(open_list_index);
			if (iter == maxiter) {
				return stn::None;
				for (size_t index:open) {
					if (nodes[index].f_cost()< current.f_cost()) {
						current = nodes[index];
					}
				}
			}
			if (current.value == end||iter==maxiter) {
				using ResultType = NodeResult<NodeType, EdgeType>;
				stn::array<ResultType> return_nodes;
				MarkedNodeType top = current;
				while (top.value!=start) {
					EdgeType leading = top.parent_edge.unwrap();
					top = nodes[top.parent.unwrap()];
					return_nodes.push(ResultType{ .current = top.value,.move = leading });
				}
				return_nodes.reverse_in_place();
				return std::move(return_nodes);
			}
			closed.insert(current.value);
			stn::array<EdgeType> neighbors = ctx.moves(current.value);
			for (EdgeType& edge : neighbors) {
				NodeType neighbor_node = edge.apply(current.value);
				if (closed.contains(neighbor_node)) {
					continue;
				}

				MarkedNodeType node{
					.value = neighbor_node,.parent = current_index ,.parent_edge=edge,.g_cost = current.g_cost + edge.cost(),.h_cost = NodeType::apx_distance(end,neighbor_node)
				};
				stn::Option<size_t> found = nodes.index_such_that([&](const MarkedNodeType& other)
					->bool {return neighbor_node == other.value; });
				if (node.value != current.value) {
					
					if (found) {
						MarkedNodeType& elem = nodes[found.unwrap()];
						if (node.g_cost< elem.g_cost) {
   							elem = node;
						}
					}    
					else {
						nodes.push(node);
						open.push(nodes.last_index());
					}
				}

			}
		}
		return stn::None;
	}
	template<NodeContextLike Context>
	struct NavigationTape {

		size_t current_index;
		stn::array<ContextResultType<Context>> tape;
	};
	struct GridCoord {
		v3::Coord pos;
		static double apx_distance(const GridCoord& c1, const GridCoord& c2) {
			return v3::manhattan_distance(c1.pos, c2.pos);
		}
		bool operator==(const GridCoord& other) const = default;
	};
}
namespace std {
	template<>
	struct hash<navigation::GridCoord> {
		size_t operator()(const navigation::GridCoord& gc) const noexcept {
			return std::hash<v3::Coord>()(gc.pos);
		}
	};
}