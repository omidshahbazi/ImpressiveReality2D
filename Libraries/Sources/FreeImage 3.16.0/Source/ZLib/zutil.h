/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2013 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id: zutil.h,v 1.10 2013/05/10 17:22:52 drolon Exp $ */

#ifndef ZUTIL_H
#define ZUTIL_H

#ifdef HAVE_HIDDEN
#  define ZLIB_INTERNAL __attribute__((visibility ("hidden")))
#else
#  define ZLIB_INTERNAL
#endif

#include "zlib.h"

#if defined(STDC) && !defined(Z_SOLO)
#  if !(defined(_WIN32_WCE) && defined(_MSC_VER))
#    include <stddef.h>
#  endif
#  include <string.h>
#  include <stdlib.h>
#endif

#ifdef Z_SOLO
   typedef long ptrdiff_t;  /* guess -- will be caught if guess is wrong */
#endif

#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

typedef unsigned char  uch;
typedef uch FAR uchf;
typedef unsigned short ush;
typedef ush FAR ushf;
typedef unsigned long  ulg;

extern z_const char * const z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

        /* common constants */

#ifndef DEF_WBITS
#  define DEF_WBITS MAX_WBITS
#endif
/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

        /* target dependencies */

#if defined(MSDOS) || (defined(WINDOWS) && !defined(WIN32))
#  define OS_CODE  0x00
#  ifndef Z_SOLO
#    if defined(__TURBOC__) || defined(__BORLANDC__)
#      if (__STDC__ == 1) && (defined(__LARGE__) || defined(__COMPACT__))
         /* Allow compilation with ANSI keywords only enabled */
         void _Cdecl farfree( void *block );
         void *_Cdecl farmalloc( unsigned long nbytes );
#      else
#        include <alloc.h>
#      endif
#    else /* MSC or DJGPP */
#      include <malloc.h>
#    endif
#  endif
#endif

#ifdef AMIGA
#  define OS_CODE  0x01
#endif

#if defined(VAXC) || defined(VMS)
#  define OS_CODE  0x02
#  define F_OPEN(name, mode) \
     fopen((name), (mode), "mbc=60", "ctx=stm", "rfm=fix", "mrs=512")
#endif

#if defined(ATARI) || defined(atarist)
#  define OS_CODE  0x05
#endif

#ifdef OS2
#  define OS_CODE  0x06
#  if defined(M_I86) && !defined(Z_SOLO)
#    include <malloc.h>
#  endif
#endif

#if defined(MACOS) || defined(TARGET_OS_MAC)
#  define OS_CODE  0x07
#  ifndef Z_SOLO
#    if defined(__MWERKS__) && __dest_os != __be_os && __dest_os != __win32_os
#      include <unix.h> /* for fdopen */
#    else
#      ifndef fdopen
#        define fdopen(fd,mode) NULL /* No fdopen() */
#      endif
#    endif
#  endif
#endif

#ifdef TOPS20
#  define OS_CODE  0x0a
#endif

#ifdef WIN32
#  ifndef __CYGWIN__  /* Cygwin is Unix, not Win32 */
#    define OS_CODE  0x0b
#  endif
#endif

#ifdef __50SERIES /* Prime/PRIMOS */
#  define OS_CODE  0x0f
#endif

#if defined(_BEOS_) || defined(RISCOS)
#  define fdopen(fd,mode) NULL /* No fdopen() */
#endif

#if (defined(_MSC_VER) && (_MSC_VER > 600)) && !defined __INTERIX
#  if defined(_WIN32_WCE)
#    define fdopen(fd,mode) NULL /* No fdopen() */
#    ifndef _PTRDIFF_T_DEFINED
       typedef int ptrdiff_t;
#      define _PTRDIFF_T_DEFINED
#    endif
#  else
#    define fdopen(fd,type)  _fdopen(fd,type)
#  endif
#endif

#if defined(__BORLANDC__) && !defined(MSDOS)
  #pragma warn -8004
  #pragma warn -8008
  #pragma warn -8066
#endif

/* provide prototypes fk%  _s__RTTIBaseClassDescriptor2  �%  _s__CatchableType - �%  $_s__RTTIBaseClassArray$_extraBytes_28 & �%  $_TypeDescriptor$_extraBytes_19    _s__ThrowInfo  t   ptrdiff_t  X  _lldiv_t - �&  $_s__CatchableTypeArray$_extraBytes_20  �%  threadlocaleinfostruct - �%  $_s__RTTIBaseClassArray$_extraBytes_20 * �'  __vc_attributes::threadingAttribute 7 �'  __vc_attributes::threadingAttribute::threading_e / �'  __vc_attributes::event_receiverAttribute 7 �'  __vc_attributes::event_receiverAttribute::type_e - �'  __vc_attributes::aggregatableAttribute 5 ~'  __vc_attributes::aggregatableAttribute::type_e - {'  __vc_attributes::event_sourceAttribute 9 t'  __vc_attributes::event_sourceAttribute::optimize_e 5 r'  __vc_attributes::event_sourceAttribute::type_e ' o'  __vc_attributes::moduleAttribute / f'  __vc_attributes::moduleAttribute::type_e 9 c'  __vc_attributes::helper_attributes::usageAttribute B _'  __vc_attributes::helper_attributes::usageAttribute::usage_e > \'  __vc_attributes::helper_attributes::v1_alttypeAttribute F W'  __vc_attributes::helper_attributes::v1_alttypeAttribute::type_e & �%  $_TypeDescriptor$_extraBytes_22  :  _locale_t , �&  $_s__CatchableTypeArray$_extraBytes_8  S  ldiv_t  !   wint_t  L  _iobuf  .&  locrefcount  �  unexpected_handler  ''  _s__CatchableTypeArray & '  $_TypeDescriptor$_extraBytes_31 & '  $_TypeDescriptor$_extraBytes_46  p  va_list     __time64_t & }%  $_TypeDescriptor$_extraBytes_23 - �%  $_s__RTTIBaseClassArray$_extraBytes_32 & �%  $_TypeDescriptor$_extraBytes_51  #   _ULonglong & �%  $_TypeDescriptor$_extraBytes_18 & 2&  $_TypeDescriptor$_extraBytes_20  �  __type_info_node  �&  _TypeDescriptor ' �%  _s__RTTIClassHierarchyDescriptor  u   size_t  �  terminate_function  n%  _Dconst  t   mbstate_t  .&  localerefcount - �%  $_s__RTTIBaseClassArray$_extraBytes_12  �%  _s__RTTIBaseClassArray  L  FILE  �  __m64    _PMFN  �%  _PMD  ,  _Cvtvec  S  _ldiv_t  t   _Mbstatet  >  pthreadlocinfo    �   	     ��z���zB���<�  J   �wA}�8�g�G��D�  �   �[ll�`d�eCzպ�  �   ���zE��^d��M    V�X�eI��`	]��  l  �Э��q�`� �c���e  �  ��$���*�F���A\N�    ��Uix�p��Si �  U  �e�<$<5l����  �  Rך�wE��`�7�    ��i��I���-7�zk��  f  q���a,c�i+�M���  �  L�1����ʖ4Y�m�    9h`�^�j-w��'  a  wAJt�u�{#����R��  �  m��f�C߂i�PF��    U��&��N8C6��	p  L  )B�����C�H��  �  ��������b!!�	K?�  �  t8�V{��z�`1V̄Q�  *  �����{��>Aǵ  �  �քk�2��g�d��  �  �̽���C�ܡ�h     ��W
����i�
|~  f  ��2A	x��