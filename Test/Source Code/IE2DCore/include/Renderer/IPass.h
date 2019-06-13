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

#include "ISerializable.h"

BEGIN_NAMESPACE

class ITexture;
	
//<Description>
//This is sample description
class IPass : public ISerializable
{
public:
	enum PassType
	{
		PT_SOLID = 0,
		PT_ALPHA_TRANSPARENCY,
		PT_COLOURED_TRANSPARENCY
	};

public:
	virtual ~IPass(void) {}

	virtual const String &GetName(void) const = 0;
	
	virtual void SetType(const PassType &Type) = 0;
	virtual const PassType &GetType(void) const = 0;
	
	virtual void SetTexture(ITexture *Texture) = 0;
	virtual ITexture *GetTexture(void) const = 0;
};

END_NAMESPACE