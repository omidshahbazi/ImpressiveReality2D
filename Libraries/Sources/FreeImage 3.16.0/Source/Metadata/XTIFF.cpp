// ==========================================================
// Metadata functions implementation
// Extended TIFF Directory GEO Tag Support
//
// Design and implementation by
// - Hervé Drolon (drolon@infonie.fr)
// - Thorsten Radde (support@IdealSoftware.com)
// - Berend Engelbrecht (softwarecave@users.sourceforge.net)
// - Mihail Naydenov (mnaydenov@users.sourceforge.net)
//
// Based on the LibTIFF xtiffio sample and on LibGeoTIFF
//
// This file is part of FreeImage 3
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ========================================================== 

#ifdef _MSC_VER
#pragma warning (disable : 4786) // identifier was truncated to 'number' characters
#endif

#include "../LibTIFF4/tiffiop.h"

#include "FreeImage.h"
#include "Utilities.h"
#include "FreeImageTag.h"
#include "FIRational.h"

// ----------------------------------------------------------
//   Extended TIFF Directory GEO Tag Support
// ----------------------------------------------------------

/**
  Tiff info structure.
  Entry format:
  { TAGNUMBER, ReadCount, WriteCount, DataType, FIELDNUM, OkToChange, PassDirCountOnSet, AsciiName }

  For ReadCount, WriteCount, -1 = unknown.
*/
static const TIFFFieldInfo xtiffFieldInfo[] = {
	{ TIFFTAG_GEOPIXELSCALE, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE, TRUE, "GeoPixelScale" },
	{ TIFFTAG_INTERGRAPH_MATRIX, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE, TRUE, "Intergraph TransformationMatrix" },
	{ TIFFTAG_GEOTRANSMATRIX, -1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE, TRUE, "GeoTransformationMatrix" },
	{ TIFFTAG_GEOTIEPOINTS,	-1, -1, TIFF_DOUBLE, FIELD_CUSTOM, TRUE, TRUE, "GeoTiePoints" },
	{ TIFFTAG_GEOKEYDIRECTORY,-1,-1, TIFF_SHORT, FIELD_CUSTOM, TRUE, TRUE, "GeoKeyDirectory" },
	{ TIFFTAG_GEODOUBLEPARAMS, -1, -1, TIFF_DOUBLE,	FIELD_CUSTOM, TRUE,	TRUE, "GeoDoubleParams" },
	{ TIFFTAG_GEOASCIIPARAMS, -1, -1, TIFF_ASCII, FIELD_CUSTOM, TRUE, FALSE, "GeoASCIIParams" },
	{ TIFFTAG_JPL_CARTO_IFD, 1, 1, TIFF_LONG, FIELD_CUSTOM, TRUE, TRUE,	"JPL Carto IFD offset" }  /** Don't use this! **/
};

static void
_XTIFFLocalDefaultDirectory(TIFF *tif) {
	int tag_size = sizeof(xtiffFieldInfo) / sizeof(xtiffFieldInfo[0]);
	// Install the extended Tag field info
	TIFFMergeFieldInfo(tif, xtiffFieldInfo, tag_size);
}

static TIFFExtendProc _ParentExtender;

/**
This is the callback procedure, and is
called by the DefaultDirectory method
every time a new TIFF directory is opened.
*/
static void
_XTIFFDefaultDirectory(TIFF *tif) {
	// set up our own defaults
	_XTIFFLocalDefaultDirectory(tif);

	/*
	Since an XTIFF client module may have overridden
	the default directory method, we call it now to
	allow it to set up the rest of its own methods.
	*/
	if (_ParentExtender)
		(*_ParentExtender)(tif);
}

/**
XTIFF Initializer -- sets up the callback procedure for the TIFF module
*/
void
XTIFFInitialize(void) {
	static int first_time = 1;

	if (! first_time)
		return; /* Been there. Done that. */
	first_time = 0;

	// Grab the inherited method and install
	_ParentExtender = TIFFSetTagExtender(_XTIFFDefaultDirectory);
}

// ----------------------------------------------------------
//   GeoTIFF tag reading / writing
// ----------------------------------------------------------

