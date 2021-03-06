/* "$Id: mkg3states.c,v 1.8 2013/11/29 22:22:01 drolon Exp $ */

/*
 * Copyright (c) 1991-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/* Initialise fax decoder tables
 * Decoder support is derived, with permission, from the code
 * in Frank Cringle's viewfax program;
 *      Copyright (C) 1990, 1995  Frank D. Cringle.
 */
#include "tif_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "tif_fax3.h"

#ifndef HAVE_GETOPT
extern int getopt(int, char**, char*);
#endif

#define	streq(a,b)	(strcmp(a,b) == 0)

/* NB: can't use names in tif_fax3.h 'cuz they are declared const */
TIFFFaxTabEnt MainTable[128];
TIFFFaxTabEnt WhiteTable[4096];
TIFFFaxTabEnt BlackTable[8192];

struct proto {
    uint16 code;		/* right justified, lsb-first, zero filled */
    uint16 val;		/* (pixel count)<<4 + code width  */
};

static struct proto Pass[] = {
{ 0x0008, 4 },
{ 0, 0 }
};

static struct proto Horiz[]  = {
{ 0x0004, 3 },
{ 0, 0 }
};

static struct proto V0[]  = {
{ 0x0001, 1 },
{ 0, 0 }
};

static struct proto VR[]  = {
{ 0x0006, (1<<4)+3 },
{ 0x0030, (2<<4)+6 },
{ 0x0060, (3<<4)+7 },
{ 0, 0 }
};

static struct proto VL[]  = {
{ 0x0002, (1<<4)+3 },
{ 0x0010, (2<<4)+6 },
{ 0x0020, (3<<4)+7 },
{ 0, 0 }
};

static struct proto Ext[]  = {
{ 0x0040, 7 },
{ 0, 0 }
};

static struct proto EOLV[]  = {
{ 0x0000, 7 },
{ 0, 0 }
};

static struct proto MakeUpW[] = {
{ 0x001b, 1029 },
{ 0x0009, 2053 },
{ 0x003a, 3078 },
{ 0x0076, 4103 },
{ 0x006c, 5128 },
{ 0x00ec, 6152 },
{ 0x0026, 7176 },
{ 0x00a6, 8200 },
{ 0x0016, 9224 },
{ 0x00e6, 10248 },
{ 0x0066, 11273 },
{ 0x0166, 12297 },
{ 0x0096, 13321 },
{ 0x0196, 14345 },
{ 0x0056, 15369 },
{ 0x0156, 16393 },
{ 0x00d6, 17417 },
{ 0x01d6, 18441 },
{ 0x0036, 19465 },
{ 0x0136, 20489 },
{ 0x00b6, 21513 },
{ 0x01b6, 22537 },
{ 0x0032, 23561 },
{ 0x0132, 24585 },
{ 0x00b2, 25609 },
{ 0x0006, 26630 },
{ 0x01b2, 27657 },
{ 0, 0 }
};

static struct proto MakeUpB[] = {
{ 0x03c0, 1034 },
{ 0x0130, 2060 },
{ 0x0930, 3084 },
{ 0x0da0, 4108 },
{ 0x0cc0, 5132 },
{ 0x02c0, 6156 },
{ 0x0ac0, 7180 },
{ 0x06c0, 8205 },
{ 0x16c0, 9229 },
{ 0x0a40, 10253 },
{ 0x1a40, 11277 },
{ 0x0640, 12301 },
{ 0x1640, 13325 },
{ 0x09c0, 14349 },
{ 0x19c0, 15373 },
{ 0x05c0, 16397 },
{ 0x15c0, 17421 },
{ 0x0dc0, 18445 },
{ 0x1dc0, 19469 },
{ 0x0940, 20493 },
{ 0x1940, 21517 },
{ 0x0540, 22541 },
{ 0x1540, 23565 },
{ 0x0b40, 24589 },
{ 0x1b40, 25613 },
{ 0x04c0, 26637 },
{ 0x14c0, 27661 },
{ 0, 0 }
};

