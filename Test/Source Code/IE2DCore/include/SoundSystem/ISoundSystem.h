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

BEGIN_NAMESPACE

class ISoundGroup;

//<Description>
//This is sample description
class ISoundSystem
{
public:
	typedef Vector<ISoundGroup*> SoundGroupsList;

public:
	virtual ~ISoundSystem(void) {}

	virtual void Initialize(unsigned int maxChannels = 100) = 0;

	virtual void Update(void) = 0;

	virtual ISoundGroup *CreateGroup(const String &Name) = 0;
	virtual ISoundGroup *GetGroup(const String &Name) = 0;
	virtual void DestroyGroup(const String &Name) = 0;
	virtual void DestroyGroup(ISoundGroup *Group) = 0;
};

END_NAMESPACE