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

#include "Common.h"

BEGIN_NAMESPACE

class IFixture;

struct Vector2D;

class IContactImpulse
{
public:
	virtual ~IContactImpulse(void) {}

	virtual const unsigned int &GetImpulseCount(void) = 0;

	virtual const float *GetNormalImpulse(void) = 0;

	virtual const float *GetTangentImpulse(void) = 0;
};

END_NAMESPACE