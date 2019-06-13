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

struct Line3D;

struct DLL_DECLARATION AABB
{
public:
	AABB(void);
	//~AABB(void);
	
	void Reset(void);

	void InsertPoint(const Vector3D &Point);

	void Translate(const Vector3D &Offset);

	const float GetInnerRadius(void);
	const float GetOuterRadius(void);
	
	const Vector3D &GetMinimum(void) const
	{
		return m_Minimum;
	}

	const Vector3D &GetMaximum(void) const
	{
		return m_Maximum;
	}
	
	const Vector3D GetExtents(void) const
	{
		return m_Maximum - m_Minimum;
	}

	const Vector3D GetCenter(void) const
	{
		return (m_Maximum + m_Minimum) / 2;
	}

	bool IntersectsWithLine(const Line3D &Line) const;

public:
	static const AABB ZERO;

private:
	bool m_IsZero;
	Vector3D m_Minimum;
	Vector3D m_Maximum;
};

END_NAMESPACE

