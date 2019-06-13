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

class IEdgeShape : public IShape
{
public:
	virtual ~IEdgeShape(void) {}

	virtual void SetBeginPoint(const Vector2D &Point) = 0;
	virtual const Vector2D &GetBeginPoint(void) const = 0;

	virtual void SetEndPoint(const Vector2D &Point) = 0;
	virtual const Vector2D &GetEndPoint(void) const = 0;
};

END_NAMESPACE