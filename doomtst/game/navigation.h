#pragma once
#include "../math/mathutil.h"
#include "../world/grid.h"
#include "../math/dir.h"
#include <unordered_set>
#include <queue>
#include "../game/time.h"
#include "ecs/component.h"
namespace navigation {

	template<typename NodeContext>
	concept NetworkLike =
		requires{
		typename NodeContext::edge;
		typename NodeContext::node;
	}
	&& 
		requires(NodeContext& context, typename NodeContext::node& node, NodeContext::edge& edge) {
			{
				context.moves(node)
			}->std::same_as<stn::array<typename NodeContext::edge>>;
			{
				edge.apply(node)
			}->std::same_as< typename NodeContext::node>;
	};

	template<NetworkLike Context>
	using NodeType = typename Context::node;
	template<NetworkLike Context>
	using EdgeType = typename Context::edge;
	template<typename T>
	concept AStarNetwork = NetworkLike<T> &&

		requires(const NodeType<T>&node, const EdgeType<T>&edge) {
			{
				NodeType<T>::apx_distance(node, node)
			}->std::convertible_to<double>;
			{
				edge.cost()
			}->std::convertible_to<double>;

	};

	template<AStarNetwork Network> 
	struct NodeResult {
		NodeType<Network> current;
		EdgeType<Network> move;
		NodeType<Network> result() const {
			return move.apply(current);
		}
	};

	template<AStarNetwork T>
	using ContextResultType = NodeResult<T>;
	template<AStarNetwork T>
	struct MarkedNode {
		using node = NodeType<T>;
		using edge = EdgeType<T>;
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
	
	template<AStarNetwork Context>
	stn::Option<stn::array<ContextResultType<Context>>> a_star(NodeType<Context> start, NodeType<Context> end, Context ctx) {
		using NodeType = NodeType<Context>;
		
		using EdgeType = EdgeType<Context>;
		using MarkedNodeType = MarkedNode<Context>;

		stn::array<size_t> open;
		std::unordered_set<NodeType> closed;
		stn::array<MarkedNodeType> nodes;
		open.push(0);
		nodes.push(MarkedNodeType{ .value = start,.g_cost = 0,.h_cost = NodeType::apx_distance(start,end) });

		const int maxiter = 600;
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
				//try it 
				return stn::None;
			}
			if (current.value == end||iter==maxiter) {
				using ResultType = NodeResult<Context>;
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