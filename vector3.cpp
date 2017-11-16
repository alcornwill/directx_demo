#include "vector3.h"

#include <cmath>

void vector3::normalize()
{
	// find magnitude
	float mag = this->magnitude();

	x = x / mag;
	y = y / mag;
	z = z / mag;
}

float vector3::magnitude()
{
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

float vector3::dotProduct(const vector3& vec)
{
	return x * vec.x + y * vec.y + z * vec.z;
}

vector3::vector3(float f_x, float f_y, float f_z)
{
	x = f_x;
	y = f_y;
	z = f_z;
}

vector3 vector3::operator+(const vector3& vec)
{
	return vector3(x + vec.x, y + vec.y, z + vec.z);
}

vector3 vector3::operator-(const vector3& vec)
{
	return vector3(x - vec.x, y - vec.y, z - vec.z);
}

void vector3::operator+=(const vector3& vec)
{
	x = x + vec.x;
	y = y + vec.y;
	z = z + vec.z;
}

void vector3::operator-=(const vector3& vec)
{
	x = x - vec.x;
	y = y - vec.y;
	z = z - vec.z;
}