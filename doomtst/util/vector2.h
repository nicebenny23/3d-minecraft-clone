#include <cmath>
#include <Windows.h>

#include <glm/glm.hpp>
#ifndef v2_HPP
#define v2_HPP
namespace v2 {
	struct Vector2;
	struct Coord2d
	{

		Coord2d(int X, int Y);
		Coord2d();
		void operator=(const Coord2d& p1);
		bool operator==(const Coord2d& p1);
		bool operator!=(const Coord2d& p1);
		Coord2d& operator+=(const Coord2d& p1);
		Coord2d operator+(const Coord2d& p1) const;

		Coord2d& operator-=(const Coord2d& p1);
		Coord2d operator-(const Coord2d& p1) const;

		Coord2d operator*(int scale) const;
		Coord2d& operator*=(int scale);
		bool operator==(const Vector2& p1);
		Vector2 operator+(const Vector2& p1) const;
		Vector2 operator-(const Vector2& p1) const;
	
		int x;
		int	 y;



	};

	

	inline Coord2d::Coord2d() {



		x = 0;
		y = 0;
	}

	inline float dotproduct(const Coord2d& p, const Coord2d& p1) {

		return (p.x * p1.x + p.y + p1.y);
	}


	

	inline void Coord2d::operator=(const Coord2d& p1) {

		x = p1.x;
		y = p1.y;


	}

	inline bool Coord2d::operator==(const Coord2d& p1)
	{
		return 	(p1.x ==x &&p1.y == y);
	}




	inline Coord2d Coord2d::operator+(const Coord2d& p1) const {


		return Coord2d(x + p1.x, y + p1.y);


	}
	inline Coord2d& Coord2d::operator+=(const Coord2d& p1) {


		x += p1.x;
		y += p1.y;
		return *this;

	}





	inline Coord2d Coord2d::operator-(const Coord2d& p1) const {


		return Coord2d(x - p1.x, y - p1.y);


	}
	inline Coord2d& Coord2d::operator-=(const Coord2d& p1) {


		x -= p1.x;
		y -= p1.y;
		return *this;

	}

	inline Coord2d Coord2d::operator*(int scale) const {

		return Coord2d(x * scale, y * scale);



	}

	inline Coord2d& Coord2d::operator*=(int scale) {


		x *= scale;
		y *= scale;
		return *this;

	}

	


	inline Coord2d::Coord2d(int X, int Y) {

		x = X;
		y = Y;
	}

	inline bool Coord2d::operator!=(const Coord2d& p1)
	{
		return(p1.x != x || p1.y != y);

	}


	struct Vector2
	{

		Vector2(float X, float Y);
		Vector2();
		Vector2(glm::vec2 glm);
		void operator=(const Vector2& p1);
		bool operator==(const Vector2& p1);
		bool operator!=(const Vector2& p1);
		Vector2& operator+=(const Vector2& p1);
		Vector2 operator+(const Vector2& p1) const;

		Vector2& operator-=(const Vector2& p1);
		Vector2 operator-(const Vector2& p1) const;

		Vector2 operator*(float scale) const;
		
		Vector2& operator*=(float scale);


		Vector2 operator/(float scale) const;

		Vector2& operator/=(float scale);
		bool operator==(const Coord2d& p1);
		float x;
		float y;
		void operator= (const Coord2d & p1) ;
		Vector2 operator*(Vector2 scale) const;

	};

	const  Vector2 zerov = Vector2(0, 0);
	const  Vector2 unitv = Vector2(1, 1);
	inline Vector2::Vector2() {



		x = 0;
		y = 0;
	}
	inline Vector2::Vector2(glm::vec2 glm) {



		x = glm.x;
		y = glm.y;
	}
	inline float distance(const Vector2& p, const Vector2& p1) {

		return(sqrt((p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y)));
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
			return zerov;
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
	inline Vector2 Vector2::operator*(Vector2 scale) const {

		return Vector2(x * scale.x, y * scale.y);



	}
	inline Vector2& Vector2::operator*=(float scale) {


		x *= scale;
		y *= scale;
		return *this;

	}

	inline Vector2 Vector2::operator/(float scale) const {

		return Vector2(x / scale, y / scale);
	}

	
	inline Vector2& Vector2::operator/=(float scale) {


		x /= scale;
		y /= scale;
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

	//this section is for stuff defined in v2 incorerating iv2;
	inline void Vector2::operator=(const Coord2d& p1)  
	{
		x = p1.x;
		y = p1.y;

	}

	inline bool Vector2::operator==(const Coord2d& p1)
	{
		return (p1.x == x && p1.y == y);
	}
	//this section is for stuff defined in iv3 incorperating a vector;




	
	inline bool Coord2d::operator==(const Vector2& p1)
	{
		return (p1.x == x && p1.y == y );
	}
	inline Vector2 Coord2d::operator+(const Vector2& p1) const
	{
		return Vector2(p1.x + x, p1.y + y);
	}

	inline Vector2 Coord2d::operator-(const Vector2& p1) const
	{
		return Vector2(x - p1.x, y - p1.y);
	}

}
#endif#pragma once
