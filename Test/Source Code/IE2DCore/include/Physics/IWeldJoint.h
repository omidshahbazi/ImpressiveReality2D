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

class IWeldJoint : public IJoint
{
public:
	virtual ~IWeldJoint(void) {}

	virtual const Vector2D GetAnchorA(void) const = 0;

	virtual const Vector2D &GetLocalAnchorA(void) const = 0;

	virtual const Vector2D GetAnchorB(void) const = 0;

	virtual const Vector2D &GetLocalAnchorB(void) const = 0;

	virtual const float &GetReferenceAngle(void) const = 0;

	virtual void SetFrequencyHz(const float &Value) = 0;
	virtual const float &GetFrequencyHz(void) const = 0;

	virtual void SetDampintRatio(const float &Value) = 0;
	virtual const float &GetDampingRatio(void) const = 0;

};

END_NAMESPACE