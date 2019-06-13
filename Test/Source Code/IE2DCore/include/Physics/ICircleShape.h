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

#include "IShape.h"

BEGIN_NAMESPACE

class ICircleShape : public IShape
{
public:
	virtual ~ICircleShape(void) {}

	virtual void SetCenter(const Vector2D &Center) = 0;
	virtual const Vector2D &GetCenter(void) const = 0;

	virtual void SetRadius(const float &Value) = 0;
	virtual const float &GetRadius(void) const = 0;

};

END_NAMESPACE