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

#include "Vector2D.h"
#include "IFont.h"
#include "Colour.h"
#include "Renderer\IImage.h"

BEGIN_NAMESPACE

class IGUIObject
{
public:
	IGUIObject(void) :
		m_Visible(true),
		m_Font(NULL),
		m_Image(NULL),
		m_Bordersize(0)
	{
	}

	virtual ~IGUIObject(void) {}

	virtual void Update(void) = 0;
	virtual void Render(void) = 0;
	
protected:
	virtual void SetSize(const Vector2D &Size)
	{
		m_Size = Size;
	}
	void SetName(const String &Name)
	{
		m_Name = Name;
	}

public:
	virtual const String &GetName(void) const
	{
		return m_Name;
	}

	virtual const Vector2D &GetSize(void)
	{
		return m_Size;
	}
		
	virtual void SetLocation(const Vector2D &Location)
	{
		m_Location = Location;
	}
	virtual const Vector2D &GetLocation(void)
	{
		return m_Location;
	}

	virtual void SetVisible(const bool &Visible)
	{
		m_Visible = Visible;
	}
	virtual  bool &GetVisible(void)
	{
		return m_Visible;
	}

	virtual void SetFont(IFont *Font)
	{
		m_Font = Font;
	}
	virtual IFont *GetFont(void)
	{
		return m_Font;
	}

	virtual void SetForeColour(const Colour &Colour)
	{
		m_ForeColour = Colour;
	}
	virtual const Colour &GetForeColour(void)
	{
		return m_ForeColour;
	}

	virtual void SetBackColour(const Colour &Colour)
	{
		m_BackColour = Colour;
	}
	virtual const Colour &GetBackColour(void)
	{
		return m_BackColour;
	}

	virtual void SetBackImage(IImage *Image)
	{
		m_Image = Image;
	}
	virtual IImage *GetBackImage(void)
	{
		return m_Image;
	}

	virtual void SetBorderSize(const int &Size)
	{
		m_Bordersize = Size;
	}
	virtual const int &GetBorderSize(void)
	{
		return m_Bordersize;
	}

	virtual void SetBorderColour(const Colour &Colour)
	{
		m_BorderColour = Colour;
	}
	virtual const Colour &GetBorderColour(void)
	{
		return m_BorderColour;
	}

private:
	String m_Name;
	Vector2D m_Size;
	Vector2D m_Location;
	bool m_Visible;
	IFont *m_Font;
	Colour m_ForeColour;
	Colour m_BackColour;
	IImage *m_Image;

	unsigned int m_Bordersize;
	Colour m_BorderColour;
};

END_NAMESPACE