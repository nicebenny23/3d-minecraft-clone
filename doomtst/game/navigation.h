#pragma once
#include "../math/mathutil.h"
#include "../world/grid.h"
#include "../math/dir.h"
#include <unordered_set>
#include <queue>
#include "../game/time.h"
#include "ecs/component.h"
namespace navigation {

	struct position {

	};

	template<typename Node>
	concept NodeLike = std::equality_comparable<Node> &&
		requires(Node & node) {
			{
				Node::apx_distance(node, node)
			}->std::convertible_to<double>;
	};
	template<NodeLike T>
	struct WithCost {
		WithCost(const T& node, double transition_cost) :value(node), cost(transition_cost) {

		}
		T value;
		double cost;
	};

	template<typename NodeContext>
	concept NodeContextLike =
		requires{
		typename NodeContext::node;
	}
	&& NodeLike< typename NodeContext::node>&&
		requires(NodeContext& context, typename NodeContext::node& node) {
			{
				context.neighbors(node)
			}->std::same_as<stn::array<WithCost<std::remove_cvref_t<decltype(node)>>>>;
	};

	template<NodeLike T>
	struct MarkedNode {
		T value;
		double f_cost() const {
			return g_cost + h_cost;
		}
		std::partial_ordering operator<=>(const MarkedNode& other) const {
			return f_cost() <=> other.f_cost();
		}
		stn::Option<size_t> parent;
		double g_cost;
		double h_cost;
	};
	template<NodeContextLike NodeContext>
	using NodeContextNode = typename NodeContext::node;
	template<NodeContextLike Context>
	stn::Option<stn::array<NodeContextNode<Context>>> a_star(NodeContextNode<Context> start, NodeContextNode<Context> end, Context ctx) {
		using NodeType = NodeContextNode<Context>;
		using MarkedNodeType = MarkedNode<NodeType>;

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
			size_t index = open[open_list_index];
			MarkedNodeType current = nodes[index];
			open.swap_drop(open_list_index);
			if (iter == maxiter || current.value == end) {
				stn::array<NodeType> return_nodes;
				MarkedNodeType top = current;
				while (true) {
					return_nodes.push(top.value);
					if (top.parent.is_none()) {
						break;
					}
					top = nodes[top.parent.unwrap()];
				}
				return std::move(return_nodes);
			}
			closed.insert(current.value);
			stn::array<WithCost<NodeType>> neighbors = ctx.neighbors(current.value);
			for (WithCost<NodeType>& neighbor : neighbors) {
				if (closed.contains(neighbor.value)) {
					continue;
				}

				MarkedNodeType node{
					.value = neighbor.value,.parent = index,.g_cost = current.g_cost + neighbor.cost,.h_cost = NodeType::apx_distance(end,neighbor.value)
				};
				stn::Option<MarkedNodeType&> found = nodes.such_that([&](MarkedNodeType& other)
					->bool {return node.value == other.value; });
				if (node.value != current.value) {
					if (found) {
						stn::set_min(found.unwrap(), node);

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
	struct NavigationComponent :ecs::component {
		NavigationComponent(ecs::Constrained<ecs::world_transform> to_follow)
			:following(to_follow) {

		}
		stn::array<GridCoord> locations;
		ecs::Constrained<ecs::world_transform> following;
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