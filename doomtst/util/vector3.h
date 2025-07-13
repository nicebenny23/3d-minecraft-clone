#include <format>
#include "mathutil.h"
#include "vector2.h"
#include <glm/glm.hpp>
#pragma once
namespace v3 {
	
	struct Vec3;
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

		Vec3 operator+(const Vec3& p1) const;
		Vec3 operator-(const Vec3& p1) const;
	
		explicit Coord(const Vec3& p1);
		
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




	struct Vec3
	{
		
		glm::vec3 glm();
		Vec3(float X, float Y,float Z);
		Vec3(glm::vec3 glm);
		Vec3(Coord coord);
		Vec3();
		void operator=(const Vec3& p1);
		bool operator==(const Vec3& p1);
		void operator=(const Coord& p1);
		
		bool operator!=(const Vec3& p1);
		Vec3& operator+=(const Vec3& p1);
		Vec3 operator+(const Vec3& p1) const;
		Vec3 operator+(const Coord& p1) const;
		Vec3& operator-=(const Vec3& p1);
		Vec3 operator-(const Vec3& p1) const;
		Vec3 operator*(const Vec3& scale) const;
		Vec3 operator*(float scale) const;
		Vec3& operator*=(float scale);
		float& operator[](size_t index);
		v2::Vec2 xy() const;
		v2::Vec2 xz() const;
		v2::Vec2 yz() const;
	
		Vec3 operator/(float scale) const;

		Vec3& operator/=(float scale);

		

		float x;
		float y;
		float z;


	};

	const  Vec3 zerov = Vec3(0, 0,0);
	const  Vec3 unitv = Vec3(1, 1,1);
	const Vec3 right = Vec3(1, 0, 0);

	const Vec3 up = Vec3(0, 1, 0);

	const Vec3 forward = Vec3(0, 0, 1);
	inline v2::Vec2 Vec3::xy() const {

		return v2::Vec2(x, y);
	}
	inline v2::Vec2 Vec3::xz() const{

		return v2::Vec2(x,z);
	}

	inline v2::Vec2 Vec3::yz() const {
		
		return v2::Vec2(y, z);
	}


	inline Vec3::Vec3() {
		x = 0;
		y = 0;
		z = 0;
	}

	inline Vec3::Vec3(float X, float Y, float Z) {

		x = X;
		y = Y;
		z = Z;
	}
	inline Vec3::Vec3(glm::vec3 glm)
	{
		x = glm.x;
		y = glm.y;
		z = glm.z;

	}
	inline Vec3::Vec3(Coord coord)
	{
		x = coord.x;
		y = coord.y;
		z = coord.z;
	}


	inline Vec3 vec(Coord coord) {
		return Vec3(float(coord.x), float(coord.y),float( coord.z));

	}


	inline void Vec3::operator=(const Vec3& p1) {

		x = p1.x;
		y = p1.y;
		z = p1.z;

	}




	inline Vec3 Vec3::operator+(const Vec3& p1) const {


		return Vec3(x + p1.x, y + p1.y,z+p1.z);


	}
	inline Vec3 Vec3::operator+(const Coord& p1) const
	{
		return Vec3(x + p1.x, y + p1.y, z + p1.z);
	}
	inline Vec3& Vec3::operator+=(const Vec3& p1) {


		x += p1.x;
		y += p1.y;
		z += p1.z;
		return *this;

	}





	inline Vec3 Vec3::operator-(const Vec3& p1) const {


		return Vec3(x - p1.x, y - p1.y,z-p1.z);


	}
	//pointwise scales by the vector
	inline Vec3 Vec3::operator*(const Vec3& scale) const
	{
		
		return Vec3(x*scale.x,y*scale.y,z*scale.z);
	}
	inline Vec3& Vec3::operator-=(const Vec3& p1) {


		x -= p1.x;
		y -= p1.y;
		z -= p1.z;
		return *this;

	}

	inline Vec3 Vec3::operator*(float scale) const {

		return Vec3(x * scale, y * scale,z*scale);



	}

