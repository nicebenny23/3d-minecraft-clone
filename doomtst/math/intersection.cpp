#pragma once
#include "intersection.h"
#include "interval.h"
namespace geointersect {
	boxRayCollision intersection(geo::Box box, ray fray) {
		v3::Vec3 dir = fray.dir();

		//not actually max

		double min_max = std::numeric_limits<double>::infinity();
		double max_min = -std::numeric_limits<double>::infinity();
		for (int i = 0; i < 3; i++) {
			double tst_val_1 = box.center[i] - box.scale[i] - fray.start[i];
			double tst_val_2 = box.center[i] + box.scale[i] - fray.start[i];
			if (apx(dir[i], 0)) {
				if (math::range(tst_val_1, tst_val_2).apx_contains(0)) {
					continue;
				}
			}
			tst_val_1 /= dir[i];
			tst_val_2 /= dir[i];
			min_max =stn::Min(min_max, stn::Max(tst_val_1, tst_val_2));
			max_min = stn::Max(max_min, stn::Min(tst_val_1, tst_val_2));
		}



		if (max_min < min_max || apx(max_min, min_max)) {

			if (0 < max_min || apx(max_min, 0)) {
				return stn::MakeOption<RayHit>(fray, max_min);
			}

		}


		return stn::None;


	}
	bool intersects(geo::cone cone, geo::Sphere sph) {
		return cone.distance_from_point(sph.center) <= sph.radius;

	}
}