#include <cmath>
#include "mathutil.h"
#include <Windows.h>
#include "vector2.h"
#include <glm/glm.hpp>
#pragma once
namespace v3 {
	
	struct Vector3;
	struct Coord
	{
		
		
		Coord(int X, int Y, int Z);
		
		Coord();
		void operator=(const Coord& p1);
		bool operator==(const Coord& p1);
		
		bool operator!=(const Coord& p1);
	
		Coord& operator+=(const Coord& p1);
		Coord operator+(const Coord& p1) const;
		Coord& operator-=(const Coord& p1);
		Coord operator-(const Coord& p1) const;
	
		Coord operator*(int scale) const;
		Coord& operator*=(int scale);

		
		
		int x;
		int y;
		int z;

		Vector3 operator+(const Vector3& p1) const;
		Vector3 operator-(const Vector3& p1) const;
	
		explicit Coord(const Vector3& p1);
		
	};

	const  Coord zeroiv = Coord(0, 0, 0);

	inline Coord::Coord() {



		x = 0;
		y = 0;
		z = 0;
	}

	inline Coord::Coord(int X, int Y, int Z) {

		x = X;
		y = Y;
		z = Z;
	}




	

	inline void Coord::operator=(const Coord& p1) {

		x = p1.x;
		y = p1.y;
		z = p1.z;

	}




	inline Coord Coord::operator+(const Coord& p1) const {


		return Coord(x + p1.x, y + p1.y, z + p1.z);


	}
	
	inline Coord& Coord::operator+=(const Coord& p1) {


		x += p1.x;
		y += p1.y;
		z += p1.z;
		return *this;

	}





	inline Coord Coord::operator-(const Coord& p1) const {


		return Coord(x - p1.x, y - p1.y, z - p1.z);


	}
	inline Coord& Coord::operator-=(const Coord& p1) {


		x -= p1.x;
		y -= p1.y;
		z -= p1.z;
		return *this;

	}

	inline Coord Coord::operator*(int scale) const {

		return Coord(x * scale, y * scale, z * scale);



	}

	inline Coord& Coord::operator*=(int scale) {


		x *= scale;
		y *= scale;
		z *= scale;
		return *this;

	}

	
	
	inline bool Coord::operator==(const Coord& p1)
	{
		return (p1.x == x && p1.y == y && p1.z == z);

	}
	inline bool Coord::operator!=(const Coord& p1)
	{
		return(p1.x != x || p1.y != y || z != p1.z);

	}




	struct Vector3
	{
		
		glm::vec3 glm();
		Vector3(float X, float Y,float Z);
		Vector3(glm::vec3 glm);
		Vector3(Coord coord);
		Vector3();
		void operator=(const Vector3& p1);
		bool operator==(const Vector3& p1);
		void operator=(const Coord& p1);
		
		bool operator!=(const Vector3& p1);
		Vector3& operator+=(const Vector3& p1);
		Vector3 operator+(const Vector3& p1) const;
		Vector3 operator+(const Coord& p1) const;
		Vector3& operator-=(const Vector3& p1);
		Vector3 operator-(const Vector3& p1) const;
		Vector3 operator*(const Vector3& scale) const;
		Vector3 operator*(float scale) const;
		Vector3& operator*=(float scale);
		float& operator[](size_t index);
		v2::Vector2 xy() const;
		v2::Vector2 xz() const;
		v2::Vector2 yz() const;
	
		Vector3 operator/(float scale) const;

		Vector3& operator/=(float scale);

		

		float x;
		float y;
		float z;


	};

	const  Vector3 zerov = Vector3(0, 0,0);
	const  Vector3 unitv = Vector3(1, 1,1);
	const Vector3 right = Vector3(1, 0, 0);

	const Vector3 up = Vector3(0, 1, 0);

	const Vector3 forward = Vector3(0, 0, 1);
	inline v2::Vector2 Vector3::xy() const {

		return v2::Vector2(x, y);
	}
	inline v2::Vector2 Vector3::xz() const{

		return v2::Vector2(x,z);
	}

	inline v2::Vector2 Vector3::yz() const {
		
		return v2::Vector2(y, z);
	}


	inline Vector3::Vector3() {
		x = 0;
		y = 0;
		z = 0;
	}

	inline Vector3::Vector3(float X, float Y, float Z) {

		x = X;
		y = Y;
		z = Z;
	}
	inline Vector3::Vector3(glm::vec3 glm)
	{
		x = glm.x;
		y = glm.y;
		z = glm.z;

	}
	inline Vector3::Vector3(Coord coord)
	{
		x = coord.x;
		y = coord.y;
		z = coord.z;
	}


	inline Vector3 vec(Coord coord) {
		return Vector3(float(coord.x), float(coord.y),float( coord.z));

	}


	inline void Vector3::operator=(const Vector3& p1) {

		x = p1.x;
		y = p1.y;
		z = p1.z;

	}




