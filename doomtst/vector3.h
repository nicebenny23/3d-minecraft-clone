#include <cmath>
#include <Windows.h>
#include <glm/glm.hpp>
#ifndef v3_HPP
#define v3_HPP
namespace iv3 {
	inline int floorabs(float x) {

		if (x < 0)
		{
			return -1 * floor(-1 * x);
		}
		else
		{
			return floor(x);
		}
	}
	inline int ceilabs(float x) {

		if (x < 0)
		{
			return -1 * ceil(-1 * x);
		}
		else
		{
			return ceil(x);
		}
	}
	struct Ivector3
	{
		glm::vec3 glm();
		Ivector3(int X, int Y, int Z);
		Ivector3(glm::vec3 glm);
		Ivector3();
		void operator=(const Ivector3& p1);
		bool operator==(const Ivector3& p1);
		bool operator!=(const Ivector3& p1);
		Ivector3& operator+=(const Ivector3& p1);
		Ivector3 operator+(const Ivector3& p1) const;

		Ivector3& operator-=(const Ivector3& p1);
		Ivector3 operator-(const Ivector3& p1) const;

		Ivector3 operator*(int scale) const;
		Ivector3& operator*=(int scale);



		Ivector3 operator*(Ivector3& scale) const;
		Ivector3& operator*=(Ivector3& scale);

		
		int x;
		int y;
		int z;


	};

	const  Ivector3 zerov = Ivector3(0, 0, 0);
	const  Ivector3 unitv = Ivector3(1, 1, 1);

	inline Ivector3::Ivector3() {



		x = 0;
		y = 0;
		z = 0;
	}





	inline int dotproduct(const Ivector3& p, const Ivector3& p1) {

		return (p.x * p1.x + p.y + p1.y);
	}


	inline Ivector3 lerp(const Ivector3& p, const Ivector3& p1, int t) {
		return p * (1 - t) + p1 * t;


	}


	inline void Ivector3::operator=(const Ivector3& p1) {

		x = p1.x;
		y = p1.y;
		z = p1.z;

	}




	inline Ivector3 Ivector3::operator+(const Ivector3& p1) const {


		return Ivector3(x + p1.x, y + p1.y, z + p1.z);


	}
	inline Ivector3& Ivector3::operator+=(const Ivector3& p1) {


		x += p1.x;
		y += p1.y;
		z += p1.z;
		return *this;

	}





	inline Ivector3 Ivector3::operator-(const Ivector3& p1) const {


		return Ivector3(x - p1.x, y - p1.y, z - p1.z);


	}
	inline Ivector3& Ivector3::operator-=(const Ivector3& p1) {


		x -= p1.x;
		y -= p1.y;
		z -= p1.z;
		return *this;

	}

	inline Ivector3 Ivector3::operator*(int scale) const {

		return Ivector3(x * scale, y * scale, z * scale);



	}

	inline Ivector3& Ivector3::operator*=(int scale) {


		x *= scale;
		y *= scale;
		z *= scale;
		return *this;

	}

	

	inline Ivector3 Ivector3::operator*(Ivector3& scale) const {

		return Ivector3(x * scale.x, y * scale.y, z * scale.z);
	}


	inline Ivector3& Ivector3::operator*=(Ivector3& scale) {
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
		return *this;
	}





	inline glm::vec3 Ivector3::glm()
	{
		return glm::vec3(x, y, z);
	}

	inline Ivector3::Ivector3(int X, int Y, int Z) {

		x = X;
		y = Y;
		z = Z;
	}
	inline Ivector3::Ivector3(glm::vec3 glm)
	{
		x = glm.x;
		y = glm.y;
		z = glm.z;

	}
	inline bool Ivector3::operator==(const Ivector3& p1)
	{
		return (p1.x == x && p1.y == y && p1.z == z);

	}
	inline bool Ivector3::operator!=(const Ivector3& p1)
	{
		return(p1.x != x || p1.y != y || z != p1.z);

	}
}



namespace v3
{
	struct Vector3
	{
		glm::vec3 glm();
		Vector3(float X, float Y,float Z);
		Vector3(glm::vec3 glm);
		Vector3(iv3::Ivector3 coord);
		Vector3();
		void operator=(const Vector3& p1);
		bool operator==(const Vector3& p1);
		bool operator!=(const Vector3& p1);
		Vector3& operator+=(const Vector3& p1);
		Vector3 operator+(const Vector3& p1) const;

		Vector3& operator-=(const Vector3& p1);
		Vector3 operator-(const Vector3& p1) const;

		Vector3 operator*(float scale) const;
		Vector3& operator*=(float scale);


		Vector3 operator/(float scale) const;

		Vector3& operator/=(float scale);

		Vector3 operator*(Vector3& scale) const;
		Vector3& operator*=(Vector3& scale);

		Vector3 operator/(Vector3& scale) const;
		Vector3& operator/=(Vector3& scale);

		float x;
		float y;
		float z;


	};

	const  Vector3 zerov = Vector3(0, 0,0);
	const  Vector3 unitv = Vector3(1, 1,1);
	
	inline Vector3::Vector3() {



		x = 0;
		y = 0;
		z = 0;
	}

	inline float distance(const Vector3& p, const Vector3& p1) {

		return(sqrt((p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y)));
	}

	
	inline float magnitude(const Vector3& p) {

		return(sqrt(p.x * p.x + p.y * p.y));
	}

	inline Vector3 normal(const Vector3& p) {

		return(p / magnitude(p));
	}

	inline float dotproduct(const Vector3& p, const Vector3& p1) {

		return (p.x * p1.x + p.y + p1.y);
	}


	inline Vector3 lerp(const Vector3& p, const Vector3& p1, float t) {
		return p * (1 - t) + p1 * t;


	}

	
	inline void Vector3::operator=(const Vector3& p1) {

		x = p1.x;
		y = p1.y;
		z = p1.z;

	}




	inline Vector3 Vector3::operator+(const Vector3& p1) const {


		return Vector3(x + p1.x, y + p1.y,z+p1.z);


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

	inline Vector3 Vector3::operator/(float scale) const {

		return Vector3(x / scale, y / scale,z/scale);
	}

	inline Vector3& Vector3::operator/=(float scale) {


		x /= scale;
		y /= scale;
		z /= scale;
		return *this;

	}


	inline Vector3 Vector3::operator*(Vector3& scale) const {

		return Vector3(x * scale.x, y * scale.y,z*scale.z);
	}


	inline Vector3& Vector3::operator*=(Vector3& scale) {
		x *= scale.x;
		y *= scale.y;
		z *= scale.z;
		return *this;
	}

	inline Vector3 Vector3::operator/(Vector3& scale) const {

		return Vector3(x / scale.x, y / scale.y,z/scale.z);
	}


	inline Vector3& Vector3::operator/=(Vector3& scale) {


		x /= scale.x;
		y /= scale.y;
		z /= scale.z;
		return *this;

	}
	


	inline glm::vec3 Vector3::glm()
	{
		return glm::vec3(x,y,z);
	}

	inline Vector3::Vector3(float X, float Y,float Z) {

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
	inline Vector3::Vector3(iv3::Ivector3 coord)
	{
		x = coord.x;
		y = coord.y;
		z = coord.z;
	}
	inline bool Vector3::operator==(const Vector3& p1)
	{
		return (p1.x == x && p1.y == y&&p1.z==z);

	}
	inline bool Vector3::operator!=(const Vector3& p1)
	{
		return(p1.x != x || p1.y != y||z!=p1.z);

	}
}
#endif