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

#include <Windows.h>
#include "Common.h"
#include "Vector2D.h"

BEGIN_NAMESPACE

class IRenderWindow
{
public:
	virtual ~IRenderWindow(void) {}

	virtual void SetText(const String &Text) = 0;

	virtual bool Show(void) = 0;

	virtual void Close(void) = 0;

	virtual bool IsClosed(void) = 0;

	virtual const HWND &GetHandle(void) const = 0;
	
	//virtual const HDC &GetDrawingHandle(void) const = 0;

	virtual bool IsAnExternalWindow(void) = 0;
	
	virtual void SetWindowPosition(const Vector2D &Size) = 0;
	virtual const Vector2D &GetWindowPosition(void) const = 0;

	virtual void SetWindowSize(const Vector2D &Size) = 0;
	virtual const Vector2D &GetWindowSize(void) const = 0;
	virtual const Vector2D GetActualWindowSize(void) const = 0;

	virtual void SetFullScreen(void) = 0;

	virtual const bool &HasFocus(void) const = 0;

#ifndef LAUNCH_MODE
	virtual void OnSizeChanged(const Vector2D &Size) = 0;
#endif
	
	virtual const String &GetName(void) const = 0;
};

END_NAMESPACE