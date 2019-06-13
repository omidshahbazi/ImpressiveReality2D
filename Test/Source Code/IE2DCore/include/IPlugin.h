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

class IPlugin
{
public:
	virtual ~IPlugin(void) {}

	virtual void Install(void) = 0;

	virtual const String GetName(void) const = 0;
	
	virtual const String GetVersion(void) const = 0;
};

END_NAMESPACE

USING_NAMESPACE

#define LOAD_PLUGIN_FUNCTION LoadPlugin

#define LOAD_PLUGIN_FUNCTION_STRING "LoadPlugin"

#define BEGIN_LOAD_PLUGIN_DEFINITION extern "C" __declspec(dllexport) IPlugin *LOAD_PLUGIN_FUNCTION(void) \
										{
#define END_LOAD_PLUGIN_DEFINITION	}

typedef IPlugin* (*LoadPluginFunction)(void);