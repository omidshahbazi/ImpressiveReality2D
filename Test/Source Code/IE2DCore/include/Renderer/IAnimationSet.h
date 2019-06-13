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
#include "IAnimation.h"
#include "ITexture.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class IAnimationSet : public ISerializable
{
public:
	typedef Vector<IAnimation*> AnimationsList;

public:
	virtual ~IAnimationSet(void) {}

	virtual const String &GetName(void) const = 0;

	virtual void SetTexture(ITexture *Texture) = 0;
	virtual ITexture *GetTexture(void) = 0;
	
#ifndef LAUNCH_MODE
	virtual IAnimation *CreateAnimation(void) = 0;
#endif
	virtual IAnimation *GetAnimation(const String &Name) = 0;

	virtual AnimationsList &GetAnimations(void) = 0;
};

END_NAMESPACE