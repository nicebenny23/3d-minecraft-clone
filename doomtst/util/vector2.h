#include <cmath>
#include <Windows.h>

#ifndef v2_HPP
#define v2_HPP
namespace iv2 {
	struct IntVector2
	{

		IntVector2(int X, int Y);
		IntVector2();
		void operator=(const IntVector2& p1);
		bool operator==(const IntVector2& p1);
		bool operator!=(const IntVector2& p1);
		IntVector2& operator+=(const IntVector2& p1);
		IntVector2 operator+(const IntVector2& p1) const;

		IntVector2& operator-=(const IntVector2& p1);
		IntVector2 operator-(const IntVector2& p1) const;

		IntVector2 operator*(int scale) const;
		IntVector2& operator*=(int scale);


	
		IntVector2 operator*(IntVector2& scale) const;
		IntVector2& operator*=(IntVector2& scale);

		IntVector2 operator/(IntVector2& scale) const;
		IntVector2& operator/=(IntVector2& scale);

		float x;
		float y;



	};

	const  IntVector2 zerov = IntVector2(0, 0);
	const  IntVector2 unitv = IntVector2(1, 1);

	inline IntVector2::IntVector2() {



		x = 0;
		y = 0;
	}

	
	inline IntVector2 sgn(const IntVector2& p, const IntVector2& p1) {


		//sgn of p-p1
		IntVector2 retv = IntVector2(1, 1);

		if (p1.x > p.x)
		{
			retv.x *= -1;
		}
		if (p1.y > p.y)
		{
			retv.y *= -1;
		}
		return  retv;
	}
	

	inline float dotproduct(const IntVector2& p, const IntVector2& p1) {

		return (p.x * p1.x + p.y + p1.y);
	}


	

	inline void IntVector2::operator=(const IntVector2& p1) {

		x = p1.x;
		y = p1.y;


	}

	inline bool IntVector2::operator==(const IntVector2& p1)
	{
		return 	(p1.x ==x &&p1.y == y);
	}




	inline IntVector2 IntVector2::operator+(const IntVector2& p1) const {


		return IntVector2(x + p1.x, y + p1.y);


	}
	inline IntVector2& IntVector2::operator+=(const IntVector2& p1) {


		x += p1.x;
		y += p1.y;
		return *this;

	}





	inline IntVector2 IntVector2::operator-(const IntVector2& p1) const {


		return IntVector2(x - p1.x, y - p1.y);


	}
	inline IntVector2& IntVector2::operator-=(const IntVector2& p1) {


		x -= p1.x;
		y -= p1.y;
		return *this;

	}

	inline IntVector2 IntVector2::operator*(int scale) const {

		return IntVector2(x * scale, y * scale);



	}

	inline IntVector2& IntVector2::operator*=(int scale) {


		x *= scale;
		y *= scale;
		return *this;

	}

	

	inline IntVector2 IntVector2::operator*(IntVector2& scale) const {

		return IntVector2(x * scale.x, y * scale.y);
	}


	inline IntVector2& IntVector2::operator*=(IntVector2& scale) {
		x *= scale.x;
		y *= scale.y;
		return *this;
	}

	inline IntVector2 IntVector2::operator/(IntVector2& scale) const {

		return IntVector2(x / scale.x, y / scale.y);
	}


	inline IntVector2& IntVector2::operator/=(IntVector2& scale) {


		x /= scale.x;
		y /= scale.y;
		return *this;

	}


	inline IntVector2::IntVector2(int X, int Y) {

		x = X;
		y = Y;
	}

	inline bool IntVector2::operator!=(const IntVector2& p1)
	{
		return(p1.x != x || p1.y != y);

	}

}
namespace v2
{
	struct Vector2
	{

		Vector2(float X, float Y);
		Vector2();
		void operator=(const Vector2& p1);
		bool operator==(const Vector2& p1);
		bool operator!=(const Vector2& p1);
		Vector2& operator+=(const Vector2& p1);
		Vector2 operator+(const Vector2& p1) const;

		Vector2& operator-=(const Vector2& p1);
		Vector2 operator-(const Vector2& p1) const;

		Vector2 operator*(float scale) const;
		Vector2 operator*(const Vector2& p1) const;
		Vector2& operator*=(float scale);


		Vector2 operator/(float scale) const;

		Vector2& operator/=(float scale);

		Vector2 operator*(const Vector2& scale);
		Vector2& operator*=(Vector2& scale);

		Vector2 operator/(Vector2& scale) const;
		Vector2& operator/=(Vector2& scale);

		float x;
		float y;



	};

	const  Vector2 zerov = Vector2(0, 0);
	const  Vector2 unitv = Vector2(1, 1);
	inline Vector2 Coordtovector(COORD pos) {


		return Vector2(pos.X, -pos.Y);
	}
	inline Vector2::Vector2() {



		x = 0;
		y = 0;
	}

