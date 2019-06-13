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

struct DLL_DECLARATION OBB
{
public:
	OBB(void);
	//~OBB(void);
	
	void Reset(void);

	void SetCorner(const unsigned int &Index, const Vector3D &Corner);

	const float GetInnerRadius(void);
	const float GetOuterRadius(void);
	
	const Vector3D *GetCorners(void) const
	{
		return m_Corners;
	}
	
	bool IntersectsWithLine(const Line3D &Line) const;

public:
	static const OBB ZERO;

private:
	Vector3D m_Corners[4];
};

END_NAMESPACE

