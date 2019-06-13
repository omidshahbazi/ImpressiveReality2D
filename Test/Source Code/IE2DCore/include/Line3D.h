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

#include "Vector3D.h"

BEGIN_NAMESPACE


struct DLL_DECLARATION Line3D
{
public:
	Line3D(void);
	Line3D(const Vector3D &Start, const Vector3D &End);

	~Line3D(void);

	const Vector3D &GetStart(void) const
	{
		return m_Start;
	}

	const Vector3D &GetEnd(void) const
	{
		return m_End;
	}
 
	float GetLength(void) const
	{
		return m_Start.DistanceTo(m_End);
	}

	Vector3D GetMiddle(void) const
	{
		return (m_Start + m_End) / 2;
	}

	Vector3D GetDirection(void) const
	{
		return m_End - m_Start;
	}
public:
	Vector3D m_Start;
	Vector3D m_End;

};


END_NAMESPACE