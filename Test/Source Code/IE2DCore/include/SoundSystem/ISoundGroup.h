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

class ISound;

//<Description>
//This is sample description
class ISoundGroup
{
public:
	typedef Vector<ISound*> SoundsList;

public:
	virtual ~ISoundGroup(void) {}

	virtual void Update(void) = 0;

	virtual void Play(void) = 0;
	virtual void Pause(void) = 0;
	virtual void Stop(void) = 0;

	virtual void SetVolume(const unsigned int &Value) = 0;

	virtual const String &GetName(void) const = 0;

	virtual ISound *CreateSound(const String &Filename) = 0;
	virtual ISound *CreateStream(const String &Filename) = 0;

	virtual void DestroySound(ISound *Sound) = 0;

};

END_NAMESPACE