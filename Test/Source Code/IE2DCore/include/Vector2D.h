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
	
//<Description>
//This is sample description
struct DLL_DECLARATION Vector2D
{
public:
	Vector2D(void);
	Vector2D(const float &Scalar);
	Vector2D(const float &X, const float &Y);
	Vector2D(const Vector2D &Vector);
	//~Vector2D(void);

	bool operator == (const Vector2D &Vector) const;
	bool operator != (const Vector2D &Vector) const;
	bool operator <= (const Vector2D &Vector) const;
	bool operator >= (const Vector2D &Vector) const;
	bool operator < (const Vector2D &Vector) const;
	bool operator > (const Vector2D &Vector) const;

	Vector2D &operator = (const Vector2D &Vector);

	Vector2D operator + (const Vector2D &Vector) const;
	Vector2D &operator += (const Vector2D &Vector);

	Vector2D operator - (const Vector2D &Vector) const;
	Vector2D &operator -= (const Vector2D &Vector);
	
	Vector2D operator * (const float &Value) const;
	Vector2D operator * (const Vector2D &Vector) const;
	Vector2D &operator *= (const float &Value);
	Vector2D &operator *= (const Vector2D &Vector);
	
	Vector2D operator / (const float &Value) const;
	Vector2D operator / (const Vector2D &Vector) const;
	Vector2D &operator /= (const float &Value);
	Vector2D &operator /= (const Vector2D &Vector);

	//const float Length(void) const;
	//const float LengthSqr(void) const;

	//void Normalize(void);
	//Vector2D NormalisedCopy(void) const;

	//float DistanceTo(const Vector2D &Vector) const;
	//float DistanceSqrTo(const Vector2D &Vector) const;

	//float DotProduct(const Vector2D &Vector) const;

public:
	float X, Y;
};

const Vector2D Vector2D_ZERO(0);
const Vector2D Vector2D_UNIT_SCALE(1);
const Vector2D Vector2D_UNIT_X(1, 0);
const Vector2D Vector2D_UNIT_Y(0, 1);

END_NAMESPACE