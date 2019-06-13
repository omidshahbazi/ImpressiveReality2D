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

#include "IAttributes.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class ISerializable
{
public:
	virtual ~ISerializable(void) {}

	virtual bool Serialize(IAttributes *Attributes) = 0;
	virtual bool Deserialize(IAttributes *Attributes) = 0;
};

END_NAMESPACE