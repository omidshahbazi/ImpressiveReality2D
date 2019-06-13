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

#define USE_NAMESPACE
#ifdef USE_NAMESPACE
	#define NAMESPACE IE2D

	#define BEGIN_NAMESPACE namespace NAMESPACE \
							{
	#define END_NAMESPACE	}

	#define USING_NAMESPACE using namespace NAMESPACE;
#else
	#define BEGIN_NAMESPACE
	#define END_NAMESPACE

	#define USING_NAMESPACE
#endif


#define USE_DLL_DECLARATION
#ifdef USE_DLL_DECLARATION
	#define DLL_DECLARATION __declspec(dllexport)
	//#define EXTERN_TEMPLATE
#else
	#define DLL_DECLARATION
	//#define EXTERN_TEMPLATE extern
#endif