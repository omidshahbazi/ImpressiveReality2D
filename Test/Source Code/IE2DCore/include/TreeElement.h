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
#include <map>


BEGIN_NAMESPACE

struct DLL_DECLARATION TreeElement
{
public:
	TreeElement(TreeElement *Parent = NULL);
	~TreeElement(void);

	TreeElement *GetChildren(const String &Name);
	TreeElement *GetChildrenByAttribute(const String &NameInAttributes);

	const String &GetAttributeValue(const String &Name);

public:
	typedef std::map<String, String> ElementsMap;
	typedef Vector<TreeElement*> TreeElementList;
	
	TreeElement *ParentElement;

	ElementsMap Attributes;
	TreeElementList Children;

	String Name;
	String Value;
};

END_NAMESPACE