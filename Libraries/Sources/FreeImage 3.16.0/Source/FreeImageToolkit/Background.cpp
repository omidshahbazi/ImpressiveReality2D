  
 �T$V��t,�A��r�1���;�r��r�1���A�;�v�^� 2�^�    �   D           ;              .           3              �         �   �   s G            ;      8   C         std::basic_string<char,std::char_traits<char>,std::allocator<char> >::_Inside                        H  9   this     6  _Ptr  O �   H           ;   `      <       � �    � �/   � �2   � �5   � �8   � �      x      |     
          
 �yr�Ë��   �   $                          .         �   �   Y G                      L         std::_String_val<std::_Simple_types<char> >::_Myptr                         H     this  O�   @              `      4       � �     �    �	    �    �      X      \     
 �      �     
 �yr�Ë��   �   $                          .         �   �   Y G                      �         std::_String_val<std::_Simple_types<char> >::_Myptr                         H     this  O�   @              `      4        �     �   	 �	    �   	 �      X      \     
 �      �     
 �|$ VW�|$��t"�~rS���tWSV�    ��S�    ��[�~�F   �7 _^�     �   )   �      �   �           D              .           ;              �           9              �                         �         �   �   q G            D      A   �         std::basic_string<char,std::char_traits<char>,std::allocator<char> >::_Tidy                        H  9   this     0   _Built     u   _Newsize  O�   p           D   `      d       � �    � �   � �   � �   � �   � �'   � �1   � �4   � �;   � �A   � �      �      �     
 l     p    
 h    �    �   i      �      �   $                          .         �   �   q G                      D        std::basic_string<char,std::char_traits<char>,std::allocator<char> >::_Xlen                         H        $LN3        $LN4  �   this  9       �   O�   (              `             � �    � �      X      \     
 �   �   �   �  
 �   �   �   �  
 �           
          
 h    �    �   l      �      �   $                          .         �   �   q G                      E        std::basic_string<char,std::char_traits<char>,std::allocator<char> >::_Xran                         H        $LN3        $LN4  �   this  9       �   O�   (              `             � �    � �      X      \     
 �   �   �   �  
 �   �   �   �  
 �           
          
 �D$3Ʌ�t���wP�    �ȃ���u�    ���    �       �      �   $           )              .         �   �   W G            )       )   �         std::_Wrap_alloc<std::allocator<char> >::allocate                         H        $LN14        $LN15  �   this     u   _Count  9       �   O �   0           )   �      $       N �    O �&   P �   �   X   �   \   �  
 �   �   �   �  
 �   �   �   �  
 �   �   �   �  
   �     �  
 �D$3Ʌ�t���wP�    �ȃ���u�    ���    �       �      �   $           )              .         �   �   D G            )       )   �         std::allocator<char>::allocate                         H        $LN11        $LN12  �   this     u   _Count  9       �   O�   0           )   �      $       A �    B �&   C �   �   X   �   \   �  
 �   �   �   �  
 �   �   �   �  
 �   �   �   �  
 �   �      �  
 S�\$U�l$VW�{��;�sh    �    +�9|$B|$;�u5�/9Fsh    �    �~�Fr�U� ��j �    _��^][� ���vh    �    �F;�s�vW�    ��tj�{r��~r*��(��u�~��r�_�  ��^][� _��^][�  � �΅�tW�+PQ�    ���~�~r��8 ��_^][� ���8 _��^][�    l      �   8   l   >   �   W      j   i   p   �   �      �   �      �   �           �              .           �              8           �              	           �              X   is implicit here so, set
		// index to color->rgbReserved
		result = color->rgbReserved;
		if (bpp == 1) {
			result &= 0x01;
		} else if (bpp == 4) {
			result &= 0x0F;
		}		

		unsigned ucolor;
		if (!IsVisualGreyscaleImage(dib)) {
			ucolor = (*((unsigned *)color)) & 0xFFFFFF;
		} else {
			ucolor = GREY(color->rgbRed, color->rgbGreen, color->rgbBlue) * 0x010101;
			//ucolor = (ucolor | (ucolor << 8) | (ucolor << 16));
		}
		unsigned ncolors = FreeImage_GetColorsUsed(dib);
		unsigned *palette = (unsigned *)FreeImage_GetPalette(dib);
		for (unsigned i = 0; i < ncolors; i++) {
			if ((palette[i] & 0xFFFFFF) == ucolor) {
				result = i;
				break;
			}
		}
	} else {
		unsigned minimum = UINT_MAX;
		unsigned ncolors = FreeImage_GetColorsUsed(dib);
		BYTE *palette = (BYTE *)FreeImage_GetPalette(dib);
		BYTE red, green, blue;
		if (!IsVisualGreyscaleImage(dib)) {
			red = color->rgbRed;
			green = color->rgbGreen;
			blue = color->rgbBlue;
		} else {
			red = GREY(color->rgbRed, color->rgbGreen, color->rgbBlue);
			green = blue = red;
		}
		for (unsigned i = 0; i < ncolors; i++) {
			unsigned m = abs(palette[FI_RGBA_BLUE] - blue)
					+ abs(palette[FI_RGBA_GREEN] - green)
					+ abs(palette[FI_RGBA_RED] - red);
			if (m < minimum) {
				minimum = m;
				result = i;
				if (m == 0) {
					break;
				}
			}
			palette += sizeof(RGBQUAD);
		}		
	}
	return result;
}

/** @brief Blends an alpha-transparent foreground color over an opaque background
 color.
 
 This function blends the alpha-transparent foreground color fgcolor over the
 background color bgcolor. The background color is considered fully opaque,
 whatever it's alpha value contains, whereas the foreground color is considered
 to be a real RGBA color with an alpha value, which is used for the blend
 operation. The resulting color is returned through the blended parameter.
 @param bgcolor The background color for the blend operation.
 @param fgcolor The foreground color for the blend operation. This color's alpha
 value, stored in the rgbReserved member, is the alpha value used for the blend
 operation.
 @param blended This out parameter takes the blended color and so, returns it to
 the caller. This color's alpha value will be 0xFF (255) so, the blended color
 itself has no transparency. The this argument is not changed, if the function
 fails. 
 @return Returns TRUE on success, FALSE otherwise. This function fails if any of
 the color arguments is a null pointer.
 */
static BOOL
GetAlphaBlendedColor(const RGBQUAD *bgcolor, const RGBQUAD *fgcolor, RGBQUAD *blended) {
	
	if ((!bgcolor) || (!fgcolor) || (!blended)) {
		return FALSE;
	}
	
	BYTE alpha = fgcolor->rgbReserved;
	BYTE not_alpha = ~alpha;
	
	blended->rgbRed   = (BYTE)( ((WORD)fgcolor->rgbRed   * alpha + not_alpha * (WORD)bgcolor->rgbRed)   >> 8 );
	blended->rgbGreen = (BYTE)( ((WORD)fgcolor->rgbGreen * alpha + not_alpha * (WORD)bgcolor->rgbGreen) >> 8) ;
	blended->rgbBlue  = (BYTE)( ((WORD)fgcolor->rgbRed   * alpha + not_alpha * (WORD)bgcolor->rgbBlue)  >> 8 );
	blended->rgbReserved = 0xFF;

	return TRUE;
}

/** @brief Fills a FIT_BITMAP image with the specified color.

 This function does the dirty work for FreeImage_FillBackground for FIT_BITMAP
 images.
 @param dib The image to be filled.
 @param color The color, the specified image should be filled with.
 @param options Options that affect the color search process for palletized images.
 @return Returns TRUE on success, FALSE otherwise. This function fails if any of
 the dib and color is NULL or the provided image is not a FIT_BITMAP image.
 */
static BOOL
FillBackgroundBitmap(FIBITMAP *dib, const RGBQUAD *color, int options) {

	if ((!dib) || (FreeImage_GetImageType(dib) != FIT_BITMAP)) {
		return FALSE;;
	}
	
	if (!color) {
		return FALSE;
	}
	
	const RGBQUAD *color_intl = color;
	unsigned bpp = FreeImage_GetBPP(dib);
	unsigned width = FreeImage_GetWidth(dib);
	unsigned height = FreeImage_GetHeight(dib);
	
	FREE_IMAGE_COLOR_TYPE color_type = FreeImage_GetColorType(dib);
	
	// get a pointer to the first scanline (bottom line)
	BYTE *src_bits = FreeImage_GetScanLine(dib, 0);
	BYTE *dst_bits = src_bits;	
	
	BOOL supports_alpha = ((bpp >= 24) || ((bpp == 8) && (color_type != FIC_PALETTE)));
	
	// Check for RGBA case if bitmap supports alpha 
	// blending (8-bit greyscale, 24- or 32-bit images)
	if (supports_alpha && (options & FI_COLOR_IS_RGBA_COLOR)) {
		
		if (color->rgbReserved == 0) {
			// the fill color is fully transparent; we are done
			return TRUE;
		}
		
		// Only if the fill color is NOT fully opaque, draw it with
		// the (much) slower FreeImage_DrawLine function and return.
		// Since we do not have the FreeImage_DrawLine function in this
		// release, just assume to have an unicolor background and fill
		// all with an 'alpha-blended' color.
		if (color->rgbReserved < 255) {
							
			// If we will draw on an unicolor background, it's
			// faster to draw opaque with an alpha blended color.
			// So, first get the color from the first pixel in the
			// image (bottom-left pixel).
			RGBQUAD bgcolor;
			if (bpp == 8) {
				bgcolor = FreeImage_GetPalette(dib)[*src_bits];
			} else {	
				bgcolor.rgbBlue = src_bits[FI_RGBA_BLUE];
				bgcolor.rgbGreen = src_bits[FI_RGBA_GREEN];
				bgcolor.rgbRed = src_bits[FI_RGBA_RED];
				bgcolor.rgbReserved = 0xFF;
			}
			RGBQUAD blend;
			GetAlphaBlendedColor(&bgcolor, color_intl, &blend);
			color_intl = &blend;
		}
	}
	
	int index = (bpp <= 8) ? GetPaletteIndex(dib, color_intl, options, &color_type) : 0;
	if (index == -1) {
		// No palette index found for a palletized
		// image. This should never happen...
		return FALSE;
	}
	
	// first, build the first scanline (line 0)
	switch (bpp) {
		case 1: {
			unsigned bytes = (width / 8);
			memset(dst_bits, ((index == 1) ? 0xFF : 0x00), bytes);
			//int n = width % 8;
			int n = width & 7;
			if (n) {
				if (index == 1) {
					// set n leftmost bits
					dst_bits[bytes] |= (0xFF << (8 - n));
				} else {
					// clear n leftmost bits
					dst_bits[bytes] &= (0xFF >> n);
				}
			}
			break;
		}
		case 4: {
			unsigned bytes = (width / 2);
			memset(dst_bits, (index | (index << 4)), bytes);
			//if (bytes % 2) {
			if (bytes & 1) {
				dst_bits[bytes] &= 0x0F;
				dst_bits[bytes] |= (index << 4);
			}
			break;
		}
		case 8: {
			memset(dst_bits, index, FreeImage_GetLine(dib));
			break;
		}
		case 16: {
			WORD wcolor = RGBQUAD_TO_WORD(dib, color_intl);
			for (unsigned x = 0; x < width; x++) {
				((WORD *)dst_bits)[x] = wcolor;
			}
			break;
		}
		case 24: {
			RGBTRIPLE rgbt = *((RGBTRIPLE *)color_intl);
			for (unsigned x = 0; x < width; x++) {
				((RGBTRIPLE *)dst_bits)[x] = rgbt;
			}
			break;
		}
		case 32: {
			RGBQUAD rgbq;
			rgbq.rgbBlue = ((RGBTRIPLE *)color_intl)->rgbtBlue;
			rgbq.rgbGreen = ((RGBTRIPLE *)color_intl)->rgbtGreen;
			rgbq.rgbRed = ((RGBTRIPLE *)color_intl)->rgbtRed;
			rgbq.rgbReserved = 0xFF;
			for (unsigned x = 0; x < width; x++) {
				((RGBQUAD *)dst_bits)[x] = rgbq;
			}
			break;
		}
		default:
			return FALSE;
	}

	// Then, copy the first scanline into all following scanlines.
	// 'src_bits' is a pointer to the first scanline and is already
	// set up correctly.
	if (src_bits) {
		unsigned pitch = FreeImage_GetPitch(dib);
		unsigned bytes = FreeImage_GetLine(dib);
		dst_bits = src_bits + pitch;
		for (unsigned y = 1; y < height; y++) {
			memcpy(dst_bits, src_bits, bytes);
			dst_bits += pitch;
		}
	}
	return TRUE;
}

/** @brief Fills an image with the specified color.

 This function sets all pixels of an image to the color provided through the color
 parameter. Since this should work for all image types supported by FreeImage, the
 pointer color must point to a memory location, which is at least as large as the
 image's color value, if this size is greater than 4 bytes. As the color is specified
 by an RGBQUAD structure for all images of type FIT_BITMAP (including all palletized
 images), the smallest possible size of this memory is the size of the RGBQUAD structure,
 which uses 4 bytes.

 So, color must point to a double, if the image to be filled is of type FIT_DOUBLE and
 point to a RGBF structure if the image is of type FIT_RGBF and so on.

 However, the fill color is always specified through a RGBQUAD structure for all images
 of type FIT_BITMAP. So, for 32- and 24-bit images, the red, green and blue members of
 the RGBQUAD structure are directly used for the image's red, green and blue channel
 respectively. Although alpha transparent RGBQUAD colors are supported, the alpha channel
 of a 32-bit image never gets modified by this function. A fill color with an alpha value
 smaller than 255 gets blended with the image's actual background color, which is determined
 from the image's bottom-left pixel. So, currently using alpha enabled colors, assumes the
 image to be unicolor before the fill operation. However, the RGBQUAD's rgbReserved member is
 only taken into account, if option FI_COLOR_IS_RGBA_COLOR has been specified.

 For 16-bit images, the red-, green- and blue components of the specified color are
 transparently translated into either the 16-bit 555 or 565 representation. This depends
 on the image's actual red- green- and blue masks.

 Special attention must be payed for palletized images. Generally, the RGB color specified
 is looked up in the image's palette. The found palette index is then used to fill the image.
 There are some option flags, that affect this lookup process:

 no option specified       (0x00)   Uses the color, that is nearest to the specified color.
                                    This is the default behavior and should always find a
                                    color in the palette. However, the visual result may
                                    far from what was expected and mainly depends on the
                                    image's palette.

 FI_COLOR_FIND_EQUAL_COLOR (0x02)	Searches the image's palette for the specified color
                                    but only uses the returned palette index, if the specified
                                    color exactly matches the palette entry. Of course,
                                    depending on the image's actual palette entries, this
                                    operation may fail. In this case, the function falls back
                                    to option FI_COLOR_ALPHA_IS_INDEX and uses the RGBQUAD's
                                    rgbReserved member (or its low nibble for 4-bit images
                                    or its least significant bit (LSB) for 1-bit images) as
                                    the palette index used for the fill operation.

 FI_COLOR_ALPHA_IS_INDEX   (0x04)   Does not perform any color lookup from the palette, but
                                    uses the RGBQUAD's alpha channel member rgbReserved as
                                    the palette index to be used for the fill operation.
                                    However, for 4-bit images, only the low nibble of the
                                    rgbReserved member are used and for 1-bit images, only
                                    the least significant bit (LSB) is used.

 This function fails if any of dib and color is NULL.

 @param dib The image to be filled.
 @param color A pointer to the color value to be used for filling the image. The
 memory pointed to by this pointer is always assumed to be at least as large as the
 image's color value, but never smaller than the size of an RGBQUAD structure.
 @param options Options that affect the color search process for palletized images.
 @return Returns TRUE on success, FALSE otherwise. This function fails if any of
 dib and color is NULL.
 */
BOOL DLL_CALLCONV
FreeImage_FillBackground(FIBITMAP *dib, const void *color, int options) {

	if (!FreeImage_HasPixels(dib)) {
		return FALSE;
	}
	
	if (!color) {
		return FALSE;
	}

	// handle FIT_BITMAP images with FreeImage_FillBackground()
	if (FreeImage_GetImageType(dib) == FIT_BITMAP) {
		return FillBackgroundBitmap(dib, (RGBQUAD *)color, options);
	}
	
	// first, construct the first scanline (bottom line)
	unsigned bytespp = (FreeImage_GetBPP(dib) / 8);
	BYTE *src_bits = FreeImage_GetScanLine(dib, 0);
	BYTE *dst_bits = src_bits;
	for (unsigned x = 0; x < FreeImage_GetWidth(dib); x++) {
		memcpy(dst_bits, color, bytespp);
		dst_bits += bytespp;
	}

	// then, copy the first scanline into all following scanlines
	unsigned height = FreeImage_GetHeight(dib);
	unsigned pitch = FreeImage_GetPitch(dib);
	unsigned bytes = FreeImage_GetLine(dib);
	dst_bits = src_bits + pitch;
	for (unsigned y = 1; y < height; y++) {
		memcpy(dst_bits, src_bits, bytes);
		dst_bits += pitch;
	}
	return TRUE;
}

/** @brief Allocates a new image of the specified type, width, height and bit depth and
 optionally fills it with the specified color.

 This function is an extension to FreeImage_AllocateT, which additionally supports specifying
 a palette to be set for the newly create image, as well as specifying a background color,
 the newly created image should initially be filled with.

 Basically, this function internally relies on function FreeImage_AllocateT, followed by a
 call to FreeImage_FillBackground. This is why both parameters color and options behave the
 same as it is documented for function FreeImage_FillBackground. So, please refer to the
 documentation of FreeImage_FillBackground to learn more about parameters color and options.

 The palette specified through parameter palette is only copied to the newly created
 image, if its image type is FIT_BITMAP and the desired bit depth is smaller than or equal
 to 8 bits per pixel. In other words, the palette parameter is only taken into account for
 palletized images. However, if the preceding conditions match and if palette is not NULL,
 the memory pointed to by the palette pointer is assumed to be at least as large as size
 of a fully populated palette for the desired bit depth. So, for an 8-bit image, this size
 is 256 x sizeof(RGBQUAD), for an 4-bit image it is 16 x sizeof(RGBQUAD) and it is
 2 x sizeof(RGBQUAD) for a 1-bit image. In other words, this function does not support
 partial palettes.

 However, specifying a palette is not necessarily needed, even for palletized images. This
 function is capable of implicitly creating a palette, if parameter palette is NULL. If the
 specified background color is a greyscale value (red = green = blue) or if option
 FI_COLOR_ALPHA_IS_INDEX is specified, a greyscale palette is created. For a 1-bit image, only
 if the specified background color is either black or white, a monochrome palette, consisting
 of black and white only is created. In any case, the darker colors are stored at the smaller
 palette indices.

 If the specified background color is not a greyscale value, or is neither black nor white
 for a 1-bit image, solely this single color is injected into the otherwise black-initialized
 palette. For this operation, option FI_COLOR_ALPHA_IS_INDEX is implicit, so the specified
 color is applied to the palette entry, specified by the background color's rgbReserved
 member. The image is then filled with this palette index.

 This function returns a newly created image as function FreeImage_AllocateT does, if both
 parameters color and palette are NULL. If only color is NULL, the palette pointed to by
 parameter palette is initially set for the new image, if a palletized image of type
 FIT_BITMAP is created. However, in the latter case, this function returns an image, whose
 pixels are all initialized with zeros so, the image will be filled with the color of the
 first palette entry.

 @param type Specifies the image type of the new image.
 @param width The desired width in pixels of the new image.
 @param height The desired height in pixels of the new image.
 @param bpp The desired bit depth of the new image.
 @param color A pointer to the color value to be used for filling the image. The
 memory pointed to by this pointer is always assumed to be at least as large as the
 image's color value but never smaller than the size of an RGBQUAD structure.
 @param options Options that affect the color search process for palletized images.
 @param red_mask Specifies the bits used to store the red components of a pixel.
 @param green_mask Specifies the bits used to store the green components of a pixel.
 @param blue_mask Specifies the bits used to store the blue components of a pixel.
 @return Returns a pointer to a newly allocated image on success, NULL otherwise.
 */
FIBITMAP * DLL_CALLCONV
FreeImage_AllocateExT(FREE_IMAGE_TYPE type, int width, int height, int bpp, const void *color, int options, const RGBQUAD *palette, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {

	FIBITMAP *bitmap = FreeImage_AllocateT(type, width, height, bpp, red_mask, green_mask, blue_mask);
	
	if (!color) {
		if ((palette) && (type == FIT_BITMAP) && (bpp <= 8)) {
			memcpy(FreeImage_GetPalette(bitmap), palette, FreeImage_GetColorsUsed(bitmap) * sizeof(RGBQUAD));
		}
		return bitmap;
	}

	if (bitmap != NULL) {
		
		// Only fill the new bitmap if the specified color
		// differs from "black", that is not all bytes of the
		// color are equal to zero.
		switch (bpp) {
			case 1: {
				// although 1-bit implies FIT_BITMAP, better get an unsigned 
				// color and palette
				unsigned *urgb = (unsigned *)color;
				unsigned *upal = (unsigned *)FreeImage_GetPalette(bitmap);
				RGBQUAD rgbq = RGBQUAD();

				if (palette != NULL) {
					// clone the specified palette
					memcpy(FreeImage_GetPalette(bitmap), palette, 2 * sizeof(RGBQUAD));
				} else if (options & FI_COLOR_ALPHA_IS_INDEX) {
					CREATE_GREYSCALE_PALETTE(upal, 2);
				} else {
					// check, whether the specified color is either black or white
					if ((*urgb & 0xFFFFFF) == 0x000000) {
						// in any case build a FIC_MINISBLACK palette
						CREATE_GREYSCALE_PALETTE(upal, 2);
						color = &rgbq;
					} else if ((*urgb & 0xFFFFFF) == 0xFFFFFF) {
						// in any case build a FIC_MINISBLACK palette
						CREATE_GREYSCALE_PALETTE(upal, 2);
						rgbq.rgbReserved = 1;
						color = &rgbq;
					} else {
						// Otherwise inject the specified color into the so far
						// black-only palette. We use color->rgbReserved as the
						// desired palette index.
						BYTE index = ((RGBQUAD *)color)->rgbReserved & 0x01;
						upal[index] = *urgb & 0x00FFFFFF;  
					}
					options |= FI_COLOR_ALPHA_IS_INDEX;
				}
				// and defer to FreeImage_FillBackground
				FreeImage_FillBackground(bitmap, color, options);
				break;
			}
			case 4: {
				// 4-bit implies FIT_BITMAP so, get a RGBQUAD color
				RGBQUAD *rgb = (RGBQUAD *)color;
				RGBQUAD *pal = FreeImage_GetPalette(bitmap);
				RGBQUAD rgbq = RGBQUAD();
				
				if (palette != NULL) {
					// clone the specified palette
					memcpy(pal, palette, 16 * sizeof(RGBQUAD));
				} else if (options & FI_COLOR_ALPHA_IS_INDEX) {
					CREATE_GREYSCALE_PALETTE(pal, 16);
				} else {
					// check, whether the specified color is a grey one
					if ((rgb->rgbRed == rgb->rgbGreen) && (rgb->rgbRed == rgb->rgbBlue)) {
						// if so, build a greyscale palette
						CREATE_GREYSCALE_PALETTE(pal, 16);
						rgbq.rgbReserved = rgb->rgbRed >> 4;
						color = &rgbq;
					} else {
						// Otherwise inject the specified color into the so far
						// black-only palette. We use color->rgbReserved as the
						// desired palette index.
						BYTE index = (rgb->rgbReserved & 0x0F);
						((unsigned *)pal)[index] = *((unsigned *)rgb) & 0x00FFFFFF;
					}
					options |= FI_COLOR_ALPHA_IS_INDEX;
				}
				// and defer to FreeImage_FillBackground
				FreeImage_FillBackground(bitmap, color, options);
				break;
			}
			case 8: {
				// 8-bit implies FIT_BITMAP so, get a RGBQUAD color
				RGBQUAD *rgb = (RGBQUAD *)color;
				RGBQUAD *pal = FreeImage_GetPalette(bitmap);
				RGBQUAD rgbq;

				if (palette != NULL) {
					// clone the specified palette
					memcpy(pal, palette, 256 * sizeof(RGBQUAD));
				} else if (options & FI_COLOR_ALPHA_IS_INDEX) {
					CREATE_GREYSCALE_PALETTE(pal, 256);
				} else {
					// check, whether the specified color is a grey one
					if ((rgb->rgbRed == rgb->rgbGreen) && (rgb->rgbRed == rgb->rgbBlue)) {
						// if so, build a greyscale palette
						CREATE_GREYSCALE_PALETTE(pal, 256);
						rgbq.rgbReserved = rgb->rgbRed;
						color = &rgbq;
					} else {
						// Otherwise inject the specified color into the so far
						// black-only palette. We use color->rgbReserved as the
						// desired palette index.
						BYTE index = rgb->rgbReserved;
						((unsigned *)pal)[index] = *((unsigned *)rgb) & 0x00FFFFFF;  
					}
					options |= FI_COLOR_ALPHA_IS_INDEX;
				}
				// and defer to FreeImage_FillBackground
				FreeImage_FillBackground(bitmap, color, options);
				break;
			}
			case 16: {
				WORD wcolor = (type == FIT_BITMAP) ?
					RGBQUAD_TO_WORD(bitmap, ((RGBQUAD *)color)) : *((WORD *)color);
				if (wcolor != 0) {
					FreeImage_FillBackground(bitmap, color, options);
				}
				break;
			}
			default: {
				int bytespp = bpp / 8;
				for (int i = 0; i < bytespp; i++) {
					if (((BYTE *)color)[i] != 0) {
						FreeImage_FillBackground(bitmap, color, options);
						break;
					}
				}
				break;
			}
		}
	}
	return bitmap;
}

/** @brief Allocates a new image of the specified width, height and bit depth and optionally
 fills it with the specified color.

 This function is an extension to FreeImage_Allocate, which additionally supports specifying
 a palette to be set for the newly create image, as well as specifying a background color,
 the newly created image should initially be filled with.

 Basically, this function internally relies on function FreeImage_Allocate, followed by a
 call to FreeImage_FillBackground. This is why both parameters color and options behave the
 same as it is documented for function FreeImage_FillBackground. So, please refer to the
 documentation of FreeImage_FillBackground to learn more about parameters color and options.

 The palette specified through parameter palette is only copied to the newly created
 image, if the desired bit depth is smaller than or equal to 8 bits per pixel. In other words,
 the palette parameter is only taken into account for palletized images. However, if the
 image to be created is a palletized image and if palette is not NULL, the memory pointed to
 by the palette pointer is assumed to be at least as large as size of a fully populated
 palette for the desired bit depth. So, for an 8-bit image, this size is 256 x sizeof(RGBQUAD),
 for an 4-bit image it is 16 x sizeof(RGBQUAD) and it is 2 x sizeof(RGBQUAD) for a 1-bit
 image. In other words, this function does not support partial palettes.

 However, specifying a palette is not necessarily needed, even for palletized images. This
 function is capable of implicitly creating a palette, if parameter palette is NULL. If the
 specified background color is a greyscale value (red = green = blue) or if option
 FI_COLOR_ALPHA_IS_INDEX is specified, a greyscale palette is created. For a 1-bit image, only
 if the specified background color is either black or white, a monochrome palette, consisting
 of black and white only is created. In any case, the darker colors are stored at the smaller
 palette indices.

 If the specified background color is not a greyscale value, or is neither black nor white
 for a 1-bit image, solely this single color is injected into the otherwise black-initialized
 palette. For this operation, option FI_COLOR_ALPHA_IS_INDEX is implicit, so the specified
 color is applied to the palette entry, specified by the background color's rgbReserved
 member. The image is then filled with this palette index.

 This function returns a newly created image as function FreeImage_Allocate does, if both
 parameters color and palette are NULL. If only color is NULL, the palette pointed to by
 parameter palette is initially set for the new image, if a palletized image of type
 FIT_BITMAP is created. However, in the latter case, this function returns an image, whose
 pixels are all initialized with zeros so, the image will be filled with the color of the
 first palette entry.

 @param width The desired width in pixels of the new image.
 @param height The desired height in pixels of the new image.
 @param bpp The desired bit depth of the new image.
 @param color A pointer to an RGBQUAD structure, that provides the color to be used for
 filling the image.
 @param options Options that affect the color search process for palletized images.
 @param red_mask Specifies the bits used to store the red components of a pixel.
 @param green_mask Specifies the bits used to store the green components of a pixel.
 @param blue_mask Specifies the bits used to store the blue components of a pixel.
 @return Returns a pointer to a newly allocated image on success, NULL otherwise.
 */
FIBITMAP * DLL_CALLCONV
FreeImage_AllocateEx(int width, int height, int bpp, const RGBQUAD *color, int options, const RGBQUAD *palette, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {
	return FreeImage_AllocateExT(FIT_BITMAP, width, height, bpp, ((void *)color), options, palette, red_mask, green_mask, blue_mask);
}

/** @brief Enlarges or shrinks an image selectively per side and fills newly added areas
 with the specified background color.

 This function enlarges or shrinks an image selectively per side. The main purpose of this
 function is to add borders to an image. To add a border to any of the image's sides, a
 positive integer value must be passed in any of the parameters left, top, right or bottom.
 This value represents the border's width in pixels. Newly created parts of the image (the
 border areas) are filled with the specified color. Specifying a negative integer value for
 a certain side, will shrink or crop the image on this side. Consequently, specifying zero
 for a certain side will not change the image's extension on that side.

 So, calling this function with all parameters left, top, right and bottom set to zero, is
 effectively the same as calling function FreeImage_Clone; setting all parameters left, top,
 right and bottom to value equal to or smaller than zero, my easily be substituted by a call
 to function FreeImage_Copy. Both these cases produce a new image, which is guaranteed not to
 be larger than the input image. Thus, since the specified color is not needed in these cases,
 the pointer color may be NULL.

 Both parameters color and options work according to function FreeImage_FillBackground. So,
 please refer to the documentation of FreeImage_FillBackground to learn more about parameters
 color and options. For palletized images, the palette of the input image src is
 transparently copied to the newly created enlarged or shrunken image, so any color
 look-ups are performed on this palette.

 Here are some examples, that illustrate, how to use the parameters left, top, right and
 bottom:

 // create a white color
 RGBQUAD c;
 c.rgbRed = 0xFF;
 c.rgbGreen = 0xFF;
 c.rgbBlue = 0xFF;
 c.rgbReserved = 0x00;

 // add a white, symmetric 10 pixel wide border to the image
 dib2 = FreeImage_EnlargeCanvas(dib, 10, 10, 10, 10, &c, FI_COLOR_IS_RGB_COLOR);

 // add white, 20 pixel wide stripes to the top and bottom side of the image
 dib3 = FreeImage_EnlargeCanvas(dib, 0, 20, 0, 20, &c, FI_COLOR_IS_RGB_COLOR);

 // add white, 30 pixel wide stripes to the right side of the image and
 // cut off the 40 leftmost pixel columns
 dib3 = FreeImage_EnlargeCanvas(dib, -40, 0, 30, 0, &c, FI_COLOR_IS_RGB_COLOR);

 This function fails if either the input image is NULL or the pointer to the color is
 NULL, while at least on of left, top, right and bottom is greater than zero. This
 function also returns NULL, if the new image's size will be negative in either x- or
 y-direction.

 @param dib The image to be enlarged or shrunken.
 @param left The number of pixels, the image should be enlarged on its left side. Negative
 values shrink the image on its left side.
 @param top The number of pixels, the image should be enlarged on its top side. Negative
 values shrink the image on its top side.
 @param right The number of pixels, the image should be enlarged on its right side. Negative
 values shrink the image on its right side.
 @param bottom The number of pixels, the image should be enlarged on its bottom side. Negative
 values shrink the image on its bottom side.
 @param color The color, the enlarged sides of the image should be filled with.
 @param options Options that affect the color search process for palletized images.
 @return         �	 
         P`.debug$S        �   
 �
        @B.text$mn            )         P`.debug$S          = A        @B.text$mn           s �         P`.debug$S        �   �         @B.text$mn           � �         P`.debug$S          � �        @B.text$mn           �          P`.debug$S        �   +         @B.text$mn           1 I         P`.debug$S          ] m        @B.text$mn           � �         P`.debug$S        �   � �        @B.text$mn           � �         P`.debug$S          � �        @B.text$mn           ' ?         P`.debug$S        �   S 3        @B.text$mn           Q i         P`.debug$S          } �        @B.text$mn           � �         P`.debug$S        �   � �        @B.text$mn           � �         P`.debug$S                    @B.text$mn           3 K         P`.debug$S        �   _ ?        @B.text$mn           ] u         P`.debug$S          � �        @B.text$mn           � �         P`.debug$S        �   � �        @B.text$mn           �          P`.debug$S                   @B.text$mn           O g         P`.debug$S        �   { W        @B.text$mn           u �         P`.debug$S          � �         @B.text$mn           �  �          P`.debug$S        �   ! �!        @B.text$mn           " "         P`.debug$S          -" 1#        @B.text$mn           c# {#         P`.debug$S        �   �# o$        @B.text$mn           �$ �$         P`.debug$S          �$ �%        @B.text$mn           �% &         P`.debug$S        �   & �&        @B.text$mn           ' 5'         P`.debug$S          I' Q(        @B.text$mn           �( �(         P`.debug$S        �   �( �)        @B.text$mn           �) �)         P`.debug$S          �) �*        @B.text$mn           '+ ?+         P`.debug$S        �   S+ 7,        @B.text$mn           U, m,         P`.debug$S          �, �-        @B.text$mn           �- �-         P`.debug$S        �   �- �.        @B.text$mn           �. /         P`.debug$S          / !0        @B.text$mn           S0 k0         P`.debug$S        �   0 g1        @B.text$mn           �1 �1         P`.debug$S          �1 �2        @B.text$mn           �2 3         P`.debug$S        �   3 �3        @B.text$mn           4 54         P`.debug$S          I4 M5        @B.text$mn           5 �5         P`.debug$S        �   �5 �6        @B.text$mn           �6 �6         P`.debug$S          �6 �7        @B.text$mn           8 '8         P`.debug$S        �   ;8 9        @B.text$mn           99 Q9         P`.debug$S          e9 m:        @B.text$mn           �: �:         P`.debug$S        �   �: �;        @B.text$mn           �; �;         P`.debug$S          �; �<        @B.text$mn           += C=         P`.debug$S        �   W= 7>        @B.text$mn           U> m>         P`.debug$S          �> �?        @B.text$mn           �? �?         P`.debug$S        �   �? �@        @B.text$mn           �@ �@         P`.debug$S          A B        @B.text$mn           GB _B         P`.debug$S        �   sB SC        @B.text$mn           qC �C         P`.debug$S          �C �D        @B.text$mn           �D �D         P`.debug$S        �   �D �E        @B.text$mn           �E F         P`.debug$S          )F 1G        @B.