	inline Vec3& Vec3::operator*=(float scale) {


		x *= scale;
		y *= scale;
		z *= scale;
		return *this;

	}
	inline float& Vec3::operator[](size_t index) 
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
			throw std::invalid_argument(std::string(index + " is not a valid index for a Vec3"));
		}
	}
	inline Vec3 Vec3::operator/(float scale) const {

		return Vec3(x / scale, y / scale,z/scale);
	}

	inline Vec3& Vec3::operator/=(float scale) {


		x /= scale;
		y /= scale;
		z /= scale;
		return *this;

	}






	inline bool Vec3::operator==(const Vec3& p1)
	{
		return (p1.x == x && p1.y == y&&p1.z==z);

	}
	
	inline bool Vec3::operator!=(const Vec3& p1)
	{
		return(x!=p1.x  || y!=p1.y ||z!=p1.z);

	}

	inline glm::vec3 Vec3::glm()
	{
		return glm::vec3(x, y, z);
	}


	
	inline float dist2(const Vec3& p, const Vec3& p1) {

		return((p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y) + (p1.z - p.z) * (p1.z - p.z));
	}
	inline float dist(const Vec3& p, const Vec3& p1) {

		return(sqrt(dist2(p, p1)));
	}

	inline float dot(const Vec3& p, const Vec3& p1) {

		return (p.x * p1.x + p.y * p1.y + p.z * p1.z);
	}


	inline float mag(const Vec3& p) {

		return(sqrt(p.x * p.x + p.y * p.y + p.z * p.z));
	}
	inline float mag2(const Vec3& p) {

		return((p.x * p.x + p.y * p.y + p.z * p.z));
	}
	inline Vec3 normal(const Vec3& p) {

		return(p / mag(p));
	}

	inline Vec3 YawPitch(float yaw, float pitch) {

		v3::Vec3 dir = v3::zerov;
		dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		dir.y = sin(glm::radians(pitch));
		dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		return normal(dir);
	}

	inline Vec3 Cross(const Vec3& p, const Vec3& p1) {
		Vec3 crosspoint;
		crosspoint.x = p.y * p1.z - p.z * p1.y;
		crosspoint.y = p.z * p1.x - p.x * p1.z;
		crosspoint.z = p.x * p1.y - p.y * p1.x;
		return crosspoint;
	}


	inline Vec3 lerp(const Vec3& p, const Vec3& p1, float t) {
		return p * (1 - t) + p1 * t;


	}



	//this section is for stuff defined in v3 incorerating iv3;
  inline void Vec3::operator=(const Coord& p1)
	{
	  x = p1.x;
	  y = p1.y;
	  z = p1.z;

	}
	//this section is for stuff defined in iv3 incorperating a vector;
	
   inline Coord::Coord(const Vec3& p1)
	{
	  x = p1.x;
	  y = p1.y;
	  z = p1.z;
	}
 
  inline bool apx(const  Vec3 p, const Vec3& p1)
  {
	  bool c1 = aproxequal(p.x, p1.x);
	  bool c2 = aproxequal(p.y, p1.y);
	  bool c3 = aproxequal(p.z, p1.z);

	  return c1 && c2 && c3;
  }


	inline Vec3 Coord::operator+(const Vec3& p1) const
	{
		return Vec3(p1.x + x, p1.y + y, p1.z + z);
	}
  
	inline Vec3 Coord::operator-(const Vec3& p1) const
	{
		return Vec3(x-p1.x , y-p1.y , z-p1.z );
	}

	inline bool operator==(const Vec3& p1, const Coord& p2)
	{
		return (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z);
	}
	inline Vec3 operator*(float scale,const Vec3& vec)  {
		return vec * scale;
	}

}

template <>
struct std::formatter<v3::Vec3> : std::formatter<std::string> {
	// Parses format specifiers (e.g., "{:.2f}")
	template <typename FormatContext>
	auto format(const v3::Vec3& v, FormatContext& ctx) {
		// Format the Vec3 as (x, y, z)
		return std::format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
	}
};