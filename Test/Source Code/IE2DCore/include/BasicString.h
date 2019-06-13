
#pragma once

#include "SecondaryCommon.h"
#include <iostream>
#include "List.h"


BEGIN_NAMESPACE
	
//<Description>
//This is sample description
class DLL_DECLARATION BasicString
{
public:
	BasicString(void);
	BasicString(const char *Value, const int &Size = -1);
	BasicString(const BasicString &Value);
	~BasicString(void);

	static unsigned int GetCounstructorCount(void);
	static unsigned int GetDestructorCount(void);
	static unsigned int GetTotalSizeOf(void);

	void Reverse(void);

	const int Find(const char &Value, const unsigned int &Start = 0) const;
	const int Find(const char *Value, const unsigned int &Start = 0) const;
	const int Find(const BasicString &Value, const unsigned int &Start = 0) const;

	const int FindFirst(const char &Value, const unsigned int &Start = 0) const;
	//const int FindFirst(const char *Value, const unsigned int &Start = 0) const;
	//const int FindFirst(const BasicString &Value, const unsigned int &Start = 0) const;

	const int FindFirstNot(const char *Value, const unsigned int &Start = 0) const;
	const int FindFirstNot(const BasicString &Value, const unsigned int &Start = 0) const;

	const int FindLast(const char &Value, const unsigned int &Start = 0) const;
	//const int FindLast(const char *Value, const unsigned int &Start = 0) const;
	//const int FindLast(const BasicString &Value, const unsigned int &Start = 0) const;

	const int FindLastNot(const char *Value, const unsigned int &Start = 0) const;
	const int FindLastNot(const BasicString &Value, const unsigned int &Start = 0) const;

	BasicString SubString(const unsigned int &Start) const;
	BasicString SubString(const unsigned int &Start, const unsigned int &Count) const;

	void Erase(const unsigned int &Start);
	void Erase(const unsigned int &Start, const unsigned int &Count);

	BasicString Trim(const bool &Left, const bool &Right) const;
	BasicString TrimAll(void) const;
	BasicString TrimLeft(void) const;
	BasicString TrimRight(void) const;
	
	const BasicString &Remove(const BasicString &eWhat) const;
	
	const BasicString &Replace(const char &What, const char &With) const;
	const BasicString &Replace(const char *What, const char *With) const;
	const BasicString &Replace(const BasicString &What, const BasicString &With) const;

	bool StartsWith(const BasicString &Pattern, const bool &CheckInLowerCase = false) const;
	bool EndsWith(const BasicString &Pattern, const bool &CheckInLowerCase = false) const;

	List<BasicString> Split(const BasicString &Delimiters = "\t\n ") const;

	BasicString ToLowerCase(void) const;
	BasicString ToUpperCase(void) const;

	void Clear(void) const;

private:
	void Free(void) const;

public:
	char &operator [](const unsigned int &Index);
	const char &operator [](const unsigned int &Index) const;
	
	//String operator +(const char Value) const;
	BasicString operator +(const char *Value) const;
	BasicString operator +(const BasicString &Value) const;

	void operator =(const char *Value);
	void operator =(const BasicString &Value);
	
	void operator +=(const char Value);
	void operator +=(const char *Value);
	void operator +=(const BasicString &Value);
	
	//const bool operator ==(const String &Value);

	const char *GetBuffer(void) const
	{
		return m_Buffer;
	}

	const unsigned int GetLength(void) const
	{
		return m_Length;
	}

private:
	static char NULL_CHARACTER;

	mutable char *m_Buffer;
	mutable unsigned int m_Length;

	static unsigned int m_ConstructorCount;
	static unsigned int m_DestructorCount;
	static unsigned int m_TotalSizeOf;
};


DLL_DECLARATION BasicString operator +(char LeftHand, const BasicString &RightHand);
DLL_DECLARATION BasicString operator +(const BasicString &LeftHand, char RightHand);
DLL_DECLARATION BasicString operator +(const char *LeftHand, const BasicString &RightHand);

DLL_DECLARATION const bool operator ==(const BasicString &LeftHand, const BasicString &RightHand);
DLL_DECLARATION const bool operator ==(const BasicString &LeftHand, const char *RightHand);
DLL_DECLARATION const bool operator ==(const char *LeftHand, const BasicString &RightHand);

DLL_DECLARATION const bool operator !=(const BasicString &LeftHand, const BasicString &RightHand);
DLL_DECLARATION const bool operator !=(const BasicString &LeftHand, const char *RightHand);
DLL_DECLARATION const bool operator !=(const char *LeftHand, const BasicString &RightHand);

DLL_DECLARATION std::ostream &operator <<(std::ostream &Stream, const BasicString &Value);

DLL_DECLARATION bool operator <(const BasicString &Right , const std::string &Left);
DLL_DECLARATION bool operator <(const std::string &Left, const BasicString &Right);
DLL_DECLARATION bool operator <(const BasicString &Left, const BasicString &Right);

typedef List<BasicString> StringsList;

END_NAMESPACE

//#endif