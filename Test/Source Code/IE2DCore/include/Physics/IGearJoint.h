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

class IRevoluteJoint;
class IPrismaticJoint;

class IGearJoint : public IJoint
{
public:
	virtual ~IGearJoint(void) {}

	virtual const Vector2D GetAnchorA(void) const = 0;

	virtual const Vector2D GetAnchorB(void) const = 0;

	virtual void SetRatio(const float &Value) = 0;
	virtual const float &GetRatio(void) const = 0;

	virtual IRevoluteJoint *GetRevoluteJoint(void) = 0;

	virtual IPrismaticJoint *GetPrismaticJoint(void) = 0;
};

END_NAMESPACE