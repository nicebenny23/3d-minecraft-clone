#include "../items/loottable.h"
#include "block_registry.h"
#include "../world/managegrid.h"
#include "../game/ticking.h"
#pragma once
namespace blocks {
	struct Liquid :ecs::component {
		Liquid(double amount) :amt(amount) {
		}
		double amt;
		double min_density = .15f;
		bool loaded = false;
	};
	struct WaterBlock :BlockType {
		WaterBlock() {

		}
		std::string name() const {
			return std::string("water");
		}
		void apply(ecs::obj& block) const override {
			block.add_component<Liquid>(1);
		}
		stn::Option<SolidBlockTraits> solid_traits_for() const override {
			return stn::None;
		}
		void read_from_bytes(ecs::obj blk, stn::file_handle& handle) const {
			apply(blk);

			blk.add_component<Liquid>(stn::file_serializer<double>().read(handle));
		};
		void write_to_bytes(ecs::obj blk, stn::file_handle& handle) const {
			stn::file_serializer<double>().write(blk.get_component<Liquid>().amt, handle);
		};
		BlockMeshTraits traits(BlockTextureRegistry& registry) const {
			block_texture plank_texture = registry.get_texture("images\\water.png");
			return BlockMeshTraits(v3::unit_scale, true, plank_texture);
		}
	};
	inline bool in_water(v3::Coord pos, grid::Grid& world) {
		return world.get_block(pos).is_some_and([](const block& b) {return b.is<WaterBlock>(); });
	}
	struct LiquidateSystem :ecs::System {
		void run(ecs::Ecs& ecs) {

			if (!ecs.get_resource<timing::Ticks>().tick_frame) {
				return;
			}
			BlockRegistry& registry = ecs.get_resource<BlockRegistry>();
		
			grid::Grid& grid = ecs.get_resource<grid::Grid>();
			for (auto [liquid, blk] : ecs::View<Liquid, block>(ecs)) {

				if (!liquid.loaded) {
					liquid.loaded = true;
					continue;
				}
				stn::Option<chunks::block_object&> neighbor_mabye = grid.get_object(v3::Coord(0, -1, 0) + blk.pos);
				if (!neighbor_mabye) {
					continue;
				}
				chunks::block_object blk_below = neighbor_mabye.unwrap();
			
				if (liquid.amt>= liquid.min_density) {
					if (blk_below.get<block>().is<AirBlock>()) {
						grid::set_block(ecs, blk_below.get<block>().pos, blk.id);
						blk_below = grid.get_object(v3::Coord(0, -1, 0) + blk.pos).unwrap();
						blk_below.get_component<Liquid>().amt = 0;
					}
					if (blk_below.get<block>().id==blk.id) {
						Liquid& l = blk_below.get_component<Liquid>();
						if (l.amt < 1) {
							double sink_to = stn::min(1 - l.amt,liquid.amt);
							liquid.amt -= sink_to;
							l.amt += sink_to;

						}
					}
				}
				
				if (liquid.amt < liquid.min_density) {
					grid::set_block(ecs, blk.pos, registry.get_id<AirBlock>());
					continue;
				}
				v3::Coord points[4] = { v3::LeftCoord,v3::RightCoord,v3::FrontCoord,v3::BackCoord };
				stn::array<stn::Tuple<v3::Coord,double>> achieving;
				//in this step we try to minizie the diffrence
				for (v3::Coord pnt : points) {
					v3::Coord pos_at = pnt + blk.pos;
					stn::Option<chunks::block_object> neighbor_mabye = grid.get_object(pos_at).copied();
					if (!neighbor_mabye) {
						continue;
					}
					chunks::block_object blk_along = neighbor_mabye.unwrap();
					if (blk_along.get<block>().solid()) {
						continue;
					}

					if (!blk_along.has_component<Liquid>()) {
						achieving.emplace(pos_at, 0);
					}
					else {
						Liquid& liq = grid.get_object(pos_at).unwrap().get_component<Liquid>();
						if (liq.amt < liquid.amt) {
							achieving.emplace(pos_at, liq.amt);
						}
					}

				}
				achieving | stn::sort([](const stn::Tuple<v3::Coord, double>& liq) {return liq.get<double>(); });
				size_t max = 0;
				double level_reached = 0;
				if (achieving.size() != 0) {

					for (size_t i = 0; i < achieving.size(); i++) {
						double min = achieving[i].get<double>();
						double to = i == achieving.last_index() ? liquid.amt : achieving[i + 1].get<double>();
						double diff = to - min;
						double coverable = stn::min(diff,(liquid.amt-min)/(i+2));
						level_reached = coverable+min<= liquid.min_density?0:coverable + min;
						//min + coverable = liquid.amt - (i + 1) * coverable;
						//liquid.amt-min=(i+2)*coverable
						//liquid.amt-min/(i+2)
						if (to> liquid.min_density&&level_reached==0) {
							break;
						}
						max = i+1;
						liquid.amt -= coverable * (i + 1);
 						if (math::approximate_equals(liquid.amt,level_reached)) {
							break;
						}
					}
					if (level_reached!=0) {

						for (size_t j = 0; j < max; j++) {
							if (!grid.get_object(achieving[j].get<v3::Coord>()).unwrap().has_component<Liquid>()) {
								grid::set_block(ecs, achieving[j].get<v3::Coord>(), blk.id);
							}
							grid.get_object(achieving[j].get<v3::Coord>()).unwrap().get_component<Liquid>().amt = level_reached;
						}
					}
				}

			}



		}
	};
}