static struct proto MakeUp[] = {
{ 0x0080, 28683 },
{ 0x0180, 29707 },
{ 0x0580, 30731 },
{ 0x0480, 31756 },
{ 0x0c80, 32780 },
{ 0x0280, 33804 },
{ 0x0a80, 34828 },
{ 0x0680, 35852 },
{ 0x0e80, 36876 },
{ 0x0380, 37900 },
{ 0x0b80, 38924 },
{ 0x0780, 39948 },
{ 0x0f80,4�z��@ �v        ��         ��gO                   !     �  / �  	 �    �  g l u _                � � GLU_TESS_WINDING_NONZERO100131        ��hO                   !     �  / �  	 �  ! �  g l u _                � � GLU_TESS_WINDING_POSITIVE100132        ��iO                   !     �  / �  	 �  ! �  g l u _                � � GLU_TESS_WINDING_NEGATIVE100133        ��jO                   !     �  / �  	 �  $ �  g l u _                � �" GLU_TESS_WINDING_ABS_GEQ_TWO100134        ��kO                   !     �  P �  	 �   �  g l u _                � � GLU_TESS_BEGIN100100        ��lO                   !     �  P �  	 �   �  g l u _                � � GLU_TESS_VERTEX100101        ��mO                   !     �  P �  	 �   �  g l u _                � � GLU_TESS_END100102        ��nO                   !     �  P �  	 �   �  g l u _                � � GLU_TESS_ERROR100103        ��oO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_EDGE_FLAG100104        ��pO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_COMBINE100105        ��qO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_BEGIN_DATA100106        ��rO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_VERTEX_DATA100107        ��sO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_END_DATA100108        ��tO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_ERROR_DATA100109        ��uO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_EDGE_FLAG_DATA100110        ��vO                   !     �  X �  	 �   �  g l u _                � � GLU_TESS_COMBINE_DATA100111        ��wO                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR1100151        ��xO                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR2100152        ��yO                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR3100153        ��zO                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR4100154        ��{O                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR5100155        ��|O                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR6100156        ��}O                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR7100157        ��~O                   !     �  # �  	 �   �  g l u _                � � GLU_TESS_ERROR8100158        ��O                   !     �  8 �  	 �  & �  g l u _                � �- GLU_TESS_MISSING_BEGIN_POLYGONGLU_TESS_ERROR1        ���O                   !     �  8 �  	 �  & �  g l u _                � �- GLU_TESS_MISSING_BEGIN_CONTOURGLU_TESS_ERROR2        ���O                   !     �  8 �  	 �  $ �  g l u _                � �+ GLU_TESS_MISSING_END_POLYGONGLU_TESS_ERROR3        ���O                   !     �  8 �  	 �  $ �  g l u _                � �+ GLU_TESS_MISSING_END_CONTOURGLU_TESS_ERROR4        ���O                   !     �  8 �  	 �    �  g l u _                � �' GLU_TESS_COORD_TOO_LARGEGLU_TESS_ERROR5        ���O                   !     �  8 �  	 �  & �  g l u _                � �- GLU_TESS_NEED_COMBINE_CALLBACKGLU_TESS_ERROR6        ���O                   !     �  ' �  	 �   �  g l u _                � � GLU_AUTO_LOAD_MATRIX100200        ���O                   !     �  ' �  	 �   �  g l u _                � � GLU_CULLING100201                           �d���RmȬ>���&��J>J�IVI�HnH�G�G�����e)���<T�sR3�mB�1y��}� �'?ɡ�@ x       !��         ���O                   !     �  ' �  	 �   �  g l u _                � � GLU_SAMPLING_TOLERANCE100203        ���O                   !     �  ' �  	 �   �  g l u _                � � GLU_DISPLAY_MODE100204        ���O                   !     �  / �  	 �    �  g l u _                � � GLU_PARAMETRIC_TOLERANCE100202        ���O                   !     �  / �  	 �   �  g l u _                � � GLU_SAMPLING_METHOD100205        ���O                   !     �  / �  	 �   �  g l u _                �
 
� GLU_U_STEP100206        ���O                   !     �  / �  	 �   �  g l u _                �
 
� GLU_V_STEP100207        ���O                   !     �  / �  	 �   �  g l u _                � � GLU_PATH_LENGTH100215        ���O                   !     �  / �  	 �   �  g l u _                � � GLU_PARAMETRIC_ERROR100216        ���O                   !     �  / �  	 �   �  g l u _                � � GLU_DOMAIN_DISTANCE100217        ���O                   !     �  ' �  	 �   �  g l u _                � � GLU_MAP1_TRIM_2100210        ���O                   !     �  ' �  	 �   �  g l u _                � � GLU_MAP1_TRIM_3100211        ���O                   !       '   	      g l u _                � � GLU_OUTLINE_POLYGON100240        ���O                   !       '   	      g l u _                � � GLU_OUTLINE_PATCH100241        ���O                   !     
  ' 
  	 
   
  g l u _                � � GLU_NURBS_ERROR1100251        ���O                   !       '   	      g l u _                � � GLU_NURBS_ERROR2100252        ��