#include <cmath>
#include <Windows.h>

#include <glm/glm.hpp>
#pragma once 
namespace v2 {
	struct Vec2;
	struct Coord2
	{

		Coord2(int X, int Y);
		Coord2();
		void operator=(const Coord2& p1);
		bool operator==(const Coord2& p1);
		bool operator!=(const Coord2& p1);
		Coord2& operator+=(const Coord2& p1);
		Coord2 operator+(const Coord2& p1) const;

		Coord2& operator-=(const Coord2& p1);
		Coord2 operator-(const Coord2& p1) const;

		Coord2 operator*(int scale) const;
		Coord2& operator*=(int scale);
		bool operator==(const Vec2& p1);
		Vec2 operator+(const Vec2& p1) const;
		Vec2 operator-(const Vec2& p1) const;
	
		int x;
		int y;



	};

	

	inline Coord2::Coord2() {



		x = 0;
		y = 0;
	}



	

	inline void Coord2::operator=(const Coord2& p1) {

		x = p1.x;
		y = p1.y;


	}

	inline bool Coord2::operator==(const Coord2& p1)
	{
		return 	(p1.x ==x &&p1.y == y);
	}




	inline Coord2 Coord2::operator+(const Coord2& p1) const {


		return Coord2(x + p1.x, y + p1.y);


	}
	inline Coord2& Coord2::operator+=(const Coord2& p1) {


		x += p1.x;
		y += p1.y;
		return *this;

	}





	inline Coord2 Coord2::operator-(const Coord2& p1) const {


		return Coord2(x - p1.x, y - p1.y);


	}
	inline Coord2& Coord2::operator-=(const Coord2& p1) {


		x -= p1.x;
		y -= p1.y;
		return *this;

	}

	inline Coord2 Coord2::operator*(int scale) const {

		return Coord2(x * scale, y * scale);



	}

	inline Coord2& Coord2::operator*=(int scale) {


		x *= scale;
		y *= scale;
		return *this;

	}

	


	inline Coord2::Coord2(int X, int Y) {

		x = X;
		y = Y;
	}

	inline bool Coord2::operator!=(const Coord2& p1)
	{
		return(p1.x != x || p1.y != y);

	}


	struct Vec2
	{

		Vec2(float X, float Y);
		Vec2();
		Vec2(glm::vec2 glm);
		void operator=(const Vec2& p1);
		bool operator==(const Vec2& p1);
		bool operator!=(const Vec2& p1);
		Vec2& operator+=(const Vec2& p1);
		Vec2 operator+(const Vec2& p1) const;

		Vec2& operator-=(const Vec2& p1);
		Vec2 operator-(const Vec2& p1) const;

		Vec2 operator*(float scale) const;
		
		Vec2& operator*=(float scale);


		Vec2 operator/(float scale) const;

		Vec2& operator/=(float scale);
		bool operator==(const Coord2& p1);
		float x;
		float y;
		void operator= (const Coord2 & p1) ;
		Vec2 operator*(Vec2 scale) const;

	};

	const  Vec2 zerov = Vec2(0, 0);
	const  Vec2 unitv = Vec2(1, 1);
	inline Vec2::Vec2() {



		x = 0;
		y = 0;
	}
	inline Vec2::Vec2(glm::vec2 glm) {



		x = glm.x;
		y = glm.y;
	}
	inline float distance(const Vec2& p, const Vec2& p1) {

		return(sqrt((p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y)));
	}
	
	

	inline bool Vec2::operator==(const Vec2& p1)
	{
		return (p1.x == x && p1.y == y);

	}


	inline float magnitude(const Vec2& p) {

		return(sqrt(p.x * p.x + p.y * p.y));
	}
	inline Vec2 normal(Vec2 p) {
		if (p == zerov)
		{
			return zerov;
 		}
		return(p / magnitude(p));
	}
	
	inline float dotproduct(const Vec2& p, const Vec2& p1) {

		return (p.x * p1.x + p.y + p1.y);
	}


	inline Vec2 lerp(const Vec2& p, const Vec2& p1, float t) {
		return p * (1 - t) + p1 * t;


	}

	inline float slope(const Vec2& p, const Vec2& p1) {
		return (p1.y - p.y) / (p1.x - p.x);


	}

	inline void Vec2::operator=(const Vec2& p1) {

		x = p1.x;
		y = p1.y;


	}




	inline Vec2 Vec2::operator+(const Vec2& p1) const {


		return Vec2(x + p1.x, y + p1.y);


	}
	inline Vec2& Vec2::operator+=(const Vec2& p1) {


		x += p1.x;
		y += p1.y;
		return *this;

	}





	inline Vec2 Vec2::operator-(const Vec2& p1) const {


		return Vec2(x - p1.x, y - p1.y);


	}
	inline Vec2& Vec2::operator-=(const Vec2& p1) {


		x -= p1.x;
		y -= p1.y;
		return *this;

	}

	inline Vec2 Vec2::operator*(float scale) const {

		return Vec2(x * scale, y * scale);



	}
	inline Vec2 Vec2::operator*(Vec2 scale) const {

		return Vec2(x * scale.x, y * scale.y);



	}
	inline Vec2& Vec2::operator*=(float scale) {


		x *= scale;
		y *= scale;
		return *this;

	}

	inline Vec2 Vec2::operator/(float scale) const {

		return Vec2(x / scale, y / scale);
	}

	
	inline Vec2& Vec2::operator/=(float scale) {


		x /= scale;
		y /= scale;
		return *this;

	}


	
	inline Vec2::Vec2(float X, float Y) {

		x = X;
		y = Y;
	}

	inline bool Vec2::operator!=(const Vec2& p1)
	{
		return(p1.x != x || p1.y != y);

	}

	//this section is for stuff defined in v2 incorerating iv2;
	inline void Vec2::operator=(const Coord2& p1)  
	{
		x = p1.x;
		y = p1.y;

	}

	inline bool Vec2::operator==(const Coord2& p1)
	{
		return (p1.x == x && p1.y == y);
	}
	//this section is for stuff defined in iv3 incorperating a vector;




	
	inline bool Coord2::operator==(const Vec2& p1)
	{
		return (p1.x == x && p1.y == y );
	}
	inline Vec2 Coord2::operator+(const Vec2& p1) const
	{
		return Vec2(p1.x + x, p1.y + y);
	}

	inline Vec2 Coord2::operator-(const Vec2& p1) const
	{
		return Vec2(x - p1.x, y - p1.y);
	}

}