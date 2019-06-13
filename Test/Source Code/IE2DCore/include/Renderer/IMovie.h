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

#include "ITexture.h"
#include "Vector2D.h"

BEGIN_NAMESPACE

//<Description>
//This is sample description
class IMovie
{
public:
	virtual ~IMovie(void) {}

	virtual void Update(void) = 0;

	virtual void Play(void) = 0;
	virtual void Pause(void) = 0;
	virtual void Stop(void) = 0;

	virtual void SetFPS(const unsigned int &FPS) = 0;

	virtual ITexture *GetTexture(void) = 0;

	virtual const Vector2D &GetSize(void) = 0;

	virtual const float &GetDuration(void) = 0;
	
	virtual void SetLoop(const bool &Value) = 0;
	virtual const bool &GetLoop(void) const = 0;
};

END_NAMESPACE