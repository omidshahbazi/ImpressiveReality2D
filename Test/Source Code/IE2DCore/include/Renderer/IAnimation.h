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
#include "ISerializable.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class IAnimation : public ISerializable
{
public:
	virtual ~IAnimation(void) {}
	
	virtual void SetName(const String &Name) = 0;
	virtual const String &GetName(void) const = 0;
	
	virtual void SetStartPosition(const Vector2D &Value) = 0;
	virtual const Vector2D &GetStartPosition(void) const = 0;

	virtual void SetFrameSize(const Vector2D &Value) = 0;
	virtual const Vector2D &GetFrameSize(void) const = 0;

	virtual void SetFramesCount(const unsigned int &Value) = 0;
	virtual const unsigned int &GetFramesCount(void) const = 0;;

	virtual void SetDuration(const unsigned int &Value) = 0;
	virtual const unsigned int &GetDuration(void) const = 0;

	virtual void SetMirror(const bool &Value) = 0;
	virtual const bool &GetMirror(void) const = 0;

	virtual void SetReverse(const bool &Value) = 0;
	virtual const bool &GetReverse(void) const = 0;

	virtual void SetLoop(const bool &Value) = 0;
	virtual const bool &GetLoop(void) const = 0;
	
	virtual void SetEnabled(const bool &Value) = 0;
	virtual const bool &GetEnabled(void) const = 0;

	//virtual void AddTexture(ITexture *Texture)
	//virtual ITexture *GetTexture(const unsigned int &Index = 0);

	virtual void SetMultiTexture(const bool &Value) = 0;
	virtual const bool &GetMultiTexture(void) const = 0;

	virtual void SetDirectory(const String &Diretory) = 0;
	virtual const String &GetDirectory(void) const = 0;

	virtual void SetPrefix(const String &Prefix) = 0;
	virtual const String &GetPrefix(void) const = 0;

	virtual void SetPostfix(const String &Postfix) = 0;
	virtual const String &GetPostfix(void) const = 0;

	virtual void SetCounterStartNumber(const unsigned int &Value) = 0;
	virtual const unsigned int &GetCounterStartNumber(void) const = 0;
};

END_NAMESPACE