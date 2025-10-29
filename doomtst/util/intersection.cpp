#pragma once
#include "intersection.h"
#include "interval.h"
namespace geointersect {
	boxRayCollision intersection(geometry::Box box, ray fray)
	{
		v3::Vec3 dir = fray.dir();

		//not actually max

		double min_max= std::numeric_limits<double>::infinity();
		double max_min= -std::numeric_limits<double>::infinity();
		for (int i = 0; i < 3; i++)
		{
			//potential closest hits on axis
			double tst_val_1= box.center[i] - box.scale[i] - fray.start[i];

			double tst_val_2 = box.center[i] + box.scale[i] - fray.start[i];
			if (apxf(dir[i], 0))
			{
				if (inter::range(tst_val_1,tst_val_2).apx_contains(0))
				{
					continue;
				}
			}
			{
				tst_val_1 /= dir[i];
				tst_val_2 /= dir[i];
			}
			min_max= Min(min_max, Max(tst_val_1, tst_val_2));
			max_min =Max(max_min, Min(tst_val_1, tst_val_2));
		}
		


		if (max_min< min_max||apxf(max_min,min_max))
		{
			
			if (0 < max_min||apxf(max_min, 0))
			{
				return stn::Construct<RayHit>(fray, max_min);
			}

		}


		return stn::None;


	}
	bool intersects(geometry::cone cone, geometry::Sphere sph)
	{
		return cone.distanceFromPoint(sph.center) <= sph.radius;

	}
}