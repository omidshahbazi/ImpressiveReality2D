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

#include "IJoint.h"

BEGIN_NAMESPACE

class IDistanceJoint : public IJoint
{
public:
	virtual ~IDistanceJoint(void) {}

	virtual const Vector2D GetAnchorA(void) const = 0;

	virtual const Vector2D &GetLocalAnchorA(void) const = 0;

	virtual const Vector2D GetAnchorB(void) const = 0;

	virtual const Vector2D &GetLocalAnchorB(void) const = 0;

	virtual void SetLength(const float &Value) = 0;
	virtual const float &GetLength(void) const = 0;

	virtual void SetDampingRatio(const float &Value) = 0;
	virtual const float &GetDampingRatio(void) const = 0;

	virtual void SetFrequency(const float &Value) = 0;
	virtual const float &GetFrequency(void) const = 0;

};

END_NAMESPACE