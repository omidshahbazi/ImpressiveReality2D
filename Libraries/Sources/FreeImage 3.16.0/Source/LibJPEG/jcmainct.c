/*
 * jcmainct.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * Modified 2003-2012 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the main buffer controller for compression.
 * The main buffer lies between the pre-processor and the JPEG
 * compressor proper; it holds downsampled data in the JPEG colorspace.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"


/* Note: currently, there is no operating mode in which a full-image buffer
 * is needed at this step.  If there were, that mode could not be used with
 * "raw data" input, since this module is bypassed in that case.  However,
 * we've left the code here for possible use in special applications.
 */
#undef FULL_MAIN_BUFFER_SUPPORTED


/* Private buffer controller object */

typedef struct {
  struct jpeg_c_main_controller pub; /* public fields */

  JDIMENSION cur_iMCU_row;	/* number of current iMCU row */
  JDIMENSION rowgroup_ctr;	/* counts row groups received in iMCU row */
  boolean suspended;		/* remember if we suspended output */
  J_BUF_MODE pass_mode;		/* current operating mode */

  /* If using just a strip buffer, this points to the entire set of buffers
   * (we allocate one for each component).  In the full-image case, this
   * points to the currently accessible strips of the virtual arrays.
   */
  JSAMPARRAY buffer[MAX_COMPONENTS];

#ifdef FULL_MAIN_BUFFER_SUPPORTED
  /* If using full-image storage, this array holds pointers to virtual-array
   * control blocks for each component.  Unused if not full-image storage.
   */
  jvirt_sarray_ptr whole_image[MAX_COMPONENTS];
#endif
} my_main_controller;

typedef my_main_controller * my_main_ptr;


/* Forward declarations */
METHODDEF(void) process_data_simple_main
	JPP((j_compress_ptr cinfo, JSAMPARRAY input_buf,
	     JDIMENSION *in_row_ctr, JDIMENSION in_rows_avail));
#ifdef FULL_MAIN_BUFFER_SUPPORTED
METHODDEF(void) process_data_buffer_main
	JPP((j_compress_ptr cinfo, JSAMPARRAY input_buf,
	     JDIMENSION *in_row_ctr, JDIMENSION in_rows_avail));
#endif


/*
 * Initialize for a processing pass.
 */

METHODDEF(void)
start_pass_main (j_compress_ptr cinfo, J_BUF_MODE pass_mode)
{
  my_main_ptr mainp = (my_main_ptr) cinfo->main;

  /* Do nothing in raw-data mode. */
  if (cinfo->raw_data_in)
    return;

  mainp->cur_iMCU_row = 0;	/* initialize counters */
  mainp->rowgroup_ctr = 0;
  mainp->suspended = FALSE;
  mainp->pass_mode = pass_mode;	/* save mode for use by process_data */

  switch (pass_mode) {
  case JBUF_PASS_THRU:
#ifdef FULL_MAIN_BUFFER_SUPPORTED
    if (mainp->whole_image[0] != NULL)
      ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
#endif
    mainp->pub.process_data = process_data_simple_main;
    break;
#ifdef FULL_MAIN_BUFFER_SUPPORTED
  case JBUF_SAVE_SOURCE:
  case JBUF_CRANK_DEST:
  case JBUF_SAVE_AND_PASS:
    if (mainp->whole_image[0] == NULL)
      ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
    mainp->pub.process_data = process_data_buffer_main;
    break;
#endif
  default:
    ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
    break;
  }
}


/*
 * Process some data.
 * This routine handles the simple pass-through mode,
 * where we have only a strip buffer.
 */