	inline float distance(const Vector2& p, const Vector2& p1) {

		return(sqrt((p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y)));
	}
	
	
	inline Vector2 rotpnt(Vector2 p, const Vector2 p1, float degrees) {

		float c = cos(degrees);
		float s = sin(degrees);
		p -= p1;


		p = Vector2(p.x * c - p.y * s, p.x * s + p.y * c);
		return (p + p1);
	}

	inline float angx(Vector2 p, const Vector2 p1) {


		return atan2(p.y - p1.y, p.x - p1.x);

	}

	inline iv2::IntVector2 intv(Vector2 p) {

		return iv2::IntVector2(floor(p.x), floor(p.y));
	}

	inline Vector2 rot(Vector2 p, float degrees) {

		float c = cos(degrees);
		float s = sin(degrees);

		p = Vector2(p.x * c - p.y * s, p.x * s + p.y * c);
		return (p);
	}
	inline Vector2 rot(Vector2 p, float mag, float degrees) {

		float c = cos(degrees);
		float s = sin(degrees);

		return Vector2(c, s) * mag + p;

	}

	inline bool Vector2::operator==(const Vector2& p1)
	{
		return (p1.x == x && p1.y == y);

	}


	inline float magnitude(const Vector2& p) {

		return(sqrt(p.x * p.x + p.y * p.y));
	}
	inline Vector2 normal(Vector2 p) {
		if (p == zerov)
		{

		}
		return(p / magnitude(p));
	}
	
	inline float dotproduct(const Vector2& p, const Vector2& p1) {

		return (p.x * p1.x + p.y + p1.y);
	}


	inline Vector2 lerp(const Vector2& p, const Vector2& p1, float t) {
		return p * (1 - t) + p1 * t;


	}

	inline float slope(const Vector2& p, const Vector2& p1) {
		return (p1.y - p.y) / (p1.x - p.x);


	}

	inline void Vector2::operator=(const Vector2& p1) {

		x = p1.x;
		y = p1.y;


	}




	inline Vector2 Vector2::operator+(const Vector2& p1) const {


		return Vector2(x + p1.x, y + p1.y);


	}
	inline Vector2& Vector2::operator+=(const Vector2& p1) {


		x += p1.x;
		y += p1.y;
		return *this;

	}





	inline Vector2 Vector2::operator-(const Vector2& p1) const {


		return Vector2(x - p1.x, y - p1.y);


	}
	inline Vector2& Vector2::operator-=(const Vector2& p1) {


		x -= p1.x;
		y -= p1.y;
		return *this;

	}

	inline Vector2 Vector2::operator*(float scale) const {

		return Vector2(x * scale, y * scale);



	}

	inline Vector2 Vector2::operator*(const Vector2& p1) const
	{
		return Vector2(x * p1.x, y * p1.y);
	}

	inline Vector2& Vector2::operator*=(float scale) {


		x *= scale;
		y *= scale;
		return *this;

	}

	inline Vector2 Vector2::operator/(float scale) const {

		return Vector2(x / scale, y / scale);
	}

	/*inline bool lineinter(Vector2 a, Vector2 b, Vector2 c, Vector2 d) {

		float dy = (a.y - b.y);

		float dy1 = (c.y - d.y);
		if (dy == 0) {




			return false;

		}
		if (dy1 == 0)
		{



			return false;

		}
		float dx = (a.x - b.x);

		float dx1 = (c.x - d.x);


		if (dx == dx1)
		{


			if (a == c || b == c || a == d || b == d)
			{
				return true;
			}
			return false;
		}

		float s = dy / dx;
		float s1 = dy1 / dx1;
		float yc = a.y - a.x * s;
		float yc1 = c.y - c.x * s1;

		float x = (yc1 - yc) / (s - s1);
		Vector2 point = Vector2(x, x * s + yc);


		if (point.x< max(a.x, b.x) && point.x >min(a.x, b.x))
		{
			if (point.x< max(d.x, c.x) && point.x >min(c.x, d.x))
			{
				return true;
			}


		}

		return false;
	}
	*/
	
	inline Vector2& Vector2::operator/=(float scale) {


		x /= scale;
		y /= scale;
		return *this;

	}


	inline Vector2 Vector2::operator*(const Vector2& scale)  {

		return Vector2(x * scale.x, y * scale.y);
	}


	inline Vector2& Vector2::operator*=(Vector2& scale) {
		x *= scale.x;
		y *= scale.y;
		return *this;
	}

	inline Vector2 Vector2::operator/(Vector2& scale) const {

		return Vector2(x / scale.x, y / scale.y);
	}


	inline Vector2& Vector2::operator/=(Vector2& scale) {


		x /= scale.x;
		y /= scale.y;
		return *this;

	}


	inline Vector2::Vector2(float X, float Y) {

		x = X;
		y = Y;
	}

	inline bool Vector2::operator!=(const Vector2& p1)
	{
		return(p1.x != x || p1.y != y);

	}
}
#endif#pragma once