void
tiff_read_geotiff_profile(TIFF *tif, FIBITMAP *dib) {
	char defaultKey[16];

	size_t tag_size = sizeof(xtiffFieldInfo) / sizeof(xtiffFieldInfo[0]);

	TagLib& tag_lib   /      (   �        std::char_traits<char>::assign  �   �                 �     q  _Left     n  _Right  O   �   0           /   �     $       3 �   4 �(   5 �   �   X   �   \   �  
 �   �   �   �  
 U����   SVWQ��4����3   ������Y�M��E��@_^[��]�   �   $           0   �           �  #     �   t   ? G            0   #   )   j        std::error_code::category  �   �                �  �����  this  O�   0           0         $       �  �#   �  �)   �  �   2   X   2   \   2  
 �   2   �   2  
 U����   SVWQ��4����3   ������Y�M��E��@_^[��]�   �   $           0   �           �  #     �   y   D G            0   #   )   o        std::error_condition::category  �   �                �  �����  this  O   �   0           0         $       Q �#   R �)   S �   5   X   5   \   5  
 �   5   �   5  
 U����   SVW��<����1   ������} u�E��<�����MQ�UR�EP�    ����<�����<���_^[���   ;��    ��]�<   �   [   k      �   $           c   �          �       �   �   B G            c      O   �        std::char_traits<char>::copy  �   �                �     p  _First1     6  _First2     u   _Count  O   �   0           c   �     $        �    �O    �   �   X   �   \   �  
 �   �   �   �  
 U����   SVWQ��4����3   ������Y�M��EP�MQ�M��    _^[���   ;��    ��]� /      ?   k      �   $           I   �          �  #     �   �   Y G            I   #   3   o        std::_Wrap_alloc<std::allocator<char> >::deallocate  �   �                �  �����   this     p  _Ptr     u   _Count  O  �   0           I   0      $       Y �#   Z �3   [ �   
   X   
   \   
  
 �   
   �   
  
 U����   SVWQ��4����3   ������Y�M��EP�    ��_^[���   ;��    ��]� (   �   ;   k      �   $           E   �          �  #     �   �   F G            E   #   /   �        std::allocator<char>::deallocate  �   �                �  ����~   this     p  _Ptr     u   __formal  O   �   0           E   0      $       < �#   = �/   > �      X      \     
 �      �     
 U����   SVWQ��4����3   ������Y�M��EP�    ��_^[���   ;��    ��]� (   �   ;   k      �   $           E   �          �  #     �   �   W G            E   #   /   �        std::allocator<std::_Container_proxy>::deallocate  �   �                �  �����#  this     �  _Ptr     u   __formal  O  �   0           E   0      $       < �#   = �/   > �   U   X   U   \   U  
 �   U   �   U  
 U����   SVWQ��4����3   ������Y�M���EP�    ��;��    ��t�    P�EP�M�    �E���    P�EP�M�    �E_^[���   ;��    ��]� +   7   5   k   >   &   K   3   W   '   d   3   w   k      �   $           �   �          �  #     �   �   Z G            �   #   k   �        std::_System_error_category::default_error_condition  �   �                �  ����.  this     t   _Errval  9)          O �   H           �         <       f �#   g �=   h �T   i �V   j �k   k �   J   X   J   \   J  
 �   J   �   J  
 �   J   �   J  
 U����   SVWQ��4����3   ������Y�M��E�P�MQ�M�    �E_^[���   ;��    ��]� /   3   B   k      �   $           L   �          �  #     �   �   R G            L   #   6   q        std::error_category::default_error_condition  �   �                �  ����j  this     t   _Errval  O �   0           L         $       y �#   z �6   { �   *   X   *   \   *  
 �   *   �   *  
 U����   SVWQ��0����4   ������Y�M��M�    P�M��    ����t�M�    ;Euǅ0���   �
ǅ0���    ��0���_^[���   ;��    ��]� '   2   0   -   ?   1   p   k      �   $           z   �          �  #     �   �   E G            z   #   d   s        std::error_category::equivalent  �   �                �  ����j  this     q  _Code     t   _Errval  O�   0           z         $       � �#   � �d   � �   ,   X   ,   \   ,  
 �   ,   �   ,  
 U����   SVWQ��$����7   ������Y�M��EP��MQ��(���R�E���M��B��;��    ���    _^[���   ;��    ��]� D   k   K   6   [   k      �   $           e   �          D  #     �   �   E G            e   #   O   r        std::error_category::equivalent  �   �                �  ����j  this     t   _Errval     u  _Cond  9?       n   O�   0           e         $       � �#   � �O   � �   +   X   +   \   +  
 �   +   �   +  
 �   +   �   +  
 U����   SVWQ��4����3   ������Y�M��E��H;Ms�M��    �EP�M��    �E�_^[���   ;��    ��]� 2   #   >      Q   k      �   $           [   �          �  #     �   �   q G            [   #   E   a        std::basic_string<char,std::char_traits<char>,std::allocator<char> >::erase  �   �                �  ����m  this     u   _Off  O �   H           [         <       6 �#   7 �.   8 �6   9 �B   : �E   ; �      X      \     
 �      �     
 U����   SVWQ������9   ������Y�M��E��H;Ms�M��    �E��H+M;Mw�EP�M��    �F�} v@�M��    E�E�E��H+M�M��E�+EP�M�MQ�U�R�    ���E�P�M��    �E�_^[���   ;��    ��]� 2   #   L      \      �   �   �      �   k      �   $           �   �          (  #     �     q G            �   #   �   b        std::basic_string<char,std::char_traits<char>,std::allocator<char> >::erase  �   �   $             �  ����m  this     u   _Off     u   _Count          @   X       ����u   _Newsize  ����p  _Ptr    O �   x           �         l       > �#   ? �.   @ �6   A �D   B �R   C �X   E �f   F �r   G ��   H ��   J ��   K �      X      \     
          
 L     P    
 U����   SVW��@����0   ������    _^[��]�   �      �   $           *   �           |       �   _   ; G            *      #   �        std::generic_category  �   �                 �  O �   0           *         $       � �   � �#   � �   &   X   &   \   &  
 �   &   �   &  
 U����   SVW��<����1   ������E���uǅ<���    ��UR�    ����<�����<���_^[���   ;��    ��]�9   �   X   k      �   $           `   �          �       �   {   D G            `      L   �        std::char_traits<char>::length  �   �                �     6  _First  O �   0           `   �     $       	 �    �L    �   �   X   �   \   �  
 �   �   �   �  
 U����   SVWQ��4����3   ������Y�M��E�P�    ��_^[���   ;��    ��]�(      ;   k      �   $           C   �           �  #     �   �   W G            C   #   /           std::_Wrap_alloc<std::allocator<char> >::max_size  �   �                �  �����   this  O�   0           C   0      $       s �#   t �/   u �      X      \     
 �      �     
 U����   SVWQ��4����3   ������Y�M����_^[��]�   �   $           -   �           �  #     �   y   D G            -   #   &   (        std::allocator<char>::max_size  �   �                �  ����x   this  O   �   0           -   0      $       c �#   d �&   e �      X      \     
 �      �     
 U����   SVW��@����0   ������M�    _^[���   ;��    ��]�"      2   k      �   $           :   �          |       �   �   \ G            :      &           std::allocator_traits<std::allocator<char> >::max_size  �   �                 �     W  _Al  O�   0           :   0      $       � �   � �&   � �      X      \     
 �      �     
 U����   SVWQ������:   ������Y�M���#���P�M��    ���    �E�}�wǅ���   ��E������������_^[���   ;��    ��]�.      5      l   k      �   $           t   �           `  #     �   �   t G            t   #   `   �        std::basic_string<char,std::char_traits<char>,std::allocator<char> >::max_size  �   �   (             �  �����  this  ����u   _Num  O  �   8           t         ,       � �#   � �<   � �`   � �      X      \     
 �           
 U����   SVWQ������:   ������Y�M�ǅ ���    ��EP�    ��;��    �E�}� t�E쉅����
ǅ���    �����Q�M�    �� ������� ����E_^[���   ;��    ��]� 5   7   ?   k   ]   �   l      �   k      �   $           �   �          `  #     �   �   K G            �   #   �   �        std::_Generic_error_category::message  �   �   (             �  ����  this     t   _Errcode  ����6  _Name  93          O �   8           �         ,       3 �-   4 �F   5 ��   6 �   ;   X   ;   \   ;  
 �   ;   �   ;  
 �   ;   �   ;  
 U����   SVWQ��(����6   ������Y�M�ǅ,���    �}u#h    �M�    ��,�������,����E�$�"�EP�MQ�M��    ��,�������,����E_^[���   ;��    ��]� 4   �   <      b   ;   �   k      �   $           �   �          �  #     �   �   L G            �   #   x   �        std::_Iostream_error_category::message  �   �                �  ����  this     t   _Errcode  O  �   H           �         <       H �-   I �3   J �T   K �V   L �x   M �   B   X   B   \   B  
 �   B   �   B  
 U����   SVWQ������:   ������Y�M�ǅ ���    ��EP�    ��;��    �E�}� t�E쉅����
ǅ���    �����Q�M�    �� ������� ����E_^[���   ;��    ��]� 5   8   ?   k   ]   �   l      �   k      �   $           �   �          `  #     �   �   J G            �   #   �   �        std::_System_error_category::message  �   �   (             �  ����.  this     t   _Errcode  ����6  _Name  93          O  �   8           �         ,       _ �-   ` �F   a ��   b �   I   X   I   \   I  
 �   I   �   I  
 �   I   �   I  
 U����   SVW��<����1   ������} u�E��<����$��MQ�UR�EP�    ��;��    ��<�����<���_^[���   ;��    ��]�?   �   I   k   e   k      �   $           m   �          �       �   �   B G            m      Y   �        std::char_traits<char>::move  �   �                �     p  _First1     6  _First2     u   _Count  9=       2   O   �   0           m   �     $       ' �   ) �Y   * �   �   X   �   \   �  
 �   �   �   �  
 �   �   �   �  
 U����   SVWQ��4����3   ������Y�M��    _^[��]�$   �      �   $           /   �           �  #     �   }   H G            /   #   (   �        std::_Generic_error_category::name  �   �                �  ����  this  O   �   0           /         $       . �#   / �(   0 �   :   X   :   \   :  
 �   :   �   :  
 U����   SVWQ��4����3   ������Y�M��    _^[��]�$   �      �   $           /   �           �  #     �   ~   I G            /   #   (   �        std::_Iostream_error_category::name  �   �                �  ����  this  O  �   0           /         $       C �#   D �(   E �   A   X   A   \   A  
 �   A   �   A  
 U����   SVWQ��4����3   ������Y�M��    _^[��]�$   �      �   $           /   �           �  #     �   |   G G            /   #   (   �        std::_System_error_category::name  �   �                �  ����.  this  O�   0           /         $       Z �#   [ �(   \ �   H   X   H   \   H  
 �   H   �   H  
 U����   SVWQ��4����3   ������Y�M��E��@_^[��]�   �   $           0   �           �  #     �   �   p G            0   #   )   �        std::basic_string<char,std::char_traits<char>,std::allocator<char> >::size  �   �                �  �����  this  O   �   0           0         $       � �#   � �)   � �      X      \     
 �      �     
 U����   SVW��@����0   ������    _^[��]�   �      �   $           *   �           |       �   t   P G            *      #   b        Imf::TypedAttribute<float>::staticTypeName  �   �                 �  O�   0           *   �     $       4  �   5  �#   6  �   O   X   O   \   O  
 �   O   �   O  
 U����   SVW��@����0   ������    _^[��]�   �      �   $           *   �           |       �   ^   : G            *      #   �        std::system_category  �   �                 �  O  �IFFGetTagListCount(tif);
	for(i = 0; i < count; i++) {
		uint32 tag = TIFFGetTagListEntry(tif, i);
		// read the tag
		if (!tiff_read_exif_tag(tif, md_model, dib, tagLib, td, tag))
			return FALSE;
	}

	// we want to know values of standard tags too!!

	// loop over all Core Directory Tags
	// ### uses private data, but there is no other way
	if(md_model == TagLib::EXIF_MAIN) {

		uint32 lastTag = 0;	//<- used to prevent reading some tags twice (as stored in tif_fieldinfo)

		for (int fi = 0, nfi = (int)tif->tif_nfields; nfi > 0; nfi--, fi++) {
			const TIFFField *fld = tif->tif_fields[fi];

			if(fld->field_tag == lastTag)
				continue;

			// test if tag value is set
			// (lifted directly form LibTiff _TIFFWriteDirectory)

			if( fld->field_bit == FIELD_CUSTOM ) {
				int ci, is_set = FALSE;

				for( ci = 0; ci < td->td_customValueCount; ci++ ) {
					is_set |= (td->td_customValues[ci].info == fld);
				}

				if( !is_set ) {
					continue;
				}

			} else if(!TIFFFieldSet(tif, fld->field_bit)) {
				continue;
			}

			// process *all* other tags (some will be ignored)

			tiff_read_exif_tag(tif, md_model, dib, tagLib, td, fld->field_tag);


			lastTag = fld->field_tag;
		}

	}

	return TRUE;

}


/**
Skip tags that are already handled by the LibTIFF writing process
*/
static BOOL 
skip_write_field(TIFF* tif, uint32 tag) {
	switch (tag) {
		case TIFFTAG_SAMPLEFORMAT:
		case TIFFTAG_IMAGEWIDTH:
		case TIFFTAG_IMAGELENGTH:
		case TIFFTAG_SAMPLESPERPIXEL:
		case TIFFTAG_BITSPERSAMPLE:
		case TIFFTAG_PHOTOMETRIC:
		case TIFFTAG_PLANARCONFIG:
		case TIFFTAG_ROWSPERSTRIP:
		case TIFFTAG_STRIPBYTECOUNTS:
		case TIFFTAG_STRIPOFFSETS:
		case TIFFTAG_RESOLUTIONUNIT:
		case TIFFTAG_XRESOLUTION:
		case TIFFTAG_YRESOLUTION:
		case TIFFTAG_SUBFILETYPE:
		case TIFFTAG_PAGENUMBER:
		case TIFFTAG_COLORMAP:
		case TIFFTAG_ORIENTATION:
		case TIFFTAG_COMPRESSION:
		case TIFFTAG_PREDICTOR:
		case TIFFTAG_GROUP3OPTIONS:
		case TIFFTAG_FILLORDER:
			// skip always, values have been set in SaveOneTIFF()
			return TRUE;
			break;
		
		case TIFFTAG_RICHTIFFIPTC:
			// skip always, IPTC metadata model is set in tiff_write_iptc_profile()
			return TRUE;
			break;

		case TIFFTAG_YCBCRCOEFFICIENTS:
		case TIFFTAG_REFERENCEBLACKWHITE:
		case TIFFTAG_YCBCRSUBSAMPLING:
			// skip as they cannot be filled yet
			return TRUE;
			break;
			
		case TIFFTAG_PAGENAME:
		{
			char *value = NULL;
			TIFFGetField(tif, TIFFTAG_PAGENAME, &value);
			// only skip if no value has been set
			if(value == NULL) {
				return FALSE;
			} else {
				return TRUE;
			}
		}
		default:
			return FALSE;
			break;
	}
}

/**
Write all known exif tags
*/
BOOL 
tiff_write_exif_tags(TIFF *tif, TagLib::MDMODEL md_model, FIBITMAP *dib) {
	char defaultKey[16];
	
	// only EXIF_MAIN so far
	if(md_model != TagLib::EXIF_MAIN) {
		return FALSE;
	}
	
	if(FreeImage_GetMetadataCount(FIMD_EXIF_MAIN, dib) == 0) {
		return FALSE;
	}
	
	TagLib& tag_lib = TagLib::instance();
	
	for (int fi = 0, nfi = (int)tif->tif_nfields; nfi > 0; nfi--, fi++) {
		const TIFFField *fld = tif->tif_fields[fi];

		if(skip_write_field(tif, fld->field_tag)) {
			// skip tags that are already handled by the LibTIFF writing process
			continue;
		}

		FITAG *tag = NULL;
		// get the tag key
		const char *key = tag_lib.getTagFieldName(TagLib::EXIF_MAIN, (WORD)fld->field_tag, defaultKey);

		if(FreeImage_GetMetadata(FIMD_EXIF_MAIN, dib, key, &tag)) {
			FREE_IMAGE_MDTYPE tag_type = FreeImage_GetTagType(tag);
			TIFFDataType tif_tag_type = fld->field_type;
			
			// check for identical formats

			// (enum value are the sames between FREE_IMAGE_MDTYPE and TIFFDataType types)
			if((int)tif_tag_type != (int)tag_type) {
				// skip tag or _TIFFmemcpy will fail
				continue;
			}
			// type of storage may differ (e.g. rationnal array vs float array type)
			if(_TIFFDataSize(tif_tag_type) != FreeImage_TagDataWidth(tag_type)) {
				// skip tag or _TIFFmemcpy will fail
				continue;
			}

			if(tag_type == FIDT_ASCII) {
				TIFFSetField(tif, fld->field_tag, FreeImage_GetTagValue(tag));
			} else {
				TIFFSetField(tif, fld->field_tag, FreeImage_GetTagCount(tag), FreeImage_GetTagValue(tag));
			}
		}
	}

	return TRUE;
}