METHODDEF(void)
process_data_simple_main (j_compress_ptr cinfo,
			  JSAMPARRAY input_buf, JDIMENSION *in_row_ctr,
			  JDIMENSION in_rows_avail)
{
  my_main_ptr mainp = (my_main_ptr) cinfo->main;

  while (mainp->cur_iMCU_row < cinfo->total_iMCU_rows) {
    /* Read input data if we haven't filled the main buffer yet */
    if (mainp->rowgroup_ctr < (JDIMENSION) cinfo->min_DCT_v_scaled_size)
      (*cinfo->prep->pre_process_data) (cinfo,
					input_buf, in_row_ctr, in_rows_avail,
					mainp->buffer, &mainp->rowgroup_ctr,
					(JDIMENSION) cinfo->min_DCT_v_scaled_size);

    /* If we don't have a full iMCU row buffered, return to application for
     * more data. ,�T��!P ��     _ _ i n l i n e   _ R e t u r n T y p e   _ _ C R T D E C L   _ _ i n s e c u r e _ # # _ V F u n c N a m e ( _ S a l A t t r i b u t e D s t   _ D s t T y p e   * _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   v a _ l i s t   _ A r g L i s t )   {   _ D e c l S p e c   _ R e t u r n T y p e   _ _ c d e c l   _ V F u n c N a m e ( _ D s t T y p e   * _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   v a _ l i s t   _ A r g L i s t ) ;   r e t u r n   _ V F u n c N a m e ( _ D s t ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ;   }   e x t e r n   " C + + "   {   _ _ p r a g m a ( w a r n i n g ( p u s h ) ) ;   _ _ p r a g m a ( w a r n i n g ( d i s a b l e :   4 7 9 3 ) ) ;   t e m p l a t e   < t y p e n a m e   _ T >   i n l i n e   _ C R T _ I N S E C U R E _ D E P R E C A T E ( _ F u n c N a m e # # _ s )   _ R e t u r n T y p e   _ _ C R T D E C L   _ F u n c N a m e ( _ T   & _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   . . . )   _ C R T _ S E C U R E _ C P P _ N O T H R O W   {   v a _ l i s t   _ A r g L i s t ;   _ c r t _ v a _ s t a r t ( _ A r g L i s t ,   _ T A r g 2 ) ;   r e t u r n   _ _ i n s e c u r e _ # # _ V F u n c N a m e ( s t a t i c _ c a s t < _ D s t T y p e   * > ( _ D s t ) ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ;   }   t e m p l a t e   < t y p e n a m e   _ T >   i n l i n e   _ C R T _ I N S E C U R E _ D E P R E C A T E ( _ F u n c N a m e # # _ s )   _ R e t u r n T y p e   _ _ C R T D E C L   _ F u n c N a m e ( c o n s t   _ T   & _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   . . . )   _ C R T _ S E C U R E _ C P P _ N O T H R O W   {   v a _ l i s t   _ A r g L i s t ;   _ c r t _ v a _ s t a r t ( _ A r g L i s t ,   _ T A r g 2 ) ;   r e t u r n   _ _ i n s e c u r e _ # # _ V F u n c N a m e ( s t a t i c _ c a s t < _ D s t T y p e   * > ( _ D s t ) ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ;   }   _ _ p r a g m a ( w a r n i n g ( p o p ) ) ;   _ _ p r a g m a ( w a r n i n g ( p u s h ) ) ;   _ _ p r a g m a ( w a r n i n g ( d i s a b l e :   4 7 9 3 ) ) ;   t e m p l a t e   < >   i n l i n e   _ C R T _ I N S E C U R E _ D E P R E C A T E ( _ F u n c N a m e # # _ s )   _ R e t u r n T y p e   _ _ C R T D E C L   _ F u n c N a m e ( _ D s t T y p e   *   & _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   . . . )   _ C R T _ S E C U R E _ C P P _ N O T H R O W   {   v a _ l i s t   _ A r g L i s t ;   _ c r t _ v a _ s t a r t ( _ A r g L i s t ,   _ T A r g 2 ) ;   r e t u r n   _ _ i n s e c u r e _ # # _ V F u n c N a m e ( _ D s t ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ;   }   _ _ p r a g m a ( w a r n i n g ( p o p ) ) ;   _ _ p r a g m a ( w a r n i n g ( p u s h ) ) ;   _ _ p r a g m a ( w a r n i n g ( d i s a b l e :   4 7 9 3 ) ) ;   t e m p l a t e   < s i z e _ t   _ S i z e >   i n l i n e   _ R e t u r n T y p e   _ _ C R T D E C L   _ F u n c N a m e ( _ S e c u r e D s t T y p e   ( & _ D s t ) [ _ S i z e ] ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   . . . )   _ C R T _ S E C U R E _ C P P _ N O T H R O W   {   v a _ l i s t   _ A r g L i s t ;   _ c r t _ v a _ s t a r t ( _ A r g L i s t ,   _ T A r g 2 ) ;   _ R e t u r n P o l i c y ( _ S e c u r e V F u n c N a m e ( _ D s t ,   _ S i z e ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ,   _ D s t ) ;   }   _ _ p r a g m a ( w a r n i n g ( p o p ) ) ;   _ _ p r a g m a ( w a r n i n g ( p u s h ) ) ;   _ _ p r a g m a ( w a r n i n g ( d i s a b l e :   4 7 9 3 ) ) ;   t e m p l a t e   < >   i n l i n e   _ C R T _ I N S E C U R E _ D E P R E C A T E ( _ F u n c N a m e # # _ s )   _ R e t u r n T y p e   _ _ C R T D E C L   _ F u n c N a m e < 1 > ( _ D s t T y p e   ( & _ D s t ) [ 1 ] ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   . . . )   _ C R T _ S E C U R E _ C P P _ N O T H R O W   {   v a _ l i s t   _ A r g L i s t ;   _ c r t _ v a ��B��!P ��     _ s t a r t ( _ A r g L i s t ,   _ T A r g 2 ) ;   _ R e t u r n P o l i c y ( _ S e c u r e V F u n c N a m e ( _ D s t ,   1 ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ,   _ D s t ) ;   }   _ _ p r a g m a ( w a r n i n g ( p o p ) ) ;   t e m p l a t e   < t y p e n a m e   _ T >   i n l i n e   _ C R T _ I N S E C U R E _ D E P R E C A T E ( _ S e c u r e V F u n c N a m e )   _ R e t u r n T y p e   _ _ C R T D E C L   _ V F u n c N a m e ( _ T   & _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   v a _ l i s t   _ A r g L i s t )   _ C R T _ S E C U R E _ C P P _ N O T H R O W   {   r e t u r n   _ _ i n s e c u r e _ # # _ V F u n c N a m e ( s t a t i c _ c a s t < _ D s t T y p e   * > ( _ D s t ) ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ;   }   t e m p l a t e   < t y p e n a m e   _ T >   i n l i n e   _ C R T _ I N S E C U R E _ D E P R E C A T E ( _ S e c u r e V F u n c N a m e )   _ R e t u r n T y p e   _ _ C R T D E C L   _ V F u n c N a m e ( c o n s t   _ T   & _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   v a _ l i s t   _ A r g L i s t )   _ C R T _ S E C U R E _ C P P _ N O T H R O W   {   r e t u r n   _ _ i n s e c u r e _ # # _ V F u n c N a m e ( s t a t i c _ c a s t < _ D s t T y p e   * > ( _ D s t ) ,   _ T A r g 1 ,   _ T A r g 2 ,   _ A r g L i s t ) ;   }   t e m p l a t e   < >   i n l i n e   _ C R T _ I N S E C U R E _ D E P R E C A T E ( _ S e c u r e V F u n c N a m e )   _ R e t u r n T y p e   _ _ C R T D E C L   _ V F u n c N a m e ( _ D s t T y p e   * & _ D s t ,   _ T T y p e 1   _ T A r g 1 ,   _ T T y p e 2   _ T A r g 2 ,   v a _ l i s t   _ A r g L i s t )   _ C R T _ S E C U R E _ C P P _