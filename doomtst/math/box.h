#include "vector3.h"
#include "dir.h"
#include "cube_vertex.h"
#pragma once
namespace geo {
	struct Box {
		v3::Point3 center;
		v3::Scale3 scale;

		Box(v3::Point3 cent, v3::Scale3 scl) : center(cent), scale(scl) {
			int l = 3;
		}
		static Box from_min_max(v3::Point3 min, v3::Point3 max) {
			v3::Vec3 scale = max - min;
			return Box(v3::lerp(max,min,1/2.0f), v3::Scale3(std::abs(scale.x), std::abs(scale.y), std::abs(scale.z)));
		}
		v3::Point3 max() const {
			return center + v3::Vec3(half_size());
		}
		v3::Point3 point_at_vertex(math::cube_index index) {
			return index.vertex_vector()*scale+min();
		}
		v3::Point3 min() const {
			return center - v3::Vec3(half_size());
		}
		double diameter() const {
			return v3::Vec3(scale.x,scale.y,scale.z).length();
		}
		double radius() const {
			return diameter()/2.0;
		}
		bool contains_point(v3::Point3 pos) const {
			v3::Vec3 shifted = center - pos;
			v3::Scale3 bounds = half_size();
			if (std::abs(shifted.x) <= bounds.x) {
				if (std::abs(shifted.y) <= bounds.y) {
					if (std::abs(shifted.z) <= bounds.z) {
						return true;
					}
				}
			}
			return false;
		}
		v3::Point3 in_direction(math::Direction3d direction) {
			return direction.vec() * half_size() + center;
		}

		bool contains_box(Box b) const {
			return contains_point(b.max()) && contains_point(b.min());
		}
		Box with_center(v3::Point3 new_center) const {
			return Box(new_center, scale);
		}
		Box with_scale(v3::Scale3 new_scale) const {
			return Box(center, new_scale);
		}
		v3::Scale3 half_size() const {
			return scale / 2;
		}
		Box expanded(v3::Scale3 size) const {
			return Box(center, size+scale);
		}
		Box expanded(double size) const {
			return Box(center, scale.expanded(size));
		}
		// Minkowski difference operator
		Box operator-(const Box& other) const {
			//minkoski diffrence changes affinity
			return Box(center-other.center, scale+other.scale);
		}
		Box translated(v3::Vec3 translation_vector) const {
			return Box(center + translation_vector, scale);
		}
		Box scaled_from_center(float dialation) const {
			return Box(center, scale * dialation);
		}

		Box transform(Box other_transform) const {
			return Box(center+other_transform.center * scale, other_transform.scale * scale);
		}
	};

	struct IntBox3d {
		v3::Coord min;
		v3::Coord max;
		struct iterator {
			const IntBox3d& owner;
			v3::Coord pnt;
			iterator& operator++() {
				pnt.x++;
				for (size_t i = 0; i < 3; i++) {
					if (pnt[i]>owner.max[i]) {
						if (i != 2) {
							pnt[i + 1]++;
							pnt[i] = owner.min[i];
						}
					}
				}
				return *this;
			}
			v3::Coord operator*() const {
				return pnt;
			}
			bool operator==(const iterator& other) const {
				return other.pnt == pnt;
			}
			bool operator!=(const iterator& other) const {
				return other.pnt != pnt;
			};
		};
		IntBox3d(v3::Coord min, v3::Coord max) :min(min), max(max) {
			for (size_t i = 0; i < 3; i++) {
				if (min[i] > max[i]) {
					std::swap(min[i], max[i]);
				}
			}
		}
		iterator begin() const {
			return iterator{ .owner = *this,.pnt = min };
		}
		iterator end() const {
			return iterator{ .owner = *this,.pnt = v3::Coord(min.x,min.y,max.z+1)};
		}
	};
}