#pragma once

// why am I not using the std one?

struct vector3
{
	float x;
	float y;
	float z;

	void normalize();
	float magnitude();
	float dotProduct(const vector3& vec);

	vector3(float f_x, float f_y, float f_z);
	vector3() {};
	vector3 operator+(const vector3& vec);
	vector3 operator-(const vector3& vec);
	void operator+=(const vector3& vec);
	void operator-=(const vector3& vec);
};