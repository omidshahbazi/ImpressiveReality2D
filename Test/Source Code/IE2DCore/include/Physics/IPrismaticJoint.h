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

class IPrismaticJoint : public IJoint
{
public:
	virtual ~IPrismaticJoint(void) {}

	virtual const Vector2D &GetLocalAxisA(void) const = 0;

	virtual const Vector2D GetAnchorA(void) const = 0;

	virtual const Vector2D &GetLocalAnchorA(void) const = 0;

	virtual const Vector2D GetAnchorB(void) const = 0;

	virtual const Vector2D &GetLocalAnchorB(void) const = 0;

	virtual const float &GetReferenceAngle(void) const = 0;

	virtual void EnableLimit(const bool &Flag) = 0;
	virtual const bool &IsLimitEnabled(void) const = 0;

	virtual void SetLimits(const float &LowerAngle, const float &UpperAngle) = 0;
	virtual void SetLowerLimit(const float &Value) = 0;
	virtual void SetUpperLimit(const float &Value) = 0;

	virtual const float &GetLowerLimit(void) const = 0;
	virtual const float &GetUpperLimit(void) const = 0;

	virtual void EnableMotor(const bool &Flag) = 0;
	virtual const bool &IsMotorEnabled(void) const = 0;

	virtual void SetMotorSpeed(const float &Value) = 0;
	virtual const float &GetMotorSpeed(void) const = 0;

	virtual void SetMaxMotorForce(const float &Value) = 0;
	virtual const float &GetMaxMotorForce(void) const = 0;
	virtual const float GetMotorForce(const float &InverseDeltaTime) const = 0;

};

END_NAMESPACE