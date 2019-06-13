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
#include "Colour.h"

BEGIN_NAMESPACE

class IImage
{
public:
	virtual ~IImage(void) {}

	virtual void Clear(const Colour &Colour = Colour_BLACK) = 0;

	virtual void DrawRectangle(const Vector2D &Origin, const Vector2D &Size, const Colour &Colour) = 0;

	virtual void SetPixel(const unsigned int &X, const unsigned int &Y, const Colour &Colour) = 0;
	virtual void SetPixel(const Vector2D &Position, const Colour &Colour) = 0;

	virtual const Colour GetPixel(const unsigned int &X, const unsigned int &Y) = 0;
	virtual const Colour GetPixel(const Vector2D &Position) = 0;

	virtual const String &GetName(void) const = 0;

	virtual const Vector2D &GetSize(void) const = 0;
};

END_NAMESPACE