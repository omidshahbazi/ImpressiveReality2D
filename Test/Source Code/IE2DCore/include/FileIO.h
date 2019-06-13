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
#include "Singleton.h"
#include "IFile.h"
#include "ITreeParser.h"

BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class DLL_DECLARATION FileIO
{
	DECLARE_SINGLETON(FileIO)

public:
	enum FileType
	{
		FT_SCENE = 0,
		FT_ANIMATION,
		FT_MATERIAL,
		FT_SCRIPT
	};

public:
	FileIO(void);
	~FileIO(void);

	IFile *OpenFile(const String &FilePath);
	IFile *OpenFile(const String &FilePath, const bool &Append);

	const String ReadText(const String &CryptSalt, const String &FilePath);
	const String ReadText(const FileType &Type, const String &FilePath);
	void WriteText(const String &CryptSalt, const String &FilePath, const String &Text);
	void WriteText(const FileType &Type, const String &FilePath, const String &Text);
	void WriteText(const String &CryptSalt, const String &FilePath, const String &Text, const bool &Append);
	void WriteText(const FileType &Type, const String &FilePath, const String &Text, const bool &Append);

	ITreeParser *CreateTreeParser(void);
	ITreeParser *CreateTreeParser(const String &CryptSalt, const String &FilePath = "", const String &StartPath = "");
	ITreeParser *CreateTreeParser(const FileType &Type, const String &FilePath = "", const String &StartPath = "");
};

END_NAMESPACE