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
#include "Log.h"
#include "Exception.h"
#include "BasicString.h"
#include "SecondaryCommon.h"
#include "Vector.h"


//#define LAUNCH_MODE


#define USE_BREAK_PROCESS_IF
#ifdef USE_BREAK_PROCESS_IF
	#define BREAK_PROCESS_IF(Value) if (Value) \
									{ \
										_asm int 3 \
									}
#else
	#define BREAK_PROCESS_IF(Value);
#endif


#define USE_LOG
#ifdef USE_LOG
	#define LOG_TEXT(Text) Log::GetReference().AddText(Text);
	#define LOG_INFO(Text) Log::GetReference().AddInfo(Text);
	#define LOG_WARNING(Text) Log::GetReference().AddWarning(Text);
	#define LOG_ERROR(Text) Log::GetReference().AddError(Text);
#else
	#define LOG_TEXT(Text);
	#define LOG_INFO(Text);
	#define LOG_WARNING(Text);
	#define LOG_ERROR(Text);
#endif

#ifdef _DEBUG
	#define LOG_INFO_DEBUG(Text) LOG_INFO(Text)
	#define LOG_WARNING_DEBUG(Text) LOG_WARNING(Text)
	#define LOG_ERROR_DEBUG(Text) LOG_ERROR(Text)
#endif


#define USE_THROW_EXCEPTION
#if defined(_DEBUG) && defined(USE_THROW_EXCEPTION)
	#define THROW_EXCEPTION(Reason, Text, Detail, StopProcess) Exception::GetReference().Throw(Reason, Text, Detail, StopProcess);
#else
	#define THROW_EXCEPTION(Reason, Text, Detail, StopProcess);
#endif

#define THROW_EXCEPTION_AND_RESUME(Reason, Text, Detail) THROW_EXCEPTION(Reason, Text, Detail, false)
#define THROW_EXCEPTION_AND_STOP(Reason, Text, Detail) THROW_EXCEPTION(Reason, Text, Detail, true)


#define FOR_EACH(Variable_Name, Array) \
	for (auto Variable_Name = Array.GetFirst(), __end = Array.GetEnd(); Variable_Name != __end; Variable_Name++)


#define FOR_EACH_STD(Variable_Name, Array) \
	for (auto Variable_Name : Array)


#define FOR_EACH_MAP(Iterator_Name, Map) \
	for (auto Iterator_Name = Map.begin(); Iterator_Name != Map.end(); Iterator_Name++)
//#define FOR_EACH_MAP(MapType, Iterator_Name, Map) \
//	for (MapType::iterator Iterator_Name = Map.begin(); Iterator_Name != Map.end(); Iterator_Name++)


#define FIND_ITERATOR(Value, Array) \
	Array.FindIterator(Value)
//#define FIND_ITERATOR(Value, Array) \
//	std::find(Array.begin(), Array.end(), Value)


#define IS_POINTER_OF(Pointer, Type) \
	dynamic_cast<Type*>(Pointer)


#define ENABLE_FLAG(Mask, Flag) Mask |= Flag;
#define DISABLE_FLAG(Mask, Flag) Mask &= ~Flag;
#define IS_FLAG_ENABLED(Mask, Flag) Mask & Flag;


BEGIN_NAMESPACE

	//typedef std::string String;
	typedef BasicString String;
	//typedef List<String> StringsList;

END_NAMESPACE


//#define USE_RENDERER_FPS_SYSTEM