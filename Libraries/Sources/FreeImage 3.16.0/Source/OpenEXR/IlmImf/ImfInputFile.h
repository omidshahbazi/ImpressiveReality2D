///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004, Industrial Light & Magic, a division of Lucas
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


#ifndef INCLUDED_IMF_INPUT_FILE_H
#define INCLUDED_IMF_INPUT_FILE_H

//-----------------------------------------------------------------------------
//
//	class InputFile -- a scanline-based interface that can be used
//	to read both scanline-based and tiled OpenEXR image files.
//
//-----------------------------------------------------------------------------

#include <ImfHeader.h>
#include <ImfFrameBuffer.h>
#include <ImfTiledOutputFile.h>
#include <string>
#include <fstream>
#include <ImfThreading.h>

namespace Imf {

class TiledInputFile;
class ScanLineInputFile;


class InputFile
{
  public:

    //-----------------------------------------------------------
    // A constructor that opens the file with the specified name.
    // Destroying the InputFile object will close the file.
    //
    // numThreads determines the number of threads that will be
    // used to read the file (see ImfThreading.h).
    //-----------------------------------------------------------

    InputFile (const char fileName[], int numThreads = globalThreadCount());


    //-------------------------------------------------------------
    // A constructor that attaches the new InputFile object to a
    // file that has already been opened.  Destroying the InputFile
    // object will not close the file.
    //
    // numThreads determines the number of threads that will be
    // used to read the file (see ImfThreading.h).
    //-------------------------------------------------------------

    InputFile (IStream &is, int numThreads = globalThreadCount());


    //-----------
    // Destructor
    //-----------

    virtual ~InputFile ();


    //------------------------
    // Access to the file name
    //------------------------

    const char *	fileName () const;


    //--------------------------
    // Access to the file header
    //--------------------------

    const Header &	header () const;


    //----------------------------------
    // Access to the file format version
    //----------------------------------

    int			version () const;


    //-----------------------------------------------------------
    // Set the current frame buffer -- copies the FrameBuffer
    // object into the InputFile object.
    //
    // The current frame buffer is the destinatio                      H     t   x     t   y  O   �   P           2   �     D       �  �    �  �   �  �   �  �#   �  �$   �  �1   �  �   �   x   �   |   �  
 �   �   �   �  
 �D$;Hu� ;D$u�� 2��    �   $                         X#         �   �   E G                      ^         std::error_category::equivalent                         @  &   this     -  _Code     t   _Errval  O  �   @              �     4       � �    � �   � �   � �   � �   O   X   O   \   O  
 �   O   �   O  
 ��T$����t$R�P�T$�H;Ju� ;u���� 2����    �   $           3             X#  	      �   �   E G            3   	   -   ]         std::error_category::equivalent                        @  &   this     t   _Errval     1  _Cond  9       *   O  �   @           3   �     4       � �    � �%   � �+   � �-   � �   N   X   N   \   N  
 �   N   �   N  
 �   N   �   N  
 �D$9Ash    �    �y�Ar�� ��� ��� ��� 
   �      $      �   $           3              X#         �   �   q G            3       3   <         std::basic_string<char,std::char_traits<char>,std::allocator<char> >::erase                         H        $LN18        $LN19  9   this     u   _Off  9       �   O �   `           3   8  	   T       6 �    7 �	   8 �   9 �#   : �%   ; �(   9 �.   : �0   ; �   >   X   >   \   >  
 �   �   �   �  
 �   �   �   �  
   >     >  
 (  >   ,  >  
 V��L$W�~;�sh    �    �T$��+�;�w!�~�Nr�_� ��^� ��_^� � ��tD�~r����+�S���+�tP�PS�    ���~�~[r��8 ��_^� ���8 _��^�    �      $   k         �   �           �              X#           �              �#           �              �#         Z                  $         �   
  q G            �      �   =         std::basic_string<char,std::char_traits<char>,std::allocator<char> >::erase                        @        $LN41        $LN42  9   this     u   _Off     u   _Count  9       �   9i       .   O  �   �           �   8     �       > �   ? �   @ �   A �&   B �6   J �9   K �<   B �D   K �G   C �K   E �W   F �]   G �r   H ��   J ��   K ��   H ��   J ��   K �   ?   �   ?   �   ?  
 -  �   1  �  
 >  �   B  �  
 �  ?   �  ?  
 �  ?   �  ?  
 �  ?   �  ?  
 �D$�D$�L$����%  ���   ����� =  �u&��t3���������� � �ø � ����V�����%�   ���= � r������^��   �   D           v              X#         S                  �#         �   �   P G            v       u   �"         Imf::`anonymous namespace'::floatToFloat24                        H     @   f @ H  Imf::?A0x0a2c3fa9::floatToFloat24::__l2::<unnamed-type-u>     H  u  O  �   �           v   �     �       \  �    c  �   j  �   k  �   l  �%   o  �,   q  �0   }  �:   �  �F   �  �G   �  �L   �  �Q   �  �S   �  �c   �  �j   �  �o   �  �u