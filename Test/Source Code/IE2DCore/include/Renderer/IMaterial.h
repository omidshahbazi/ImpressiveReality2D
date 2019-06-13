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

#include "IPass.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class IMaterial : public ISerializable
{
public:
	typedef Vector<IPass*> PassesList;

public:
	virtual ~IMaterial(void) {}

	virtual const String &GetName(void) const = 0;

	virtual IPass *CreatePass(void) = 0;

	virtual PassesList &GetPasses(void) = 0;
};

END_NAMESPACE