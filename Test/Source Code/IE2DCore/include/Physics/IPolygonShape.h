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

class IPolygonShape : public IShape
{
public:
	virtual ~IPolygonShape(void) {}

	virtual const Vector2D &GetCenter(void) const = 0;

	virtual const List<Vector2D> &GetVertices(void) const = 0;

	virtual const unsigned int &GetVertexCount(void) const = 0;
};

END_NAMESPACE