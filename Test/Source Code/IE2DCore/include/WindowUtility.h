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
#include "Vector2D.h"
#include <Windows.h>

BEGIN_NAMESPACE

static class DLL_DECLARATION WindowUtility
{
	BEGIN_DECLARE_STATIC_LISTENER_HOLDER
		virtual void OnSizeChanged(const Vector2D &Size) = 0;
		virtual void OnLostFocus(void) = 0;
		virtual void OnGetFocus(void) = 0;
		virtual void OnClose(void) = 0;
		
		virtual const HWND &GetHandle(void) = 0;
	END_DECLARE_STATIC_LISTENER_HOLDER

public:
	static void PumpMessages(void);

	static LRESULT CALLBACK WndProc(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam);

	static LRESULT HandleMessages(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam);

private:
	typedef Vector<IListener*> IListenersList;
} WindowUtility_Dummy;

END_NAMESPACE