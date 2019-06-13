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
class IFile
{
public:
	virtual ~IFile(void) {}

	virtual const String &ReadText(void) = 0;
	
	virtual void WriteText(const String &Text) = 0;

	virtual bool IsOpen(void) const = 0;

	virtual void Close(void) = 0;
	
	virtual IFile &operator <<(const String &Text) = 0;

	virtual const String &GetFilePath(void) const = 0;

	virtual const long &GetSize(void) const = 0;

	virtual const bool &IsWriteMode(void) const = 0;
};

END_NAMESPACE