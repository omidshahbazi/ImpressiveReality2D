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

#include <Vector2D.h>
#include "IFont.h"
#include "ILabel.h"
#include "ITextBox.h"
#include "IButton.h"
#include "IProgressBar.h"

BEGIN_NAMESPACE

class IGUIScene
{
public:
	typedef Vector<IGUIObject*> GUIObjectList;

public:
	virtual ~IGUIScene(void) {}
	
	virtual ILabel *CreateLabel(const String &Name, const Vector2D &Size, IFont *Font, const String &Text = "") = 0;
	
	virtual ITextBox *CreateTextBox(const String &Name, const Vector2D &Size, IFont *Font) = 0;

	virtual IButton *CreateButton(const String &Name, const Vector2D &Size, IFont *Font) = 0;

	virtual IProgressBar *CreateProgressBar(const String &Name, const Vector2D &Size, IFont *Font) = 0;

	virtual void DestroyObject(IGUIObject *Object) = 0;

	virtual void Render(void) = 0;

	virtual void Update(void) = 0;

};

END_NAMESPACE