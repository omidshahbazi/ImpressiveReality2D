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

class IRevoluteJoint : public IJoint
{
public:
	virtual ~IRevoluteJoint(void) {}

	virtual const Vector2D GetAnchorA(void) const = 0;

	virtual const Vector2D &GetLocalAnchorA(void) const = 0;

	virtual const Vector2D GetAnchorB(void) const = 0;

	virtual const Vector2D &GetLocalAnchorB(void) const = 0;

	virtual const float GetJointAngle(void) const = 0;
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

	virtual void SetMaxMotorTorque(const float &Value) = 0;
	virtual const float GetMotorTorque(const float &InverseDeltaTime) const = 0;
	virtual const float &GetMaxMotorTorque(void) const = 0;
};

END_NAMESPACE