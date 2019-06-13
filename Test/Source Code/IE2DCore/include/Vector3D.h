///////////////////////////////////////////////////////////////////////////////////
///
///  Impressive Engine 2D
///
/// Copyright (c) 2012-2013 Impressive Reality team
///
/// The license is
///
/// Permission is denied, to any person or company
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// Project leader : O.Shahbazi <sh_omid_m@yahoo.com>
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common.h"

BEGIN_NAMESPACE

struct Vector2D;

//<Description>
//This is sample description
struct DLL_DECLARATION Vector3D
{
public:
	Vector3D(void);
	Vector3D(const float &Scalar);
	Vector3D(const float &X, const float &Y, const float &Z);
	Vector3D(const Vector2D &Vector);
	Vector3D(const Vector2D &Vector, const float &Z);
	Vector3D(const Vector3D &Vector);
	//~Vector3D(void);

	bool operator == (const Vector3D &Vector) const;
	bool operator != (const Vector3D &Vector) const;
	bool operator <= (const Vector3D &Vector) const;
	bool operator >= (const Vector3D &Vector) const;
	bool operator < (const Vector3D &Vector) const;
	bool operator > (const Vector3D &Vector) const;

	Vector3D &operator = (const Vector3D &Vector);

	Vector3D operator + (const Vector3D &Vector) const;
	Vector3D &operator += (const Vector3D &Vector);

	Vector3D operator - (const Vector3D &Vector) const;
	Vector3D &operator -= (const Vector3D &Vector);
	
	Vector3D operator * (const float &Value) const;
	Vector3D operator * (const Vector3D &Vector) const;
	Vector3D &operator *= (const float &Value);
	Vector3D &operator *= (const Vector3D &Vector);
	
	Vector3D operator / (const float &Value) const;
	Vector3D operator / (const Vector3D &Vector) const;
	Vector3D &operator /= (const float &Value);
	Vector3D &operator /= (const Vector3D &Vector);

	const float Length(void) const;
	const float LengthSqr(void) const;

	void Normalize(void);
	Vector3D NormalisedCopy(void) const;

	float DistanceTo(const Vector3D &Vector) const;
	//float DistanceSqrTo(const Vector3D &Vector) const;

	float DotProduct(const Vector3D &Vector) const;

	Vector3D CrossProduct(const Vector3D &Vector) const;

	float AngleBetween(const Vector3D &Vector) const;

public:
	float X, Y, Z;
};


const Vector3D Vector3D_ZERO(0);
const Vector3D Vector3D_UNIT_SCALE(1);
const Vector3D Vector3D_UNIT_X(1, 0, 0);
const Vector3D Vector3D_UNIT_Y(0, 1, 0);
const Vector3D Vector3D_UNIT_Z(0, 0, 1);

END_NAMESPACE