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

class IBoxShape : public IShape
{
public:
	virtual ~IBoxShape(void) {}

	virtual void SetCenter(const Vector2D &Center) = 0;
	virtual const Vector2D &GetCenter(void) const = 0;

	virtual void SetHalfSize(const Vector2D &HalfSize) = 0;
	virtual const Vector2D &GetHalfSize(void) const = 0;

};

END_NAMESPACE