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
	
//<Description>
//This is sample description
class ISound
{
public:
	virtual ~ISound(void) {}

	virtual void Update(void) = 0;

	virtual void Play(void) = 0;
	virtual void Play(const unsigned int &DelayTime) = 0;

	virtual void Pause(void) = 0;
	virtual const bool &IsPaused(void) = 0;
	virtual void Stop(void) = 0;

	virtual void SetLoop(const bool &Flag) = 0;
	virtual const bool &IsLooped(void) const = 0;

	virtual void SetVolume(const unsigned int &Value) = 0;
	virtual const unsigned int &GetVolume(void) = 0;

	virtual const float &GetSampleRate(void) const = 0;

	virtual void SetDelayTime(const unsigned int &Value) = 0;
	virtual const unsigned int &GetDelayTime(void) const = 0;

	virtual const unsigned int &GetLength(void) const = 0;

	virtual const unsigned int &GetCurrentPosition(void) = 0;

	virtual void SetClip(const unsigned int &StartTime, const unsigned int &EndTime) = 0;
};

END_NAMESPACE