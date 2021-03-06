///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////



#ifndef INCLUDED_IMATHCOLORALGO_H
#define INCLUDED_IMATHCOLORALGO_H


#include "ImathColor.h"
#include "ImathMath.h"
#include "ImathLimits.h"

namespace Imath {


//
//	Non-templated helper routines for color conversion.
//	These routines eliminate type warnings under g++.
//

Vec3<double>	hsv2rgb_d(const Vec3<double> &hsv);

Color4<double>	hsv2rgb_d(const Color4<double> &hsv);


Vec3<double>	rgb2hsv_d(const Vec3<double> &rgb);

Color4<double>	rgb2hsv_d(const Color4<double> &rgb);


//
//	Color conversion functions and general color algorithms
//
//	hsv2rgb(), rgb2hsv(), rgb2packed(), packed2rgb()
//	see each funtion definition for details.
//

template<class T> 
Vec3<T>  
hsv2rgb(const Vec3<T> &hsv)
{
    if ( limits<T>::isIntegral() )
    {
	Vec3<double> v = Vec3<double>(hsv.x / double(limits<T>::max()),
				      hsv.y / double(limits<T>::max()),
				      hsv.z / double(limits<T>::max()));
	Vec3<double> c = hsv2rgb_d(v);
	return Vec3<T>((T) (c.x * limits<T>::max()),
		       (T) (c.y * limits<T>::max()),
		       (T) (c.z * limits<T>::max()));
    }
    else
    {
	Vec3<double> v = Vec3<double>(hsv.x, hsv.y, hsv.z);
	Vec3<double> c = hsv2rgb_d(v);
	return Vec3<T>((T) c.x, (T) c.y, (T) c.z);
    }
}


template<class T> 
Color4<T>  
hsv2rgb(const Color4<T> &hsv)
{
    if ( limits<T>::isIntegral() )
    {
	Color4<double> v = Color4<double>(hsv.r / float(limits<T>::max()),
					  hsv.g / float(limits<T>::max()),
					  hsv.b / float(limits<T>::max()),
					  hsv.a / float(limits<T>::max()));
	Color4<double> c = hsv2rgb_d(v);
	return Color4<T>((T) (c.r * limits<T>::max()),
    	    	    	 (T) (c.g * limits<T>::max()),
    	    	    	 (T) (c.b * limits<T>::max()),
			 (T) (c.a * limits<T>::max()));
    }
    else
    {
	Color4<double> v = Color4<double>(hsv.r, hsv.g, hsv.b, hsv.a);
	Color4<double> c = hsv2rgb_d(v);
	return Color4<T>((T) c.r, (T) c.g, (T) c.b, (T) c.a);
    }
}


template<class T> 
Vec3<T>  
rgb2hsv(const Vec3<T> &rgb)
{
    if ( limits<T>::isIntegral() )
    {
	Vec3<double> v = Vec3<double>(rgb.x / double(limits<T>::max()),
				      rgb.y / double(limits<T>::max()),
				      rgb.z / double(limits<T>::max()));
	Vec3<double> c = rgb2hsv_d(v);
	return Vec3<T>((T) (c.x * limits<T>::max()),
		       (T) (c.y * limits<T>::max()),
		       (T) (c.zor@D@2@@std@@@2@@std@@@Imf@@QAE@ABV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Z __unwindtable$??0?$TypedAttribute@V?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Imf@@QAE@ABV?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Z __ehfuncinfo$?copy@?$TypedAttribute@V?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Imf@@UBEPAVAttribute@2@XZ __unwindtable$?copy@?$TypedAttribute@V?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@@Imf@@UBEPAVAttribute@2@XZ __ehfuncinfo$?construct@?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@QAEXPAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@2@ABV32@@Z __unwindtable$?construct@?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@std@@QAEXPAV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@2@ABV32@@Z __ehfuncinfo$??0?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAE@ABV01@@Z __unwindtable$??0?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAE@ABV01@@Z __tryblocktable$??0?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAE@ABV01@@Z __catchsym$??0?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAE@ABV01@@Z$2 __ehfuncinfo$??4?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAEAAV01@ABV01@@Z __unwindtable$??4?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAEAAV01@ABV01@@Z __tryblocktable$??4?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAEAAV01@ABV01@@Z __catchsym$??4?$vector@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$allocator@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@2@@std@@QAEAAV01@ABV01@@Z$2 __ehfuncinfo$??0?$TypedAttribute@VTimeCode@Imf@@@Imf@@QAE@XZ __unwindtable$??0?$TypedAttribute@VTimeCode@Imf@@@Imf@@QAE@XZ __ehfuncinfo$