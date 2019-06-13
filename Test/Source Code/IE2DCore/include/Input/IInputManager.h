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

#include "KeyCodes.h"

BEGIN_NAMESPACE

class IRenderWindow;
struct Vector2D;

//<Description>
//This is sample description
class IInputManager
{
public:
	enum MouseButtons
	{
		M_LEFT = 0,
		M_RIGHT,
		M_MIDDLE,
		M_BUTTON3,
		M_BUTTON4,
		M_BUTTON5,
		M_BUTTON6,
		M_BUTTON7,
		M_COUNT
	};		

public:
	virtual ~IInputManager(void) {}

	virtual void Initialize(IRenderWindow *RenderWindow) = 0;

	virtual void Update(void) = 0;
	virtual void PostUpdate(void) = 0;

	//virtual bool IsModifierDown(const Modifier &Modifier) = 0;

	virtual const bool &IsKeyDown(const KeyCodes &Code) const = 0;
	virtual const bool &IsKeyPressed(const KeyCodes &Code) const = 0;
	virtual const bool &IsControlDown(void) const = 0;
	virtual const bool &IsShiftDown(void) const = 0;
	virtual const bool &IsAltDown(void) const = 0;

	virtual void SetCursorVisible(const bool &Value) = 0;
	virtual const bool &IsCursorVisible(void) const = 0;

	virtual const bool &IsMouseHold(const MouseButtons &Button) = 0;
	virtual const bool &IsMouseDown(const MouseButtons &Button) = 0;
	virtual const bool &IsMouseClicked(const MouseButtons &Button) = 0;

	virtual void SetMousePosition(const int &X, const int &Y) = 0;
	virtual void SetMousePosition(const Vector2D &Position) = 0;
	virtual const Vector2D &GetMousePosition(void) = 0;
	virtual void SetNormalizedMousePosition(const float &X, const float &Y) = 0;
	virtual void SetNormalizedMousePosition(const Vector2D &Position) = 0;
	virtual const Vector2D GetNormalizedMousePosition(void) const = 0;

	virtual void SetChar(const char &Character) = 0;
	virtual const char &GetChar(void) = 0;
};

END_NAMESPACE