	inline Vector3 Vector3::operator+(const Vector3& p1) const {


		return Vector3(x + p1.x, y + p1.y,z+p1.z);


	}
	inline Vector3 Vector3::operator+(const Coord& p1) const
	{
		return Vector3(x + p1.x, y + p1.y, z + p1.z);
	}
	inline Vector3& Vector3::operator+=(const Vector3& p1) {


		x += p1.x;
		y += p1.y;
		z += p1.z;
		return *this;

	}





	inline Vector3 Vector3::operator-(const Vector3& p1) const {


		return Vector3(x - p1.x, y - p1.y,z-p1.z);


	}
	//pointwise scales by the vector
	inline Vector3 Vector3::operator*(const Vector3& scale) const
	{
		
		return Vector3(x*scale.x,y*scale.y,z*scale.z);
	}
	inline Vector3& Vector3::operator-=(const Vector3& p1) {


		x -= p1.x;
		y -= p1.y;
		z -= p1.z;
		return *this;

	}

	inline Vector3 Vector3::operator*(float scale) const {

		return Vector3(x * scale, y * scale,z*scale);



	}

	inline Vector3& Vector3::operator*=(float scale) {


		x *= scale;
		y *= scale;
		z *= scale;
		return *this;

	}
	inline float& Vector3::operator[](size_t index) 
	{
		switch (index) {
		case 0:
			return x;
			break;
		case 1:
			return y;
			break;
		case 2:
			return z;
			break;
		default:
			throw std::invalid_argument(std::string(index + " is not a valid index for a Vector3"));
		}
	}
	inline Vector3 Vector3::operator/(float scale) const {

		return Vector3(x / scale, y / scale,z/scale);
	}

	inline Vector3& Vector3::operator/=(float scale) {


		x /= scale;
		y /= scale;
		z /= scale;
		return *this;

	}






	inline bool Vector3::operator==(const Vector3& p1)
	{
		return (p1.x == x && p1.y == y&&p1.z==z);

	}
	
	inline bool Vector3::operator!=(const Vector3& p1)
	{
		return(x!=p1.x  || y!=p1.y ||z!=p1.z);

	}

	inline glm::vec3 Vector3::glm()
	{
		return glm::vec3(x, y, z);
	}


	
	inline float dist2(const Vector3& p, const Vector3& p1) {

		return((p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y) + (p1.z - p.z) * (p1.z - p.z));
	}
	inline float dist(const Vector3& p, const Vector3& p1) {

		return(sqrt(dist2(p, p1)));
	}

	inline float dot(const Vector3& p, const Vector3& p1) {

		return (p.x * p1.x + p.y * p1.y + p.z * p1.z);
	}


	inline float mag(const Vector3& p) {

		return(sqrt(p.x * p.x + p.y * p.y + p.z * p.z));
	}
	inline float mag2(const Vector3& p) {

		return((p.x * p.x + p.y * p.y + p.z * p.z));
	}
	inline Vector3 normal(const Vector3& p) {

		return(p / mag(p));
	}

	inline Vector3 YawPitch(float yaw, float pitch) {

		v3::Vector3 dir = v3::zerov;
		dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		dir.y = sin(glm::radians(pitch));
		dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		return normal(dir);
	}

	inline Vector3 Cross(const Vector3& p, const Vector3& p1) {
		Vector3 crosspoint;
		crosspoint.x = p.y * p1.z - p.z * p1.y;
		crosspoint.y = p.z * p1.x - p.x * p1.z;
		crosspoint.z = p.x * p1.y - p.y * p1.x;
		return crosspoint;
	}


	inline Vector3 lerp(const Vector3& p, const Vector3& p1, float t) {
		return p * (1 - t) + p1 * t;


	}



	//this section is for stuff defined in v3 incorerating iv3;
  inline void Vector3::operator=(const Coord& p1)
	{
	  x = p1.x;
	  y = p1.y;
	  z = p1.z;

	}
	//this section is for stuff defined in iv3 incorperating a vector;
	
   inline Coord::Coord(const Vector3& p1)
	{
	  x = p1.x;
	  y = p1.y;
	  z = p1.z;
	}
 
  inline bool apx(const  Vector3 p, const Vector3& p1)
  {
	  bool c1 = aproxequal(p.x, p1.x);
	  bool c2 = aproxequal(p.y, p1.y);
	  bool c3 = aproxequal(p.z, p1.z);

	  return c1 && c2 && c3;
  }


	inline Vector3 Coord::operator+(const Vector3& p1) const
	{
		return Vector3(p1.x + x, p1.y + y, p1.z + z);
	}
  
	inline Vector3 Coord::operator-(const Vector3& p1) const
	{
		return Vector3(x-p1.x , y-p1.y , z-p1.z );
	}

	inline bool operator==(const Vector3& p1, const Coord& p2)
	{
		return (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z);
	}
	
}
