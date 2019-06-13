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

#include "Vector2D.h"

BEGIN_NAMESPACE

class IShape
{
public:
	enum ShapeType
	{
		ST_CIRCLE = 0,
		ST_BOX,
		ST_POLYGON,
		ST_EDGE
	};

public:
	virtual ~IShape(void) {}

	const ShapeType &GetType(void) const
	{
		return m_Type;
	}

protected:
	ShapeType m_Type;
};

END_NAMESPACE