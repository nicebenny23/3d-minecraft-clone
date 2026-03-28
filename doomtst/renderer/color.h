#include "../math/mathutil.h"

#pragma once
namespace colors {
	struct Color {
		Color(double red, double green, double blue, double alpha) :r(red), g(green), b(blue), a(alpha) {

		}
		Color():r(1),g(1),b(1),a(1) {

		}
		double r;
		double g;
		double b;
		double a;
		Color clamped() const{
		return Color(math::clamp(r, 0.0, 1.0),
			math::clamp(g, 0.0, 1.0),
			math::clamp(b, 0.0, 1.0),
			math::clamp(a, 0.0, 1.0));
		}
		Color clamp_self() {
			*this = clamped();
		}
		Color operator*(double v) const {
			return Color(r * v, g * v, b * v, a).clamped();
		}
		Color& operator*=(double v) {
			*this = *this * v;
		}
		glm::vec4 glm() const {
			return glm::vec4(r, g, b, a);
		}
	};

	static inline const Color White{1, 1, 1, 1 };
	static inline const Color Black{ 0, 0, 0, 1 };
	static inline const Color Red{ 1, 0, 0, 1 };
	static inline const Color Green{ 0, 1, 0, 1 };
	static inline const Color Blue{ 0, 0, 1, 1 };
	static inline const Color Yellow{ 1, 1, 0, 1 };
	static inline const Color Cyan{ 0, 1, 1, 1 };
	static inline const Color Magenta{ 1, 0, 1, 1 };
	static inline const Color Gray{ 0.5f, 0.5f, 0.5f, 1 };
	static inline const Color LightGray{ 0.75f, 0.75f, 0.75f, 1 };
	static inline const Color DarkGray{ 0.25f, 0.25f, 0.25f, 1 };
	static inline const Color Transparent{ 0, 0, 0, 0 };
}