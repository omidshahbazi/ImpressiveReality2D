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

#include "Common.h"
#include "IFont.h"
#include "ILabel.h"

BEGIN_NAMESPACE

class IGUIScene;

class IGUISystem
{
public:
	virtual ~IGUISystem(void) {}

	virtual void Initialize(void) = 0;
	
	virtual IFont *LoadFont(const String &Name, const unsigned int &Size = 50) = 0;
	virtual void DestroyFont(IFont *Font) = 0;

	virtual IGUIScene *CreateGUIScene(void) = 0;
	virtual void DestroyGUIScene(IGUIScene *Scene) = 0;
};

END_NAMESPACE