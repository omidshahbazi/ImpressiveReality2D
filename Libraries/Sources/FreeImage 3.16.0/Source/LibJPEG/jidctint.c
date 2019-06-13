/*
 * jidctint.c
 *
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modification developed 2002-2013 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a slow-but-accurate integer implementation of the
 * inverse DCT (Discrete Cosine Transform).  In the IJG code, this routine
 * must also perform dequantization of the input coefficients.
 *
 * A 2-D IDCT can be done by 1-D IDCT on each column followed by 1-D IDCT
 * on each row (or vice versa, but it's more convenient to emit a row at
 * a time).  Direct algorithms are also available, but they are much more
 * complex and seem not to be any faster when reduced to code.
 *
 * This implementation is based on an algorithm described in
 *   C. Loeffler, A. Ligtenberg and G. Moschytz, "Practical Fast 1-D DCT
 *   Algorithms with 11 Multiplications", Proc. Int'l. Conf. on Acoustics,
 *   Speech, and Signal Processing 1989 (ICASSP '89), pp. 988-991.
 * The primary algorithm described there uses 11 multiplies and 29 adds.
 * We use their alternate method with 12 multiplies and 32 adds.
 * The advantage of this method is that no data path contains more than one
 * multiplication; this allows a very simple and accurate implementation in
 * scaled fixed-point arithmetic, with a minimal number of shifts.
 *
 * We also provide IDCT routines with various output sample block sizes for
 * direct resolution reduction or enlargement and for direct resolving the
 * common 2x1 and 1x2 subsampling cases without additional resampling: NxN
 * (N=1...16), 2NxN, and Nx2N (N=1...8) pixels for one 8x8 input DCT block.
 *
 * For N<8 we simply take the corresponding low-frequency coefficients of
 * the 8x8 input DCT block and apply an NxN point IDCT on the sub-block
 * to yield the downscaled outputs.
 * This can be seen as direct low-pass downsampling from the DCT domain
 * point of view rather than the usual spatial domain point of view,
 * yielding significant computational savings and results at least
 * as good as common bilinear (averaging) spatial downsampling.
 *
 * For N>8 we apply a partial NxN IDCT on the 8 input coefficients as
 * lower frequencies and higher frequencies assumed to be zero.
 * It turns out that the computational effort is similar to the 8x8 IDCT
 * regarding the output size.
 * Furthermore, the scaling and descaling is the same for all IDCT sizes.
 *
 * CAUTION: We rely on the FIX() macro except for the N=1,2,4,8 cases
 * since there would be too many additional constants to pre-calculate.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "jdct.h"		/* Private declarations for DCT subsystem */

#ifdef DCT_ISLOW_SUPPORTED


/*
 * This module is specialized to the case DCTSIZE = 8.
 */

#if DCTSIZE != 8
  Sorry, this code only copes with 8x8 DCT blocks. /* deliberate syntax err */
#endif


/*
 * The poop on this scaling stuff is as follows:
 *
 * Each 1-D IDCT step produces outputs which are a factor of sqrt(N)
 * larger than the true IDCT outputs.  The final outputs are therefore
 * a factor of N larger than desired; since N=8 this can be cured by
 * a simple right shift at the end of the algorithm.  The advantage of
 * this arrangement is that we save two multiplications per 1-D IDCT,
 * because the y0 and y4 inputs need not be divided by sqrt(N).
 *
 * We have to do addition and subtraction of the integer inputs, which
 * is no problem, and multiplication by fractional constants, which is
 * a problem to do in integer arithmetic.  We multiply all the constants
 * by CONST_SCALE and convert them to integer constants (thus retaining
 * CONST_BITS bits of precision in the constants).  After doing a
 * multiplication we have to divide the product by CONST_SCALE, with proper
 * rounding, to produce the correct output.  This division can be done
 * cheaply as a right shift of CONST_BITS bits.  We postpone shifting
 * as long as possible so that partial sums can be added together with
 * full fractional precision.
 *
 * The outputs of the first pass are scaled up by PASS1_BITS bits so that
 * they are represented to better-than-integral precision.  These outputs
 * require BITS_IN_JSAMPLE + PASS1_BITS + 3 bits; this fits in a 16-bit word
 * with the recommended scaling.  (To scale up 12-bit sample data further, an
 * intermediate INT32 array would be needed.)
 *
 * To avoid overflow of the 32-bit intermediate results in pass 2, we must
 * have BITS_IN_JSAMPLE + CONST_BITS + PASS1_BITS <= 26.  Error analysis
 * shows that the values given below are the most effective.
 */

#if BITS_IN_JSAMPLE == 8
#define CONST_BITS  13
#define PASS1_BITS  2
#else
#define CONST_BITS  13
#define PASS1_BITS  1		/* lose a little precision to avoid overflow */
#endif

/* Some C compilers fail to reduce "FIX(constant)" at compile time, thus
 * causing a lot of useless floating-point operations at run time.
 * To get around this we use the following pre-calculated constants.
 * If you change CONST_BITS you may want to add appropriate values.
 * (With a reasonable C compiler, you can just rely on the FIX() macro...)
 */

#if CONST_BITS == 13
#define FIX_0_298631336  ((INT32)  2446)	/* FIX(0.298631336) */
#define FIX_0_390180644  ((INT32)  3196)	/* FIX(0.390180644) */
#define FIX_0_541196100  ((INT32)  4433)	/* FIX(0.541196100) */
#define FIX_0_765366865  ((INT32)  6270)	/* FIX(0.765366865) */
#define FIX_0_899976223  ((INT32)  7373)	/* FIX(0.899976223) */
#define FIX_1_175875602  ((INT32)  9633)	/* FIX(1.175875602) */
#define FIX_1_501321110  ((INT32)  12299)	/* FIX(1.501321110) */
#define FIX_1_847759065  ((INT32)  15137)	/* FIX(1.847759065) */
#define FIX_1_961570560  ((INT32)  16069)	/* FIX(1.961570560) */
#define FIX_2_053119869  ((INT32)  16819)	/* FIX(2.053119869) */
#define FIX_2_562915447  ((INT32)  20995)	/* FIX(2.562915447) */
#define FIX_3_072711026  ((INT32)  25172)	/* FIX(3.072711026) */
#else
#define FIX_0_298631336  FIX(0.298631336)
#define FIX_0_390180644  FIX(0.390180644)
#define FIX_0_541196100  FIX(0.541196100)
#define FIX_0_765366865  FIX(0.765366865)
#define FIX_0_899976223  FIX(0.899976223)
#define FIX_1_175875602  FIX(1.175875602)
#define FIX_1_501321110  FIX(1.501321110)
#define FIX_1_847759065  FIX(1.847759065)
#define FIX_1_961570560  FIX(1.961570560)
#define FIX_2_053119869  FIX(2.053119869)
#define FIX_2_562915447  FIX(2.562915447)
#define FIX_3_072711026  FIX(3.072711026)
#endif


/* Multiply an INT32 variable by an INT32 constant to yield an INT32 result.
 * For 8-bit samples with the recommended scaling, all the variable
 * and constant values involved are no more than 16 bits wide, so a
 * 16x16->32 bit multiply can be used instead of a full 32x32 multiply.
 * For 12-bit samples, a full 32-bit multiplication will be needed.
 */

#if BITS_IN_JSAMPLE == 8
#define MULTIPLY(var,const)  MULTIPLY16C16(var,const)
#else
#define MULTIPLY(var,const)  ((var) * (const))
#endif


/* Dequantize a coefficient by multiplying it by the multiplier-table
 * entry; produce an int result.  In this module, both inputs and result
 * are 16 bits or less, so either int or short multiply will work.
 */

#define DEQUANTIZE(coef,quantval)  (((ISLOW_MULT_TYPE) (coef)) * (quantval))


/*
 * Perform dequantization and inverse DCT on one block of coefficients.
 *
 * cK represents sqrt(2) * cos(K*pi/16).
 */

GLOBAL(void)
jpeg_idct_islow (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3;
  INT32 tmp10, tmp11, tmp12, tmp13;
  INT32 z1, z2, z3;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[DCTSIZE2];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * Note results are scaled up by sqrt(8) compared to a true IDCT;
   * furthermore, we scale the results by 2**PASS1_BITS.
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = DCTSIZE; ctr > 0; ctr--) {
    /* Due to quantization, we will usually find that many of the input
     * coefficients are zero, especially the AC terms.  We can exploit this
     * by short-circuiting the IDCT calculation for any column in which all
     * the AC terms are zero.  In that case each output is equal to the
     * DC coefficient (with scale factor as needed).
     * With typical images and quantization tables, half or more of the
     * column DCT calculations can be simplified this way.
     */

    if (inptr[DCTSIZE*1] == 0 && inptr[DCTSIZE*2] == 0 &&
	inptr[DCTSIZE*3] == 0 && inptr[DCTSIZE*4] == 0 &&
	inptr[DCTSIZE*5] == 0 && inptr[DCTSIZE*6] == 0 &&
	inptr[DCTSIZE*7] == 0) {
      /* AC terms all zero */
      int dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]) << PASS1_BITS;

      wsptr[DCTSIZE*0] = dcval;
      wsptr[DCTSIZE*1] = dcval;
      wsptr[DCTSIZE*2] = dcval;
      wsptr[DCTSIZE*3] = dcval;
      wsptr[DCTSIZE*4] = dcval;
      wsptr[DCTSIZE*5] = dcval;
      wsptr[DCTSIZE*6] = dcval;
      wsptr[DCTSIZE*7] = dcval;

      inptr++;			/* advance pointers to next column */
      quantptr++;
      wsptr++;
      continue;
    }

    /* Even part: reverse the even part of the forward DCT.
     * The rotator is c(-6).
     */

    z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);       /* c6 */
    tmp2 = z1 + MULTIPLY(z2, FIX_0_765366865);     /* c2-c6 */
    tmp3 = z1 - MULTIPLY(z3, FIX_1_847759065);     /* c2+c6 */

    z2 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z2 <<= CONST_BITS;
    z3 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z2 += ONE << (CONST_BITS-PASS1_BITS-1);

    tmp0 = z2 + z3;
    tmp1 = z2 - z3;

    tmp10 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;
    tmp11 = tmp1 + tmp3;
    tmp12 = tmp1 - tmp3;

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
    tmp1 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    tmp3 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);

    z2 = tmp0 + tmp2;
    z3 = tmp1 + tmp3;

    z1 = MULTIPLY(z2 + z3, FIX_1_175875602);       /*  c3 */
    z2 = MULTIPLY(z2, - FIX_1_961570560);          /* -c3-c5 */
    z3 = MULTIPLY(z3, - FIX_0_390180644);          /* -c3+c5 */
    z2 += z1;
    z3 += z1;

    z1 = MULTIPLY(tmp0 + tmp3, - FIX_0_899976223); /* -c3+c7 */
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);        /* -c1+c3+c5-c7 */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);        /*  c1+c3-c5-c7 */
    tmp0 += z1 + z2;
    tmp3 += z1 + z3;

    z1 = MULTIPLY(tmp1 + tmp2, - FIX_2_562915447); /* -c1-c3 */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);        /*  c1+c3-c5+c7 */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);        /*  c1+c3+c5-c7 */
    tmp1 += z1 + z3;
    tmp2 += z1 + z2;

    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    wsptr[DCTSIZE*0] = (int) RIGHT_SHIFT(tmp10 + tmp3, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*7] = (int) RIGHT_SHIFT(tmp10 - tmp3, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*1] = (int) RIGHT_SHIFT(tmp11 + tmp2, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*6] = (int) RIGHT_SHIFT(tmp11 - tmp2, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*2] = (int) RIGHT_SHIFT(tmp12 + tmp1, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*5] = (int) RIGHT_SHIFT(tmp12 - tmp1, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*3] = (int) RIGHT_SHIFT(tmp13 + tmp0, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*4] = (int) RIGHT_SHIFT(tmp13 - tmp0, CONST_BITS-PASS1_BITS);

    inptr++;			/* advance pointers to next column */
    quantptr++;
    wsptr++;
  }

  /* Pass 2: process rows from work array, store into output array.
   * Note that we must descale the results by a factor of 8 == 2**3,
   * and also undo the PASS1_BITS scaling.
   */

  wsptr = workspace;
  for (ctr = 0; ctr < DCTSIZE; ctr++) {
    outptr = output_buf[ctr] + output_col;
    /* Rows of zeroes can be exploited in the same way as we did with columns.
     * However, the column calculation has created many nonzero AC terms, so
     * the simplification applies less often (typically 5% to 10% of the time).
     * On machines with very fast multiplication, it's possible that the
     * test takes more time than it's worth.  In that case this section
     * may be commented out.
     */

#ifndef NO_ZERO_ROW_TEST
    if (wsptr[1] == 0 && wsptr[2] == 0 && wsptr[3] == 0 && wsptr[4] == 0 &&
	wsptr[5] == 0 && wsptr[6] == 0 && wsptr[7] == 0) {
      /* AC terms all zero */
      JSAMPLE dcval = range_limit[(int) DESCALE((INT32) wsptr[0], PASS1_BITS+3)
				  & RANGE_MASK];

      outptr[0] = dcval;
      outptr[1] = dcval;
      outptr[2] = dcval;
      outptr[3] = dcval;
      outptr[4] = dcval;
      outptr[5] = dcval;
      outptr[6] = dcval;
      outptr[7] = dcval;

      wsptr += DCTSIZE;		/* advance pointer to next row */
      continue;
    }
#endif

    /* Even part: reverse the even part of the forward DCT.
     * The rotator is c(-6).
     */

    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[6];

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);       /* c6 */
    tmp2 = z1 + MULTIPLY(z2, FIX_0_765366865);     /* c2-c6 */
    tmp3 = z1 - MULTIPLY(z3, FIX_1_847759065);     /* c2+c6 */

    /* Add fudge factor here for final descale. */
    z2 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z3 = (INT32) wsptr[4];

    tmp0 = (z2 + z3) << CONST_BITS;
    tmp1 = (z2 - z3) << CONST_BITS;

    tmp10 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;
    tmp11 = tmp1 + tmp3;
    tmp12 = tmp1 - tmp3;

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */

    tmp0 = (INT32) wsptr[7];
    tmp1 = (INT32) wsptr[5];
    tmp2 = (INT32) wsptr[3];
    tmp3 = (INT32) wsptr[1];

    z2 = tmp0 + tmp2;
    z3 = tmp1 + tmp3;

    z1 = MULTIPLY(z2 + z3, FIX_1_175875602);       /*  c3 */
    z2 = MULTIPLY(z2, - FIX_1_961570560);          /* -c3-c5 */
    z3 = MULTIPLY(z3, - FIX_0_390180644);          /* -c3+c5 */
    z2 += z1;
    z3 += z1;

    z1 = MULTIPLY(tmp0 + tmp3, - FIX_0_899976223); /* -c3+c7 */
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);        /* -c1+c3+c5-c7 */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);        /*  c1+c3-c5-c7 */
    tmp0 += z1 + z2;
    tmp3 += z1 + z3;

    z1 = MULTIPLY(tmp1 + tmp2, - FIX_2_562915447); /* -c1-c3 */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);        /*  c1+c3-c5+c7 */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);        /*  c1+c3+c5-c7 */
    tmp1 += z1 + z3;
    tmp2 += z1 + z2;

    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[7] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp11 + tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) RIGHT_SHIFT(tmp11 - tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12 + tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp12 - tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp13 + tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MKA  KA @KA `KA €KA  KA ÀKA àKA  LA  LA @LA `LA €LA  LA ÀLA àLA  MA  MA @MA `MA €MA  MA ÀMA àMA  NA  NA @NA `NA €NA  NA ÀNA àNA  OA  OA @OA `OA €OA  OA ÀOA àOA  PA  PA @PA `PA €PA  PA ÀPA àPA  QA  QA @QA `QA €QA  QA ÀQA àQA  RA  RA @RA `RA €RA  RA ÀRA àRA  SA  SA @SA `SA €SA  SA ÀSA àSA  TA  TA @TA `TA €TA  TA ÀTA àTA  UA  UA @UA `UA €UA  UA ÀUA àUA  VA  VA @VA `VA €VA  VA ÀVA àVA  WA  WA @WA `WA €WA  WA ÀWA àWA  XA  XA @XA `XA €XA  XA ÀXA àXA  YA  YA @YA `YA €YA  YA ÀYA àYA  ZA  ZA @ZA `ZA €ZA  ZA ÀZA àZA  [A  [A @[A `[A €[A  [A À[A à[A  \A  \A @\A `\A €\A  \A À\A à\A  ]A  ]A @]A `]A €]A  ]A À]A à]A  ^A  ^A @^A `^A €^A  ^A À^A à^A  _A  _A @_A `_A €_A  _A À_A à_A  `A  `A @`A ``A €`A  `A À`A à`A  aA  aA @aA `aA €aA  aA ÀaA àaA  bA  bA @bA `bA €bA  bA ÀbA àbA  cA  cA @cA `cA €cA  cA ÀcA àcA  dA  dA @dA `dA €dA  dA ÀdA àdA  eA  eA @eA `eA €eA  eA ÀeA àeA  fA  fA @fA `fA €fA  fA ÀfA àfA  gA  gA @gA `gA €gA  gA ÀgA àgA  hA  hA @hA `hA €hA  hA ÀhA àhA  iA  iA @iA `iA €iA  iA ÀiA àiA  jA  jA @jA `jA €jA  jA ÀjA àjA  kA  kA @kA `kA €kA  kA ÀkA àkA  lA  lA @lA `lA €lA  lA ÀlA àlA  mA  mA @mA `mA €mA  mA ÀmA àmA  nA  nA @nA `nA €nA  nA ÀnA ànA  oA  oA @oA `oA €oA  oA ÀoA àoA  pA  pA @pA `pA €pA  pA ÀpA àpA  qA  qA @qA `qA €qA  qA ÀqA àqA  rA  rA @rA `rA €rA  rA ÀrA àrA  sA  sA @sA `sA €sA  sA ÀsA àsA  tA  tA @tA `tA €tA  tA ÀtA àtA  uA  uA @uA `uA €uA  uA ÀuA àuA  vA  vA @vA `vA €vA  vA ÀvA àvA  wA  wA @wA `wA €wA  wA ÀwA àwA  xA  xA @xA `xA €xA  xA ÀxA àxA  yA  yA @yA `yA €yA  yA ÀyA àyA  zA  zA @zA `zA €zA  zA ÀzA àzA  {A  {A @{A `{A €{A  {A À{A à{A  |A  |A @|A `|A €|A  |A À|A à|A  }A  }A @}A `}A €}A  }A À}A à}A  ~A  ~A @~A `~A €~A  ~A À~A à~A  A  A @A `A €A  A ÀA àA  €A  €A @€A `€A €€A  €A À€A à€A  A  A @A `A €A  A ÀA àA  ‚A  ‚A @‚A `‚A €‚A  ‚A À‚A à‚A  ƒA  ƒA @ƒA `ƒA €ƒA  ƒA ÀƒA àƒA  „A  „A @„A `„A €„A  „A À„A à„A  …A  …A @…A `…A €…A  …A À…A à…A  †A  †A @†A `†A €†A  †A À†A à†A  ‡A  ‡A @‡A `‡A €‡A  ‡A À‡A à‡A  ˆA  ˆA @ˆA `ˆA €ˆA  ˆA ÀˆA àˆA  ‰A  ‰A @‰A `‰A €‰A  ‰A À‰A à‰A  ŠA  ŠA @ŠA `ŠA €ŠA  ŠA ÀŠA àŠA  ‹A  ‹A @‹A `‹A €‹A  ‹A À‹A à‹A  ŒA  ŒA @ŒA `ŒA €ŒA  ŒA ÀŒA àŒA  A  A @A `A €A  A ÀA àA  A  A @A `A €A  A ÀA àA  A  A @A `A €A  A ÀA àA  A  A @A `A €A  A ÀA àA  ‘A  ‘A @‘A `‘A €‘A  ‘A À‘A à‘A  ’A  ’A @’A `’A €’A  ’A À’A à’A  “A  “A @“A `“A €“A  “A À“A à“A  ”A  ”A @”A `”A €”A  ”A À”A à”A  •A  •A @•A `•A €•A  •A À•A à•A  –A  –A @–A `–A €–A  –A À–A à–A  —A  —A @—A `—A €—A  —A À—A à—A  ˜A  ˜A @˜A `˜A €˜A  ˜A À˜A à˜A  ™A  ™A @™A `™A €™A  ™A À™A à™A  šA  šA @šA `šA €šA  šA ÀšA àšA  ›A  ›A @›A `›A €›A  ›A À›A à›A  œA  œA @œA `œA €œA  œA ÀœA àœA  A  A @A `A €A  A ÀA àA  A  A @A `A €A  A ÀA àA  ŸA  ŸA @ŸA `ŸA €ŸA  ŸA ÀŸA àŸA   A   A @ A ` A € A   A À A à A  ¡A  ¡A @¡A `¡A €¡A  ¡A À¡A à¡A  ¢A  ¢A @¢A `¢A €¢A  ¢A À¢A à¢A  £A  £A @£A `£A €£A  £A À£A à£A  ¤A  ¤A @¤A `¤A €¤A  ¤A À¤A à¤A  ¥A  ¥A @¥A `¥A €¥A  ¥A À¥A à¥A  ¦A  ¦A @¦A `¦A €¦A  ¦A À¦A à¦A  §A  §A @§A `§A €§A  §A À§A à§A  ¨A  ¨A @¨A `¨A €¨A  ¨A À¨A à¨A  ©A  ©A @©A `©A €©A  ©A À©A à©A  ªA  ªA @ªA `ªA €ªA  ªA ÀªA àªA  «A  «A @«A `«A €«A  «A À«A à«A  ¬A  ¬A @¬A `¬A €¬A  ¬A À¬A à¬A  ­A  ­A @­A `­A €­A  ­A À­A à­A  ®A  ®A @®A `®A €®A  ®A À®A à®A  ¯A  ¯A @¯A `¯A €¯A  ¯A À¯A à¯A  °A  °A @°A `°A €°A  °A À°A à°A  ±A  ±A @±A `±A €±A  ±A À±A à±A  ²A  ²A @²A `²A €²A  ²A À²A à²A  ³A  ³A @³A `³A €³A  ³A À³A à³A  ´A  ´A @´A `´A €´A  ´A À´A à´A  µA  µA @µA `µA €µA  µA ÀµA àµA  ¶A  ¶A @¶A `¶A €¶A  ¶A À¶A à¶A  ·A  ·A @·A `·A €·A  ·A À·A à·A  ¸A  ¸A @¸A `¸A €¸A  ¸A À¸A à¸A  ¹A  ¹A @¹A `¹A €¹A  ¹A À¹A à¹A  ºA  ºA @ºA `ºA €ºA  ºA ÀºA àºA  »A  »A @»A `»A €»A  »A À»A à»A  ¼A  ¼A @¼A `¼A €¼A  ¼A À¼A à¼A  ½A  ½A @½A `½A €½A  ½A À½A à½A  ¾A  ¾A @¾A `¾A €¾A  ¾A À¾A à¾A  ¿A  ¿A @¿A `¿A €¿A  ¿A À¿A à¿A  ÀA  ÀA @ÀA `ÀA €ÀA  ÀA ÀÀA àÀA  ÁA  ÁA @ÁA `ÁA €ÁA  ÁA ÀÁA àÁA  ÂA  ÂA @ÂA `ÂA €ÂA  ÂA ÀÂA àÂA  ÃA  ÃA @ÃA `ÃA €ÃA  ÃA ÀÃA àÃA  ÄA  ÄA @ÄA `ÄA €ÄA  ÄA ÀÄA àÄA  ÅA  ÅA @ÅA `ÅA €ÅA  ÅA ÀÅA àÅA  ÆA  ÆA @ÆA `ÆA €ÆA  ÆA ÀÆA àÆA  ÇA  ÇA @ÇA `ÇA €ÇA  ÇA ÀÇA àÇA  ÈA  ÈA @ÈA `ÈA €ÈA  ÈA ÀÈA àÈA  ÉA  ÉA @ÉA `ÉA €ÉA  ÉA ÀÉA àÉA  ÊA  ÊA @ÊA `ÊA €ÊA  ÊA ÀÊA àÊA  ËA  ËA @ËA `ËA €ËA  ËA ÀËA àËA  ÌA  ÌA @ÌA `ÌA €ÌA  ÌA ÀÌA àÌA  ÍA  ÍA @ÍA `ÍA €ÍA  ÍA ÀÍA àÍA  ÎA  ÎA @ÎA `ÎA €ÎA  ÎA ÀÎA àÎA  ÏA  ÏA @ÏA `ÏA €ÏA  ÏA ÀÏA àÏA  ĞA  ĞA @ĞA `ĞA €ĞA  ĞA ÀĞA àĞA  ÑA  ÑA @ÑA `ÑA €ÑA  ÑA ÀÑA àÑA  ÒA  ÒA @ÒA `ÒA €ÒA  ÒA ÀÒA àÒA  ÓA  ÓA @ÓA `ÓA €ÓA  ÓA ÀÓA àÓA  ÔA  ÔA @ÔA `ÔA €ÔA  ÔA ÀÔA àÔA  ÕA  ÕA @ÕA `ÕA €ÕA  ÕA ÀÕA àÕA  ÖA  ÖA @ÖA `ÖA €ÖA  ÖA ÀÖA àÖA  ×A  ×A @×A `×A €×A  ×A À×A à×A  ØA  ØA @ØA `ØA €ØA  ØA ÀØA àØA  ÙA  ÙA @ÙA `ÙA €ÙA  ÙA ÀÙA àÙA  ÚA  ÚA @ÚA `ÚA €ÚA  ÚA ÀÚA àÚA  ÛA  ÛA @ÛA `ÛA €ÛA  ÛA ÀÛA àÛA  ÜA  ÜA @ÜA `ÜA €ÜA  ÜA ÀÜA àÜA  İA  İA @İA `İA €İA  İA ÀİA àİA  ŞA  ŞA @ŞA `ŞA €ŞA  ŞA ÀŞA àŞA  ßA  ßA @ßA `ßA €ßA  ßA ÀßA àßA  àA  àA @àA `àA €àA  àA ÀàA ààA  áA  áA @áA `áA €áA  áA ÀáA àáA  âA  âA @âA `âA €âA  âA ÀâA àâA  ãA  ãA @ãA `ãA €ãA  ãA ÀãA àãA  äA  äA @äA `äA €äA  äA ÀäA àäA  åA  åA @åA `åA €åA  åA ÀåA àåA  æA  æA @æA `æA €æA  æA ÀæA àæA  çA  çA @çA `çA €çA  çA ÀçA àçA  èA  èA @èA `èA €èA  èA ÀèA àèA  éA  éA @éA `éA €éA  éA ÀéA àéA  êA  êA @êA `êA €êA  êA ÀêA àêA  ëA  ëA @ëA `ëA €ëA  ëA ÀëA àëA  ìA  ìA @ìA `ìA €ìA  ìA ÀìA àìA  íA  íA @íA `íA €íA  íA ÀíA àíA  îA  îA @îA `îA €îA  îA ÀîA àîA  ïA  ïA @ïA `ïA €ïA  ïA ÀïA àïA  ğA  ğA @ğA `ğA €ğA  ğA ÀğA àğA  ñA  ñA @ñA `ñA €ñA  ñA ÀñA àñA  òA  òA @òA `òA €òA  òA ÀòA àòA  óA  óA @óA `óA €óA  óA ÀóA àóA  ôA  ôA @ôA `ôA €ôA  ôA ÀôA àôA  õA  õA @õA `õA €õA  õA ÀõA àõA  öA  öA @öA `öA €öA  öA ÀöA àöA  ÷A  ÷A @÷A `÷A €÷A  ÷A À÷A à÷A  øA  øA @øA `øA €øA  øA ÀøA àøA  ùA  ùA @ùA `ùA €ùA  ùA ÀùA àùA  úA  úA @úA `úA €úA  úA ÀúA àúA  ûA  ûA @ûA `ûA €ûA  ûA ÀûA àûA  üA  üA @üA `üA €üA  üA ÀüA àüA  ıA  ıA @ıA `ıA €ıA  ıA ÀıA àıA  şA  şA @şA `şA €şA  şA ÀşA àşA  ÿA  ÿA @ÿA `ÿA €ÿA  ÿA ÀÿA àÿA   B   B @ B ` B € B   B À B à B  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  	B  	B @	B `	B €	B  	B À	B à	B  
B  
B @
B `
B €
B  
B À
B à
B  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB   B   B @ B ` B € B   B À B à B  !B  !B @!B `!B €!B  !B À!B à!B  "B  "B @"B `"B €"B  "B À"B à"B  #B  #B @#B `#B €#B  #B À#B à#B  $B  $B @$B `$B €$B  $B À$B à$B  %B  %B @%B `%B €%B  %B À%B à%B  &B  &B @&B `&B €&B  &B À&B à&B  'B  'B @'B `'B €'B  'B À'B à'B  (B  (B @(B `(B €(B  (B À(B à(B  )B  )B @)B `)B €)B  )B À)B à)B  *B  *B @*B `*B €*B  *B À*B à*B  +B  +B @+B `+B €+B  +B À+B à+B  ,B  ,B @,B `,B €,B  ,B À,B à,B  -B  -B @-B `-B €-B  -B À-B à-B  .B  .B @.B `.B €.B  .B À.B à.B  /B  /B @/B `/B €/B  /B À/B à/B  0B  0B @0B `0B €0B  0B À0B à0B  1B  1B @1B `1B €1B  1B À1B à1B  2B  2B @2B `2B €2B  2B À2B à2B  3B  3B @3B `3B €3B  3B À3B à3B  4B  4B @4B `4B €4B  4B À4B à4B  5B  5B @5B `5B €5B  5B À5B à5B  6B  6B @6B `6B €6B  6B À6B à6B  7B  7B @7B `7B €7B  7B À7B à7B  8B  8B @8B `8B €8B  8B À8B à8B  9B  9B @9B `9B €9B  9B À9B à9B  :B  :B @:B `:B €:B  :B À:B à:B  ;B  ;B @;B `;B €;B  ;B À;B à;B  <B  <B @<B `<B €<B  <B À<B à<B  =B  =B @=B `=B €=B  =B À=B à=B  >B  >B @>B `>B €>B  >B À>B à>B  ?B  ?B @?B `?B €?B  ?B À?B à?B  @B  @B @@B `@B €@B  @B À@B à@B  AB  AB @AB `AB €AB  AB ÀAB àAB  BB  BB @BB `BB €BB  BB ÀBB àBB  CB  CB @CB `CB €CB  CB ÀCB àCB  DB  DB @DB `DB €DB  DB ÀDB àDB  EB  EB @EB `EB €EB  EB ÀEB àEB  FB  FB @FB `FB €FB  FB ÀFB àFB  GB  GB @GB `GB €GB  GB ÀGB àGB  HB  HB @HB `HB €HB  HB ÀHB àHB  IB  IB @IB `IB €IB  IB ÀIB àIB  JB  JB @JB `JB €JB  JB ÀJB àJB  KB  KB @KB `KB €KB  KB ÀKB àKB  LB  LB @LB `LB €LB  LB ÀLB àLB  MB  MB @MB `MB €MB  MB ÀMB àMB  NB  NB @NB `NB €NB  NB ÀNB àNB  OB  OB @OB `OB €OB  OB ÀOB àOB  PB  PB @PB `PB €PB  PB ÀPB àPB  QB  QB @QB `QB €QB  QB ÀQB àQB  RB  RB @RB `RB €RB  RB ÀRB àRB  SB  SB @SB `SB €SB  SB ÀSB àSB  TB  TB @TB `TB €TB  TB ÀTB àTB  UB  UB @UB `UB €UB  UB ÀUB àUB  VB  VB @VB `VB €VB  VB ÀVB àVB  WB  WB @WB `WB €WB  WB ÀWB àWB  XB  XB @XB `XB €XB  XB ÀXB àXB  YB  YB @YB `YB €YB  YB ÀYB àYB  ZB  ZB @ZB `ZB €ZB  ZB ÀZB àZB  [B  [B @[B `[B €[B  [B À[B à[B  \B  \B @\B `\B €\B  \B À\B à\B  ]B  ]B @]B `]B €]B  ]B À]B à]B  ^B  ^B @^B `^B €^B  ^B À^B à^B  _B  _B @_B `_B €_B  _B À_B à_B  `B  `B @`B ``B €`B  `B À`B à`B  aB  aB @aB `aB €aB  aB ÀaB àaB  bB  bB @bB `bB €bB  bB ÀbB àbB  cB  cB @cB `cB €cB  cB ÀcB àcB  dB  dB @dB `dB €dB  dB ÀdB àdB  eB  eB @eB `eB €eB  eB ÀeB àeB  fB  fB @fB `fB €fB  fB ÀfB àfB  gB  gB @gB `gB €gB  gB ÀgB àgB  hB  hB @hB `hB €hB  hB ÀhB àhB  iB  iB @iB `iB €iB  iB ÀiB àiB  jB  jB @jB `jB €jB  jB ÀjB àjB  kB  kB @kB `kB €kB  kB ÀkB àkB  lB  lB @lB `lB €lB  lB ÀlB àlB  mB  mB @mB `mB €mB  mB ÀmB àmB  nB  nB @nB `nB €nB  nB ÀnB ànB  oB  oB @oB `oB €oB  oB ÀoB àoB  pB  pB @pB `pB €pB  pB ÀpB àpB  qB  qB @qB `qB €qB  qB ÀqB àqB  rB  rB @rB `rB €rB  rB ÀrB àrB  sB  sB @sB `sB €sB  sB ÀsB àsB  tB  tB @tB `tB €tB  tB ÀtB àtB  uB  uB @uB `uB €uB  uB ÀuB àuB  vB  vB @vB `vB €vB  vB ÀvB àvB  wB  wB @wB `wB €wB  wB ÀwB àwB  xB  xB @xB `xB €xB  xB ÀxB àxB  yB  yB @yB `yB €yB  yB ÀyB àyB  zB  zB @zB `zB €zB  zB ÀzB àzB  {B  {B @{B `{B €{B  {B À{B à{B  |B  |B @|B `|B €|B  |B À|B à|B  }B  }B @}B `}B €}B  }B À}B à}B  ~B  ~B @~B `~B €~B  ~B À~B à~B  B  B @B `B €B  B ÀB àB  €B  €B @€B `€B €€B  €B À€B à€B  B  B @B `B €B  B ÀB àB  ‚B  ‚B @‚B `‚B €‚B  ‚B À‚B à‚B  ƒB  ƒB @ƒB `ƒB €ƒB  ƒB ÀƒB àƒB  „B  „B @„B `„B €„B  „B À„B à„B  …B  …B @…B `…B €…B  …B À…B à…B  †B  †B @†B `†B €†B  †B À†B à†B  ‡B  ‡B @‡B `‡B €‡B  ‡B À‡B à‡B  ˆB  ˆB @ˆB `ˆB €ˆB  ˆB ÀˆB àˆB  ‰B  ‰B @‰B `‰B €‰B  ‰B À‰B à‰B  ŠB  ŠB @ŠB `ŠB €ŠB  ŠB ÀŠB àŠB  ‹B  ‹B @‹B `‹B €‹B  ‹B À‹B à‹B  ŒB  ŒB @ŒB `ŒB €ŒB  ŒB ÀŒB àŒB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  ‘B  ‘B @‘B `‘B €‘B  ‘B À‘B à‘B  ’B  ’B @’B `’B €’B  ’B À’B à’B  “B  “B @“B `“B €“B  “B À“B à“B  ”B  ”B @”B `”B €”B  ”B À”B à”B  •B  •B @•B `•B €•B  •B À•B à•B  –B  –B @–B `–B €–B  –B À–B à–B  —B  —B @—B `—B €—B  —B À—B à—B  ˜B  ˜B @˜B `˜B €˜B  ˜B À˜B à˜B  ™B  ™B @™B `™B €™B  ™B À™B à™B  šB  šB @šB `šB €šB  šB ÀšB àšB  ›B  ›B @›B `›B €›B  ›B À›B à›B  œB  œB @œB `œB €œB  œB ÀœB àœB  B  B @B `B €B  B ÀB àB  B  B @B `B €B  B ÀB àB  ŸB  ŸB @ŸB `ŸB €ŸB  ŸB ÀŸB àŸB   B   B @ B ` B € B   B À B à B  ¡B  ¡B @¡B `¡B €¡B  ¡B À¡B à¡B  ¢B  ¢B @¢B `¢B €¢B  ¢B À¢B à¢B  £B  £B @£B `£B €£B  £B À£B à£B  ¤B  ¤B @¤B `¤B €¤B  ¤B À¤B à¤B  ¥B  ¥B @¥B `¥B €¥B  ¥B À¥B à¥B  ¦B  ¦B @¦B `¦B €¦B  ¦B À¦B à¦B  §B  §B @§B `§B €§B  §B À§B à§B  ¨B  ¨B @¨B `¨B €¨B  ¨B À¨B à¨B  ©B  ©B @©B `©B €©B  ©B À©B à©B  ªB  ªB @ªB `ªB €ªB  ªB ÀªB àªB  «B  «B @«B `«B €«B  «B À«B à«B  ¬B  ¬B @¬B `¬B €¬B  ¬B À¬B à¬B  ­B  ­B @­B `­B €­B  ­B À­B à­B  ®B  ®B @®B `®B €®B  ®B À®B à®B  ¯B  ¯B @¯B `¯B €¯B  ¯B À¯B à¯B  °B  °B @°B `°B €°B  °B À°B à°B  ±B  ±B @±B `±B €±B  ±B À±B à±B  ²B  ²B @²B `²B €²B  ²B À²B à²B  ³B  ³B @³B `³B €³B  ³B À³B à³B  ´B  ´B @´B `´B €´B  ´B À´B à´B  µB  µB @µB `µB €µB  µB ÀµB àµB  ¶B  ¶B @¶B `¶B €¶B  ¶B À¶B à¶B  ·B  ·B @·B `·B €·B  ·B À·B à·B  ¸B  ¸B @¸B `¸B €¸B  ¸B À¸B à¸B  ¹B  ¹B @¹B `¹B €¹B  ¹B À¹B à¹B  ºB  ºB @ºB `ºB €ºB  ºB ÀºB àºB  »B  »B @»B `»B €»B  »B À»B à»B  ¼B  ¼B @¼B `¼B €¼B  ¼B À¼B à¼B  ½B  ½B @½B `½B €½B  ½B À½B à½B  ¾B  ¾B @¾B `¾B €¾B  ¾B À¾B à¾B  ¿B  ¿B @¿B `¿B €¿B  ¿B À¿B à¿B  ÀB  ÀB @ÀB `ÀB €ÀB  ÀB ÀÀB àÀB  ÁB  ÁB @ÁB `ÁB €ÁB  ÁB ÀÁB àÁB  ÂB  ÂB @ÂB `ÂB €ÂB  ÂB ÀÂB àÂB  ÃB  ÃB @ÃB `ÃB €ÃB  ÃB ÀÃB àÃB  ÄB  ÄB @ÄB `ÄB €ÄB  ÄB ÀÄB àÄB  ÅB  ÅB @ÅB `ÅB €ÅB  ÅB ÀÅB àÅB  ÆB  ÆB @ÆB `ÆB €ÆB  ÆB ÀÆB àÆB  ÇB  ÇB @ÇB `ÇB €ÇB  ÇB ÀÇB àÇB  ÈB  ÈB @ÈB `ÈB €ÈB  ÈB ÀÈB àÈB  ÉB  ÉB @ÉB `ÉB €ÉB  ÉB ÀÉB àÉB  ÊB  ÊB @ÊB `ÊB €ÊB  ÊB ÀÊB àÊB  ËB  ËB @ËB `ËB €ËB  ËB ÀËB àËB  ÌB  ÌB @ÌB `ÌB €ÌB  ÌB ÀÌB àÌB  ÍB  ÍB @ÍB `ÍB €ÍB  ÍB ÀÍB àÍB  ÎB  ÎB @ÎB `ÎB €ÎB  ÎB ÀÎB àÎB  ÏB  ÏB @ÏB `ÏB €ÏB  ÏB ÀÏB àÏB  ĞB  ĞB @ĞB `ĞB €ĞB  ĞB ÀĞB àĞB  ÑB  ÑB @ÑB `ÑB €ÑB  ÑB ÀÑB àÑB  ÒB  ÒB @ÒB `ÒB €ÒB  ÒB ÀÒB àÒB  ÓB  ÓB @ÓB `ÓB €ÓB  ÓB ÀÓB àÓB  ÔB  ÔB @ÔB `ÔB €ÔB  ÔB ÀÔB àÔB  ÕB  ÕB @ÕB `ÕB €ÕB  ÕB ÀÕB àÕB  ÖB  ÖB @ÖB `ÖB €ÖB  ÖB ÀÖB àÖB  ×B  ×B @×B `×B €×B  ×B À×B à×B  ØB  ØB @ØB `ØB €ØB  ØB ÀØB àØB  ÙB  ÙB @ÙB `ÙB €ÙB  ÙB ÀÙB àÙB  ÚB  ÚB @ÚB `ÚB €ÚB  ÚB ÀÚB àÚB  ÛB  ÛB @ÛB `ÛB €ÛB  ÛB ÀÛB àÛB  ÜB  ÜB @ÜB `ÜB €ÜB  ÜB ÀÜB àÜB  İB  İB @İB `İB €İB  İB ÀİB àİB  ŞB  ŞB @ŞB `ŞB €ŞB  ŞB ÀŞB àŞB  ßB  ßB @ßB `ßB €ßB  ßB ÀßB àßB  àB  àB @àB `àB €àB  àB ÀàB ààB  áB  áB @áB `áB €áB  áB ÀáB àáB  âB  âB @âB `âB €âB  âB ÀâB àâB  ãB  ãB @ãB `ãB €ãB  ãB ÀãB àãB  äB  äB @äB `äB €äB  äB ÀäB àäB  åB  åB @åB `åB €åB  åB ÀåB àåB  æB  æB @æB `æB €æB  æB ÀæB àæB  çB  çB @çB `çB €çB  çB ÀçB àçB  èB  èB @èB `èB €èB  èB ÀèB àèB  éB  éB @éB `éB €éB  éB ÀéB àéB  êB  êB @êB `êB €êB  êB ÀêB àêB  ëB  ëB @ëB `ëB €ëB  ëB ÀëB àëB  ìB  ìB @ìB `ìB €ìB  ìB ÀìB àìB  íB  íB @íB `íB €íB  íB ÀíB àíB  îB  îB @îB `îB €îB  îB ÀîB àîB  ïB  ïB @ïB `ïB €ïB  ïB ÀïB àïB  ğB  ğB @ğB `ğB €ğB  ğB ÀğB àğB  ñB  ñB @ñB `ñB €ñB  ñB ÀñB àñB  òB  òB @òB `òB €òB  òB ÀòB àòB  óB  óB @óB `óB €óB  óB ÀóB àóB  ôB  ôB @ôB `ôB €ôB  ôB ÀôB àôB  õB  õB @õB `õB €õB  õB ÀõB àõB  öB  öB @öB `öB €öB  öB ÀöB àöB  ÷B  ÷B @÷B `÷B €÷B  ÷B À÷B à÷B  øB  øB @øB `øB €øB  øB ÀøB àøB  ùB  ùB @ùB `ùB €ùB  ùB ÀùB àùB  úB  úB @úB `úB €úB  úB ÀúB àúB  ûB  ûB @ûB `ûB €ûB  ûB ÀûB àûB  üB  üB @üB `üB €üB  üB ÀüB àüB  ıB  ıB @ıB `ıB €ıB  ıB ÀıB àıB  şB  şB @şB `şB €şB  şB ÀşB àşB  ÿB  ÿB @ÿB `ÿB €ÿB  ÿB ÀÿB àÿB   C   C @ C ` C € C   C À C à C  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  	C  	C @	C `	C €	C  	C À	C à	C  
C  
C @
C `
C €
C  
C À
C à
C  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC   C   C @ C ` C € C   C À C à C  !C  !C @!C `!C €!C  !C À!C à!C  "C  "C @"C `"C €"C  "C À"C à"C  #C  #C @#C `#C €#C  #C À#C à#C  $C  $C @$C `$C €$C  $C À$C à$C  %C  %C @%C `%C €%C  %C À%C à%C  &C  &C @&C `&C €&C  &C À&C à&C  'C  'C @'C `'C €'C  'C À'C à'C  (C  (C @(C `(C €(C  (C À(C à(C  )C  )C @)C `)C €)C  )C À)C à)C  *C  *C @*C `*C €*C  *C À*C à*C  +C  +C @+C `+C €+C  +C À+C à+C  ,C  ,C @,C `,C €,C  ,C À,C à,C  -C  -C @-C `-C €-C  -C À-C à-C  .C  .C @.C `.C €.C  .C À.C à.C  /C  /C @/C `/C €/C  /C À/C à/C  0C  0C @0C `0C €0C  0C À0C à0C  1C  1C @1C `1C €1C  1C À1C à1C  2C  2C @2C `2C €2C  2C À2C à2C  3C  3C @3C `3C €3C  3C À3C à3C  4C  4C @4C `4C €4C  4C À4C à4C  5C  5C @5C `5C €5C  5C À5C à5C  6C  6C @6C `6C €6C  6C À6C à6C  7C  7C @7C `7C €7C  7C À7C à7C  8C  8C @8C `8C €8C  8C À8C à8C  9C  9C @9C `9C €9C  9C À9C à9C  :C  :C @:C `:C €:C  :C À:C à:C  ;C  ;C @;C `;C €;C  ;C À;C à;C  <C  <C @<C `<C €<C  <C À<C à<C  =C  =C @=C `=C €=C  =C À=C à=C  >C  >C @>C `>C €>C  >C À>C à>C  ?C  ?C @?C `?C €?C  ?C À?C à?C  @C  @C @@C `@C €@C  @C À@C à@C  AC  AC @AC `AC €AC  AC ÀAC àAC  BC  BC @BC `BC €BC  BC ÀBC àBC  CC  CC @CC `CC €CC  CC ÀCC àCC  DC  DC @DC `DC €DC  DC ÀDC àDC  EC  EC @EC `EC €EC  EC ÀEC àEC  FC  FC @FC `FC €FC  FC ÀFC àFC  GC  GC @GC `GC €GC  GC ÀGC àGC  HC  HC @HC `HC €HC  HC ÀHC àHC  IC  IC @IC `IC €IC  IC ÀIC àIC  JC  JC @JC `JC €JC  JC ÀJC àJC  KC  KC @KC `KC €KC  KC ÀKC àKC  LC  LC @LC `LC €LC  LC ÀLC àLC  MC  MC @MC `MC €MC  MC ÀMC àMC  NC  NC @NC `NC €NC  NC ÀNC àNC  OC  OC @OC `OC €OC  OC ÀOC àOC  PC  PC @PC `PC €PC  PC ÀPC àPC  QC  QC @QC `QC €QC  QC ÀQC àQC  RC  RC @RC `RC €RC  RC ÀRC àRC  SC  SC @SC `SC €SC  SC ÀSC àSC  TC  TC @TC `TC €TC  TC ÀTC àTC  UC  UC @UC `UC €UC  UC ÀUC àUC  VC  VC @VC `VC €VC  VC ÀVC àVC  WC  WC @WC `WC €WC  WC ÀWC àWC  XC  XC @XC `XC €XC  XC ÀXC àXC  YC  YC @YC `YC €YC  YC ÀYC àYC  ZC  ZC @ZC `ZC €ZC  ZC ÀZC àZC  [C  [C @[C `[C €[C  [C À[C à[C  \C  \C @\C `\C €\C  \C À\C à\C  ]C  ]C @]C `]C €]C  ]C À]C à]C  ^C  ^C @^C `^C €^C  ^C À^C à^C  _C  _C @_C `_C €_C  _C À_C à_C  `C  `C @`C ``C €`C  `C À`C à`C  aC  aC @aC `aC €aC  aC ÀaC àaC  bC  bC @bC `bC €bC  bC ÀbC àbC  cC  cC @cC `cC €cC  cC ÀcC àcC  dC  dC @dC `dC €dC  dC ÀdC àdC  eC  eC @eC `eC €eC  eC ÀeC àeC  fC  fC @fC `fC €fC  fC ÀfC àfC  gC  gC @gC `gC €gC  gC ÀgC àgC  hC  hC @hC `hC €hC  hC ÀhC àhC  iC  iC @iC `iC €iC  iC ÀiC àiC  jC  jC @jC `jC €jC  jC ÀjC àjC  kC  kC @kC `kC €kC  kC ÀkC àkC  lC  lC @lC `lC €lC  lC ÀlC àlC  mC  mC @mC `mC €mC  mC ÀmC àmC  nC  nC @nC `nC €nC  nC ÀnC ànC  oC  oC @oC `oC €oC  oC ÀoC àoC  pC  pC @pC `pC €pC  pC ÀpC àpC  qC  qC @qC `qC €qC  qC ÀqC àqC  rC  rC @rC `rC €rC  rC ÀrC àrC  sC  sC @sC `sC €sC  sC ÀsC àsC  tC  tC @tC `tC €tC  tC ÀtC àtC  uC  uC @uC `uC €uC  uC ÀuC àuC  vC  vC @vC `vC €vC  vC ÀvC àvC  wC  wC @wC `wC €wC  wC ÀwC àwC  xC  xC @xC `xC €xC  xC ÀxC àxC  yC  yC @yC `yC €yC  yC ÀyC àyC  zC  zC @zC `zC €zC  zC ÀzC àzC  {C  {C @{C `{C €{C  {C À{C à{C  |C  |C @|C `|C €|C  |C À|C à|C  }C  }C @}C `}C €}C  }C À}C à}C  ~C  ~C @~C `~C €~C  ~C À~C à~C  C  C @C `C €C  C ÀC àC  €C  €C @€C `€C €€C  €C À€C à€C  C  C @C `C €C  C ÀC àC  ‚C  ‚C @‚C `‚C €‚C  ‚C À‚C à‚C  ƒC  ƒC @ƒC `ƒC €ƒC  ƒC ÀƒC àƒC  „C  „C @„C `„C €„C  „C À„C à„C  …C  …C @…C `…C €…C  …C À…C à…C  †C  †C @†C `†C €†C  †C À†C à†C  ‡C  ‡C @‡C `‡C €‡C  ‡C À‡C à‡C  ˆC  ˆC @ˆC `ˆC €ˆC  ˆC ÀˆC àˆC  ‰C  ‰C @‰C `‰C €‰C  ‰C À‰C à‰C  ŠC  ŠC @ŠC `ŠC €ŠC  ŠC ÀŠC àŠC  ‹C  ‹C @‹C `‹C €‹C  ‹C À‹C à‹C  ŒC  ŒC @ŒC `ŒC €ŒC  ŒC ÀŒC àŒC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  ‘C  ‘C @‘C `‘C €‘C  ‘C À‘C à‘C  ’C  ’C @’C `’C €’C  ’C À’C à’C  “C  “C @“C `“C €“C  “C À“C à“C  ”C  ”C @”C `”C €”C  ”C À”C à”C  •C  •C @•C `•C €•C  •C À•C à•C  –C  –C @–C `–C €–C  –C À–C à–C  —C  —C @—C `—C €—C  —C À—C à—C  ˜C  ˜C @˜C `˜C €˜C  ˜C À˜C à˜C  ™C  ™C @™C `™C €™C  ™C À™C à™C  šC  šC @šC `šC €šC  šC ÀšC àšC  ›C  ›C @›C `›C €›C  ›C À›C à›C  œC  œC @œC `œC €œC  œC ÀœC àœC  C  C @C `C €C  C ÀC àC  C  C @C `C €C  C ÀC àC  ŸC  ŸC @ŸC `ŸC €ŸC  ŸC ÀŸC àŸC   C   C @ C ` C € C   C À C à C  ¡C  ¡C @¡C `¡C €¡C  ¡C À¡C à¡C  ¢C  ¢C @¢C `¢C €¢C  ¢C À¢C à¢C  £C  £C @£C `£C €£C  £C À£C à£C  ¤C  ¤C @¤C `¤C €¤C  ¤C À¤C à¤C  ¥C  ¥C @¥C `¥C €¥C  ¥C À¥C à¥C  ¦C  ¦C @¦C `¦C €¦C  ¦C À¦C à¦C  §C  §C @§C `§C €§C  §C À§C à§C  ¨C  ¨C @¨C `¨C €¨C  ¨C À¨C à¨C  ©C  ©C @©C `©C €©C  ©C À©C à©C  ªC  ªC @ªC `ªC €ªC  ªC ÀªC àªC  «C  «C @«C `«C €«C  «C À«C à«C  ¬C  ¬C @¬C `¬C €¬C  ¬C À¬C à¬C  ­C  ­C @­C `­C €­C  ­C À­C à­C  ®C  ®C @®C `®C €®C  ®C À®C à®C  ¯C  ¯C @¯C `¯C €¯C  ¯C À¯C à¯C  °C  °C @°C `°C €°C  °C À°C à°C  ±C  ±C @±C `±C €±C  ±C À±C à±C  ²C  ²C @²C `²C €²C  ²C À²C à²C  ³C  ³C @³C `³C €³C  ³C À³C à³C  ´C  ´C @´C `´C €´C  ´C À´C à´C  µC  µC @µC `µC €µC  µC ÀµC àµC  ¶C  ¶C @¶C `¶C €¶C  ¶C À¶C à¶C  ·C  ·C @·C `·C €·C  ·C À·C à·C  ¸C  ¸C @¸C `¸C €¸C  ¸C À¸C à¸C  ¹C  ¹C @¹C `¹C €¹C  ¹C À¹C à¹C  ºC  ºC @ºC `ºC €ºC  ºC ÀºC àºC  »C  »C @»C `»C €»C  »C À»C à»C  ¼C  ¼C @¼C `¼C €¼C  ¼C À¼C à¼C  ½C  ½C @½C `½C €½C  ½C À½C à½C  ¾C  ¾C @¾C `¾C €¾C  ¾C À¾C à¾C  ¿C  ¿C @¿C `¿C €¿C  ¿C À¿C à¿C  ÀC  ÀC @ÀC `ÀC €ÀC  ÀC ÀÀC àÀC  ÁC  ÁC @ÁC `ÁC €ÁC  ÁC ÀÁC àÁC  ÂC  ÂC @ÂC `ÂC €ÂC  ÂC ÀÂC àÂC  ÃC  ÃC @ÃC `ÃC €ÃC  ÃC ÀÃC àÃC  ÄC  ÄC @ÄC `ÄC €ÄC  ÄC ÀÄC àÄC  ÅC  ÅC @ÅC `ÅC €ÅC  ÅC ÀÅC àÅC  ÆC  ÆC @ÆC `ÆC €ÆC  ÆC ÀÆC àÆC  ÇC  ÇC @ÇC `ÇC €ÇC  ÇC ÀÇC àÇC  ÈC  ÈC @ÈC `ÈC €ÈC  ÈC ÀÈC àÈC  ÉC  ÉC @ÉC `ÉC €ÉC  ÉC ÀÉC àÉC  ÊC  ÊC @ÊC `ÊC €ÊC  ÊC ÀÊC àÊC  cale. */
  tmp4 += ONE << 2;

  tmp0 = tmp4 + tmp5;
  tmp2 = tmp4 - tmp5;

  /* Column 1 */
  tmp4 = DEQUANTIZE(coef_block[DCTSIZE*0+1], quantptr[DCTSIZE*0+1]);
  tmp5 = DEQUANTIZE(coef_block[DCTSIZE*1+1], quantptr[DCTSIZE*1+1]);

  tmp1 = tmp4 + tmp5;
  tmp3 = tmp4 - tmp5;

  /* Pass 2: process 2 rows, store into output array. */

  /* Row 0 */
  outptr = output_buf[0] + output_col;

  outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp0 + tmp1, 3) & RANGE_MASK];
  outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp0 - tmp1, 3) & RANGE_MASK];

  /* Row 1 */
  outptr = output_buf[1] + output_col;

  outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp2 + tmp3, 3) & RANGE_MASK];
  outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp2 - tmp3, 3) & RANGE_MASK];
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a reduced-size 1x1 output block.
 *
 * We hardly need an inverse DCT routine for this: just take the
 * average pixel value, which is one-eighth of the DC coefficient.
 */

GLOBAL(void)
jpeg_idct_1x1 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  int dcval;
  ISLOW_MULT_TYPE * quantptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  SHIFT_TEMPS

  /* 1x1 is trivial: just take the DC coefficient divided by 8. */
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  dcval = DEQUANTIZE(coef_block[0], quantptr[0]);
  dcval = (int) DESCALE((INT32) dcval, 3);

  output_buf[0][output_col] = range_limit[dcval & RANGE_MASK];
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 9x9 output block.
 *
 * Optimized algorithm with 10 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/18).
 */

GLOBAL(void)
jpeg_idct_9x9 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3, tmp10, tmp11, tmp12, tmp13, tmp14;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*9];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp0 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp0 += ONE << (CONST_BITS-PASS1_BITS-1);

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    tmp3 = MULTIPLY(z3, FIX(0.707106781));      /* c6 */
    tmp1 = tmp0 + tmp3;
    tmp2 = tmp0 - tmp3 - tmp3;

    tmp0 = MULTIPLY(z1 - z2, FIX(0.707106781)); /* c6 */
    tmp11 = tmp2 + tmp0;
    tmp14 = tmp2 - tmp0 - tmp0;

    tmp0 = MULTIPLY(z1 + z2, FIX(1.328926049)); /* c2 */
    tmp2 = MULTIPLY(z1, FIX(1.083350441));      /* c4 */
    tmp3 = MULTIPLY(z2, FIX(0.245575608));      /* c8 */

    tmp10 = tmp1 + tmp0 - tmp3;
    tmp12 = tmp1 - tmp0 + tmp2;
    tmp13 = tmp1 - tmp2 + tmp3;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    z2 = MULTIPLY(z2, - FIX(1.224744871));           /* -c3 */

    tmp2 = MULTIPLY(z1 + z3, FIX(0.909038955));      /* c5 */
    tmp3 = MULTIPLY(z1 + z4, FIX(0.483689525));      /* c7 */
    tmp0 = tmp2 + tmp3 - z2;
    tmp1 = MULTIPLY(z3 - z4, FIX(1.392728481));      /* c1 */
    tmp2 += z2 - tmp1;
    tmp3 += z2 + tmp1;
    tmp1 = MULTIPLY(z1 - z3 - z4, FIX(1.224744871)); /* c3 */

    /* Final output stage */

    wsptr[8*0] = (int) RIGHT_SHIFT(tmp10 + tmp0, CONST_BITS-PASS1_BITS);
    wsptr[8*8] = (int) RIGHT_SHIFT(tmp10 - tmp0, CONST_BITS-PASS1_BITS);
    wsptr[8*1] = (int) RIGHT_SHIFT(tmp11 + tmp1, CONST_BITS-PASS1_BITS);
    wsptr[8*7] = (int) RIGHT_SHIFT(tmp11 - tmp1, CONST_BITS-PASS1_BITS);
    wsptr[8*2] = (int) RIGHT_SHIFT(tmp12 + tmp2, CONST_BITS-PASS1_BITS);
    wsptr[8*6] = (int) RIGHT_SHIFT(tmp12 - tmp2, CONST_BITS-PASS1_BITS);
    wsptr[8*3] = (int) RIGHT_SHIFT(tmp13 + tmp3, CONST_BITS-PASS1_BITS);
    wsptr[8*5] = (int) RIGHT_SHIFT(tmp13 - tmp3, CONST_BITS-PASS1_BITS);
    wsptr[8*4] = (int) RIGHT_SHIFT(tmp14, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 9 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 9; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp0 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp0 <<= CONST_BITS;

    z1 = (INT32) wsptr[2];
    z2 = (INT32) wsptr[4];
    z3 = (INT32) wsptr[6];

    tmp3 = MULTIPLY(z3, FIX(0.707106781));      /* c6 */
    tmp1 = tmp0 + tmp3;
    tmp2 = tmp0 - tmp3 - tmp3;

    tmp0 = MULTIPLY(z1 - z2, FIX(0.707106781)); /* c6 */
    tmp11 = tmp2 + tmp0;
    tmp14 = tmp2 - tmp0 - tmp0;

    tmp0 = MULTIPLY(z1 + z2, FIX(1.328926049)); /* c2 */
    tmp2 = MULTIPLY(z1, FIX(1.083350441));      /* c4 */
    tmp3 = MULTIPLY(z2, FIX(0.245575608));      /* c8 */

    tmp10 = tmp1 + tmp0 - tmp3;
    tmp12 = tmp1 - tmp0 + tmp2;
    tmp13 = tmp1 - tmp2 + tmp3;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];

    z2 = MULTIPLY(z2, - FIX(1.224744871));           /* -c3 */

    tmp2 = MULTIPLY(z1 + z3, FIX(0.909038955));      /* c5 */
    tmp3 = MULTIPLY(z1 + z4, FIX(0.483689525));      /* c7 */
    tmp0 = tmp2 + tmp3 - z2;
    tmp1 = MULTIPLY(z3 - z4, FIX(1.392728481));      /* c1 */
    tmp2 += z2 - tmp1;
    tmp3 += z2 + tmp1;
    tmp1 = MULTIPLY(z1 - z3 - z4, FIX(1.224744871)); /* c3 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[8] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp11 + tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[7] = range_limit[(int) RIGHT_SHIFT(tmp11 - tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12 + tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) RIGHT_SHIFT(tmp12 - tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp13 + tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp13 - tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp14,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 10x10 output block.
 *
 * Optimized algorithm with 12 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/20).
 */

GLOBAL(void)
jpeg_idct_10x10 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24;
  INT32 z1, z2, z3, z4, z5;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*10];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z3 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z3 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z3 += ONE << (CONST_BITS-PASS1_BITS-1);
    z4 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z1 = MULTIPLY(z4, FIX(1.144122806));         /* c4 */
    z2 = MULTIPLY(z4, FIX(0.437016024));         /* c8 */
    tmp10 = z3 + z1;
    tmp11 = z3 - z2;

    tmp22 = RIGHT_SHIFT(z3 - ((z1 - z2) << 1),   /* c0 = (c4-c8)*2 */
			CONST_BITS-PASS1_BITS);

    z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    z1 = MULTIPLY(z2 + z3, FIX(0.831253876));    /* c6 */
    tmp12 = z1 + MULTIPLY(z2, FIX(0.513743148)); /* c2-c6 */
    tmp13 = z1 - MULTIPLY(z3, FIX(2.176250899)); /* c2+c6 */

    tmp20 = tmp10 + tmp12;
    tmp24 = tmp10 - tmp12;
    tmp21 = tmp11 + tmp13;
    tmp23 = tmp11 - tmp13;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = z2 + z4;
    tmp13 = z2 - z4;

    tmp12 = MULTIPLY(tmp13, FIX(0.309016994));        /* (c3-c7)/2 */
    z5 = z3 << CONST_BITS;

    z2 = MULTIPLY(tmp11, FIX(0.951056516));           /* (c3+c7)/2 */
    z4 = z5 + tmp12;

    tmp10 = MULTIPLY(z1, FIX(1.396802247)) + z2 + z4; /* c1 */
    tmp14 = MULTIPLY(z1, FIX(0.221231742)) - z2 + z4; /* c9 */

    z2 = MULTIPLY(tmp11, FIX(0.587785252));           /* (c1-c9)/2 */
    z4 = z5 - tmp12 - (tmp13 << (CONST_BITS - 1));

    tmp12 = (z1 - tmp13 - z3) << PASS1_BITS;

    tmp11 = MULTIPLY(z1, FIX(1.260073511)) - z2 - z4; /* c3 */
    tmp13 = MULTIPLY(z1, FIX(0.642039522)) - z2 + z4; /* c7 */

    /* Final output stage */

    wsptr[8*0] = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*9] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1] = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*8] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*2] = (int) (tmp22 + tmp12);
    wsptr[8*7] = (int) (tmp22 - tmp12);
    wsptr[8*3] = (int) RIGHT_SHIFT(tmp23 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*6] = (int) RIGHT_SHIFT(tmp23 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*4] = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*5] = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 10 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 10; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z3 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z3 <<= CONST_BITS;
    z4 = (INT32) wsptr[4];
    z1 = MULTIPLY(z4, FIX(1.144122806));         /* c4 */
    z2 = MULTIPLY(z4, FIX(0.437016024));         /* c8 */
    tmp10 = z3 + z1;
    tmp11 = z3 - z2;

    tmp22 = z3 - ((z1 - z2) << 1);               /* c0 = (c4-c8)*2 */

    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[6];

    z1 = MULTIPLY(z2 + z3, FIX(0.831253876));    /* c6 */
    tmp12 = z1 + MULTIPLY(z2, FIX(0.513743148)); /* c2-c6 */
    tmp13 = z1 - MULTIPLY(z3, FIX(2.176250899)); /* c2+c6 */

    tmp20 = tmp10 + tmp12;
    tmp24 = tmp10 - tmp12;
    tmp21 = tmp11 + tmp13;
    tmp23 = tmp11 - tmp13;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z3 <<= CONST_BITS;
    z4 = (INT32) wsptr[7];

    tmp11 = z2 + z4;
    tmp13 = z2 - z4;

    tmp12 = MULTIPLY(tmp13, FIX(0.309016994));        /* (c3-c7)/2 */

    z2 = MULTIPLY(tmp11, FIX(0.951056516));           /* (c3+c7)/2 */
    z4 = z3 + tmp12;

    tmp10 = MULTIPLY(z1, FIX(1.396802247)) + z2 + z4; /* c1 */
    tmp14 = MULTIPLY(z1, FIX(0.221231742)) - z2 + z4; /* c9 */

    z2 = MULTIPLY(tmp11, FIX(0.587785252));           /* (c1-c9)/2 */
    z4 = z3 - tmp12 - (tmp13 << (CONST_BITS - 1));

    tmp12 = ((z1 - tmp13) << CONST_BITS) - z3;

    tmp11 = MULTIPLY(z1, FIX(1.260073511)) - z2 - z4; /* c3 */
    tmp13 = MULTIPLY(z1, FIX(0.642039522)) - z2 + z4; /* c7 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[9] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[8] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[7] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 11x11 output block.
 *
 * Optimized algorithm with 24 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/22).
 */

GLOBAL(void)
jpeg_idct_11x11 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*11];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp10 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp10 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp10 += ONE << (CONST_BITS-PASS1_BITS-1);

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    tmp20 = MULTIPLY(z2 - z3, FIX(2.546640132));     /* c2+c4 */
    tmp23 = MULTIPLY(z2 - z1, FIX(0.430815045));     /* c2-c6 */
    z4 = z1 + z3;
    tmp24 = MULTIPLY(z4, - FIX(1.155664402));        /* -(c2-c10) */
    z4 -= z2;
    tmp25 = tmp10 + MULTIPLY(z4, FIX(1.356927976));  /* c2 */
    tmp21 = tmp20 + tmp23 + tmp25 -
	    MULTIPLY(z2, FIX(1.821790775));          /* c2+c4+c10-c6 */
    tmp20 += tmp25 + MULTIPLY(z3, FIX(2.115825087)); /* c4+c6 */
    tmp23 += tmp25 - MULTIPLY(z1, FIX(1.513598477)); /* c6+c8 */
    tmp24 += tmp25;
    tmp22 = tmp24 - MULTIPLY(z3, FIX(0.788749120));  /* c8+c10 */
    tmp24 += MULTIPLY(z2, FIX(1.944413522)) -        /* c2+c8 */
	     MULTIPLY(z1, FIX(1.390975730));         /* c4+c10 */
    tmp25 = tmp10 - MULTIPLY(z4, FIX(1.414213562));  /* c0 */

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = z1 + z2;
    tmp14 = MULTIPLY(tmp11 + z3 + z4, FIX(0.398430003)); /* c9 */
    tmp11 = MULTIPLY(tmp11, FIX(0.887983902));           /* c3-c9 */
    tmp12 = MULTIPLY(z1 + z3, FIX(0.670361295));         /* c5-c9 */
    tmp13 = tmp14 + MULTIPLY(z1 + z4, FIX(0.366151574)); /* c7-c9 */
    tmp10 = tmp11 + tmp12 + tmp13 -
	    MULTIPLY(z1, FIX(0.923107866));              /* c7+c5+c3-c1-2*c9 */
    z1    = tmp14 - MULTIPLY(z2 + z3, FIX(1.163011579)); /* c7+c9 */
    tmp11 += z1 + MULTIPLY(z2, FIX(2.073276588));        /* c1+c7+3*c9-c3 */
    tmp12 += z1 - MULTIPLY(z3, FIX(1.192193623));        /* c3+c5-c7-c9 */
    z1    = MULTIPLY(z2 + z4, - FIX(1.798248910));       /* -(c1+c9) */
    tmp11 += z1;
    tmp13 += z1 + MULTIPLY(z4, FIX(2.102458632));        /* c1+c5+c9-c7 */
    tmp14 += MULTIPLY(z2, - FIX(1.467221301)) +          /* -(c5+c9) */
	     MULTIPLY(z3, FIX(1.001388905)) -            /* c1-c9 */
	     MULTIPLY(z4, FIX(1.684843907));             /* c3+c9 */

    /* Final output stage */

    wsptr[8*0]  = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*10] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1]  = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*9]  = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*2]  = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*8]  = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*3]  = (int) RIGHT_SHIFT(tmp23 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*7]  = (int) RIGHT_SHIFT(tmp23 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*4]  = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*6]  = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*5]  = (int) RIGHT_SHIFT(tmp25, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 11 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 11; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp10 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp10 <<= CONST_BITS;

    z1 = (INT32) wsptr[2];
    z2 = (INT32) wsptr[4];
    z3 = (INT32) wsptr[6];

    tmp20 = MULTIPLY(z2 - z3, FIX(2.546640132));     /* c2+c4 */
    tmp23 = MULTIPLY(z2 - z1, FIX(0.430815045));     /* c2-c6 */
    z4 = z1 + z3;
    tmp24 = MULTIPLY(z4, - FIX(1.155664402));        /* -(c2-c10) */
    z4 -= z2;
    tmp25 = tmp10 + MULTIPLY(z4, FIX(1.356927976));  /* c2 */
    tmp21 = tmp20 + tmp23 + tmp25 -
	    MULTIPLY(z2, FIX(1.821790775));          /* c2+c4+c10-c6 */
    tmp20 += tmp25 + MULTIPLY(z3, FIX(2.115825087)); /* c4+c6 */
    tmp23 += tmp25 - MULTIPLY(z1, FIX(1.513598477)); /* c6+c8 */
    tmp24 += tmp25;
    tmp22 = tmp24 - MULTIPLY(z3, FIX(0.788749120));  /* c8+c10 */
    tmp24 += MULTIPLY(z2, FIX(1.944413522)) -        /* c2+c8 */
	     MULTIPLY(z1, FIX(1.390975730));         /* c4+c10 */
    tmp25 = tmp10 - MULTIPLY(z4, FIX(1.414213562));  /* c0 */

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];

    tmp11 = z1 + z2;
    tmp14 = MULTIPLY(tmp11 + z3 + z4, FIX(0.398430003)); /* c9 */
    tmp11 = MULTIPLY(tmp11, FIX(0.887983902));           /* c3-c9 */
    tmp12 = MULTIPLY(z1 + z3, FIX(0.670361295));         /* c5-c9 */
    tmp13 = tmp14 + MULTIPLY(z1 + z4, FIX(0.366151574)); /* c7-c9 */
    tmp10 = tmp11 + tmp12 + tmp13 -
	    MULTIPLY(z1, FIX(0.923107866));              /* c7+c5+c3-c1-2*c9 */
    z1    = tmp14 - MULTIPLY(z2 + z3, FIX(1.163011579)); /* c7+c9 */
    tmp11 += z1 + MULTIPLY(z2, FIX(2.073276588));        /* c1+c7+3*c9-c3 */
    tmp12 += z1 - MULTIPLY(z3, FIX(1.192193623));        /* c3+c5-c7-c9 */
    z1    = MULTIPLY(z2 + z4, - FIX(1.798248910));       /* -(c1+c9) */
    tmp11 += z1;
    tmp13 += z1 + MULTIPLY(z4, FIX(2.102458632));        /* c1+c5+c9-c7 */
    tmp14 += MULTIPLY(z2, - FIX(1.467221301)) +          /* -(c5+c9) */
	     MULTIPLY(z3, FIX(1.001388905)) -            /* c1-c9 */
	     MULTIPLY(z4, FIX(1.684843907));             /* c3+c9 */

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 12x12 output block.
 *
 * Optimized algorithm with 15 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/24).
 */

GLOBAL(void)
jpeg_idct_12x12 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*12];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z3 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z3 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z3 += ONE << (CONST_BITS-PASS1_BITS-1);

    z4 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z4 = MULTIPLY(z4, FIX(1.224744871)); /* c4 */

    tmp10 = z3 + z4;
    tmp11 = z3 - z4;

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z4 = MULTIPLY(z1, FIX(1.366025404)); /* c2 */
    z1 <<= CONST_BITS;
    z2 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);
    z2 <<= CONST_BITS;

    tmp12 = z1 - z2;

    tmp21 = z3 + tmp12;
    tmp24 = z3 - tmp12;

    tmp12 = z4 + z2;

    tmp20 = tmp10 + tmp12;
    tmp25 = tmp10 - tmp12;

    tmp12 = z4 - z1 - z2;

    tmp22 = tmp11 + tmp12;
    tmp23 = tmp11 - tmp12;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = MULTIPLY(z2, FIX(1.306562965));                  /* c3 */
    tmp14 = MULTIPLY(z2, - FIX_0_541196100);                 /* -c9 */

    tmp10 = z1 + z3;
    tmp15 = MULTIPLY(tmp10 + z4, FIX(0.860918669));          /* c7 */
    tmp12 = tmp15 + MULTIPLY(tmp10, FIX(0.261052384));       /* c5-c7 */
    tmp10 = tmp12 + tmp11 + MULTIPLY(z1, FIX(0.280143716));  /* c1-c5 */
    tmp13 = MULTIPLY(z3 + z4, - FIX(1.045510580));           /* -(c7+c11) */
    tmp12 += tmp13 + tmp14 - MULTIPLY(z3, FIX(1.478575242)); /* c1+c5-c7-c11 */
    tmp13 += tmp15 - tmp11 + MULTIPLY(z4, FIX(1.586706681)); /* c1+c11 */
    tmp15 += tmp14 - MULTIPLY(z1, FIX(0.676326758)) -        /* c7-c11 */
	     MULTIPLY(z4, FIX(1.982889723));                 /* c5+c7 */

    z1 -= z4;
    z2 -= z3;
    z3 = MULTIPLY(z1 + z2, FIX_0_541196100);                 /* c9 */
    tmp11 = z3 + MULTIPLY(z1, FIX_0_765366865);              /* c3-c9 */
    tmp14 = z3 - MULTIPLY(z2, FIX_1_847759065);              /* c3+c9 */

    /* Final output stage */

    wsptr[8*0]  = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*11] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1]  = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*10] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*2]  = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*9]  = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*3]  = (int) RIGHT_SHIFT(tmp23 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*8]  = (int) RIGHT_SHIFT(tmp23 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*4]  = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*7]  = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*5]  = (int) RIGHT_SHIFT(tmp25 + tmp15, CONST_BITS-PASS1_BITS);
    wsptr[8*6]  = (int) RIGHT_SHIFT(tmp25 - tmp15, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 12 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 12; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z3 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z3 <<= CONST_BITS;

    z4 = (INT32) wsptr[4];
    z4 = MULTIPLY(z4, FIX(1.224744871)); /* c4 */

    tmp10 = z3 + z4;
    tmp11 = z3 - z4;

    z1 = (INT32) wsptr[2];
    z4 = MULTIPLY(z1, FIX(1.366025404)); /* c2 */
    z1 <<= CONST_BITS;
    z2 = (INT32) wsptr[6];
    z2 <<= CONST_BITS;

    tmp12 = z1 - z2;

    tmp21 = z3 + tmp12;
    tmp24 = z3 - tmp12;

    tmp12 = z4 + z2;

    tmp20 = tmp10 + tmp12;
    tmp25 = tmp10 - tmp12;

    tmp12 = z4 - z1 - z2;

    tmp22 = tmp11 + tmp12;
    tmp23 = tmp11 - tmp12;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];

    tmp11 = MULTIPLY(z2, FIX(1.306562965));                  /* c3 */
    tmp14 = MULTIPLY(z2, - FIX_0_541196100);                 /* -c9 */

    tmp10 = z1 + z3;
    tmp15 = MULTIPLY(tmp10 + z4, FIX(0.860918669));          /* c7 */
    tmp12 = tmp15 + MULTIPLY(tmp10, FIX(0.261052384));       /* c5-c7 */
    tmp10 = tmp12 + tmp11 + MULTIPLY(z1, FIX(0.280143716));  /* c1-c5 */
    tmp13 = MULTIPLY(z3 + z4, - FIX(1.045510580));           /* -(c7+c11) */
    tmp12 += tmp13 + tmp14 - MULTIPLY(z3, FIX(1.478575242)); /* c1+c5-c7-c11 */
    tmp13 += tmp15 - tmp11 + MULTIPLY(z4, FIX(1.586706681)); /* c1+c11 */
    tmp15 += tmp14 - MULTIPLY(z1, FIX(0.676326758)) -        /* c7-c11 */
	     MULTIPLY(z4, FIX(1.982889723));                 /* c5+c7 */

    z1 -= z4;
    z2 -= z3;
    z3 = MULTIPLY(z1 + z2, FIX_0_541196100);                 /* c9 */
    tmp11 = z3 + MULTIPLY(z1, FIX_0_765366865);              /* c3-c9 */
    tmp14 = z3 - MULTIPLY(z2, FIX_1_847759065);              /* c3+c9 */

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 13x13 output block.
 *
 * Optimized algorithm with 29 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/26).
 */

GLOBAL(void)
jpeg_idct_13x13 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*13];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z1 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS-PASS1_BITS-1);

    z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    tmp10 = z3 + z4;
    tmp11 = z3 - z4;

    tmp12 = MULTIPLY(tmp10, FIX(1.155388986));                /* (c4+c6)/2 */
    tmp13 = MULTIPLY(tmp11, FIX(0.096834934)) + z1;           /* (c4-c6)/2 */

    tmp20 = MULTIPLY(z2, FIX(1.373119086)) + tmp12 + tmp13;   /* c2 */
    tmp22 = MULTIPLY(z2, FIX(0.501487041)) - tmp12 + tmp13;   /* c10 */

    tmp12 = MULTIPLY(tmp10, FIX(0.316450131));                /* (c8-c12)/2 */
    tmp13 = MULTIPLY(tmp11, FIX(0.486914739)) + z1;           /* (c8+c12)/2 */

    tmp21 = MULTIPLY(z2, FIX(1.058554052)) - tmp12 + tmp13;   /* c6 */
    tmp25 = MULTIPLY(z2, - FIX(1.252223920)) + tmp12 + tmp13; /* c4 */

    tmp12 = MULTIPLY(tmp10, FIX(0.435816023));                /* (c2-c10)/2 */
    tmp13 = MULTIPLY(tmp11, FIX(0.937303064)) - z1;           /* (c2+c10)/2 */

    tmp23 = MULTIPLY(z2, - FIX(0.170464608)) - tmp12 - tmp13; /* c12 */
    tmp24 = MULTIPLY(z2, - FIX(0.803364869)) + tmp12 - tmp13; /* c8 */

    tmp26 = MULTIPLY(tmp11 - z2, FIX(1.414213562)) + z1;      /* c0 */

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = MULTIPLY(z1 + z2, FIX(1.322312651));     /* c3 */
    tmp12 = MULTIPLY(z1 + z3, FIX(1.163874945));     /* c5 */
    tmp15 = z1 + z4;
    tmp13 = MULTIPLY(tmp15, FIX(0.937797057));       /* c7 */
    tmp10 = tmp11 + tmp12 + tmp13 -
	    MULTIPLY(z1, FIX(2.020082300));          /* c7+c5+c3-c1 */
    tmp14 = MULTIPLY(z2 + z3, - FIX(0.338443458));   /* -c11 */
    tmp11 += tmp14 + MULTIPLY(z2, FIX(0.837223564)); /* c5+c9+c11-c3 */
    tmp12 += tmp14 - MULTIPLY(z3, FIX(1.572116027)); /* c1+c5-c9-c11 */
    tmp14 = MULTIPLY(z2 + z4, - FIX(1.163874945));   /* -c5 */
    tmp11 += tmp14;
    tmp13 += tmp14 + MULTIPLY(z4, FIX(2.205608352)); /* c3+c5+c9-c7 */
    tmp14 = MULTIPLY(z3 + z4, - FIX(0.657217813));   /* -c9 */
    tmp12 += tmp14;
    tmp13 += tmp14;
    tmp15 = MULTIPLY(tmp15, FIX(0.338443458));       /* c11 */
    tmp14 = tmp15 + MULTIPLY(z1, FIX(0.318774355)) - /* c9-c11 */
	    MULTIPLY(z2, FIX(0.466105296));          /* c1-c7 */
    z1    = MULTIPLY(z3 - z2, FIX(0.937797057));     /* c7 */
    tmp14 += z1;
    tmp15 += z1 + MULTIPLY(z3, FIX(0.384515595)) -   /* c3-c7 */
	     MULTIPLY(z4, FIX(1.742345811));         /* c1+c11 */

    /* Final output stage */

    wsptr[8*0]  = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*12] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1]  = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*11] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*2]  = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*10] = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*3]  = (int) RIGHT_SHIFT(tmp23 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*9]  = (int) RIGHT_SHIFT(tmp23 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*4]  = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*8]  = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*5]  = (int) RIGHT_SHIFT(tmp25 + tmp15, CONST_BITS-PASS1_BITS);
    wsptr[8*7]  = (int) RIGHT_SHIFT(tmp25 - tmp15, CONST_BITS-PASS1_BITS);
    wsptr[8*6]  = (int) RIGHT_SHIFT(tmp26, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 13 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 13; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z1 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z1 <<= CONST_BITS;

    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[4];
    z4 = (INT32) wsptr[6];

    tmp10 = z3 + z4;
    tmp11 = z3 - z4;

    tmp12 = MULTIPLY(tmp10, FIX(1.155388986));                /* (c4+c6)/2 */
    tmp13 = MULTIPLY(tmp11, FIX(0.096834934)) + z1;           /* (c4-c6)/2 */

    tmp20 = MULTIPLY(z2, FIX(1.373119086)) + tmp12 + tmp13;   /* c2 */
    tmp22 = MULTIPLY(z2, FIX(0.501487041)) - tmp12 + tmp13;   /* c10 */

    tmp12 = MULTIPLY(tmp10, FIX(0.316450131));                /* (c8-c12)/2 */
    tmp13 = MULTIPLY(tmp11, FIX(0.486914739)) + z1;           /* (c8+c12)/2 */

    tmp21 = MULTIPLY(z2, FIX(1.058554052)) - tmp12 + tmp13;   /* c6 */
    tmp25 = MULTIPLY(z2, - FIX(1.252223920)) + tmp12 + tmp13; /* c4 */

    tmp12 = MULTIPLY(tmp10, FIX(0.435816023));                /* (c2-c10)/2 */
    tmp13 = MULTIPLY(tmp11, FIX(0.937303064)) - z1;           /* (c2+c10)/2 */

    tmp23 = MULTIPLY(z2, - FIX(0.170464608)) - tmp12 - tmp13; /* c12 */
    tmp24 = MULTIPLY(z2, - FIX(0.803364869)) + tmp12 - tmp13; /* c8 */

    tmp26 = MULTIPLY(tmp11 - z2, FIX(1.414213562)) + z1;      /* c0 */

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];

    tmp11 = MULTIPLY(z1 + z2, FIX(1.322312651));     /* c3 */
    tmp12 = MULTIPLY(z1 + z3, FIX(1.163874945));     /* c5 */
    tmp15 = z1 + z4;
    tmp13 = MULTIPLY(tmp15, FIX(0.937797057));       /* c7 */
    tmp10 = tmp11 + tmp12 + tmp13 -
	    MULTIPLY(z1, FIX(2.020082300));          /* c7+c5+c3-c1 */
    tmp14 = MULTIPLY(z2 + z3, - FIX(0.338443458));   /* -c11 */
    tmp11 += tmp14 + MULTIPLY(z2, FIX(0.837223564)); /* c5+c9+c11-c3 */
    tmp12 += tmp14 - MULTIPLY(z3, FIX(1.572116027)); /* c1+c5-c9-c11 */
    tmp14 = MULTIPLY(z2 + z4, - FIX(1.163874945));   /* -c5 */
    tmp11 += tmp14;
    tmp13 += tmp14 + MULTIPLY(z4, FIX(2.205608352)); /* c3+c5+c9-c7 */
    tmp14 = MULTIPLY(z3 + z4, - FIX(0.657217813));   /* -c9 */
    tmp12 += tmp14;
    tmp13 += tmp14;
    tmp15 = MULTIPLY(tmp15, FIX(0.338443458));       /* c11 */
    tmp14 = tmp15 + MULTIPLY(z1, FIX(0.318774355)) - /* c9-c11 */
	    MULTIPLY(z2, FIX(0.466105296));          /* c1-c7 */
    z1    = MULTIPLY(z3 - z2, FIX(0.937797057));     /* c7 */
    tmp14 += z1;
    tmp15 += z1 + MULTIPLY(z3, FIX(0.384515595)) -   /* c3-c7 */
	     MULTIPLY(z4, FIX(1.742345811));         /* c1+c11 */

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[12] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp26,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 14x14 output block.
 *
 * Optimized algorithm with 20 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/28).
 */

GLOBAL(void)
jpeg_idct_14x14 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*14];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z1 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS-PASS1_BITS-1);
    z4 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z2 = MULTIPLY(z4, FIX(1.274162392));         /* c4 */
    z3 = MULTIPLY(z4, FIX(0.314692123));         /* c12 */
    z4 = MULTIPLY(z4, FIX(0.881747734));         /* c8 */

    tmp10 = z1 + z2;
    tmp11 = z1 + z3;
    tmp12 = z1 - z4;

    tmp23 = RIGHT_SHIFT(z1 - ((z2 + z3 - z4) << 1), /* c0 = (c4+c12-c8)*2 */
			CONST_BITS-PASS1_BITS);

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    z3 = MULTIPLY(z1 + z2, FIX(1.105676686));    /* c6 */

    tmp13 = z3 + MULTIPLY(z1, FIX(0.273079590)); /* c2-c6 */
    tmp14 = z3 - MULTIPLY(z2, FIX(1.719280954)); /* c6+c10 */
    tmp15 = MULTIPLY(z1, FIX(0.613604268)) -     /* c10 */
	    MULTIPLY(z2, FIX(1.378756276));      /* c2 */

    tmp20 = tmp10 + tmp13;
    tmp26 = tmp10 - tmp13;
    tmp21 = tmp11 + tmp14;
    tmp25 = tmp11 - tmp14;
    tmp22 = tmp12 + tmp15;
    tmp24 = tmp12 - tmp15;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
    tmp13 = z4 << CONST_BITS;

    tmp14 = z1 + z3;
    tmp11 = MULTIPLY(z1 + z2, FIX(1.334852607));           /* c3 */
    tmp12 = MULTIPLY(tmp14, FIX(1.197448846));             /* c5 */
    tmp10 = tmp11 + tmp12 + tmp13 - MULTIPLY(z1, FIX(1.126980169)); /* c3+c5-c1 */
    tmp14 = MULTIPLY(tmp14, FIX(0.752406978));             /* c9 */
    tmp16 = tmp14 - MULTIPLY(z1, FIX(1.061150426));        /* c9+c11-c13 */
    z1    -= z2;
    tmp15 = MULTIPLY(z1, FIX(0.467085129)) - tmp13;        /* c11 */
    tmp16 += tmp15;
    z1    += z4;
    z4    = MULTIPLY(z2 + z3, - FIX(0.158341681)) - tmp13; /* -c13 */
    tmp11 += z4 - MULTIPLY(z2, FIX(0.424103948));          /* c3-c9-c13 */
    tmp12 += z4 - MULTIPLY(z3, FIX(2.373959773));          /* c3+c5-c13 */
    z4    = MULTIPLY(z3 - z2, FIX(1.405321284));           /* c1 */
    tmp14 += z4 + tmp13 - MULTIPLY(z3, FIX(1.6906431334)); /* c1+c9-c11 */
    tmp15 += z4 + MULTIPLY(z2, FIX(0.674957567));          /* c1+c11-c5 */

    tmp13 = (z1 - z3) << PASS1_BITS;

    /* Final output stage */

    wsptr[8*0]  = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*13] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1]  = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*12] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*2]  = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*11] = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*3]  = (int) (tmp23 + tmp13);
    wsptr[8*10] = (int) (tmp23 - tmp13);
    wsptr[8*4]  = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*9]  = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*5]  = (int) RIGHT_SHIFT(tmp25 + tmp15, CONST_BITS-PASS1_BITS);
    wsptr[8*8]  = (int) RIGHT_SHIFT(tmp25 - tmp15, CONST_BITS-PASS1_BITS);
    wsptr[8*6]  = (int) RIGHT_SHIFT(tmp26 + tmp16, CONST_BITS-PASS1_BITS);
    wsptr[8*7]  = (int) RIGHT_SHIFT(tmp26 - tmp16, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 14 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 14; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z1 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z1 <<= CONST_BITS;
    z4 = (INT32) wsptr[4];
    z2 = MULTIPLY(z4, FIX(1.274162392));         /* c4 */
    z3 = MULTIPLY(z4, FIX(0.314692123));         /* c12 */
    z4 = MULTIPLY(z4, FIX(0.881747734));         /* c8 */

    tmp10 = z1 + z2;
    tmp11 = z1 + z3;
    tmp12 = z1 - z4;

    tmp23 = z1 - ((z2 + z3 - z4) << 1);          /* c0 = (c4+c12-c8)*2 */

    z1 = (INT32) wsptr[2];
    z2 = (INT32) wsptr[6];

    z3 = MULTIPLY(z1 + z2, FIX(1.105676686));    /* c6 */

    tmp13 = z3 + MULTIPLY(z1, FIX(0.273079590)); /* c2-c6 */
    tmp14 = z3 - MULTIPLY(z2, FIX(1.719280954)); /* c6+c10 */
    tmp15 = MULTIPLY(z1, FIX(0.613604268)) -     /* c10 */
	    MULTIPLY(z2, FIX(1.378756276));      /* c2 */

    tmp20 = tmp10 + tmp13;
    tmp26 = tmp10 - tmp13;
    tmp21 = tmp11 + tmp14;
    tmp25 = tmp11 - tmp14;
    tmp22 = tmp12 + tmp15;
    tmp24 = tmp12 - tmp15;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];
    z4 <<= CONST_BITS;

    tmp14 = z1 + z3;
    tmp11 = MULTIPLY(z1 + z2, FIX(1.334852607));           /* c3 */
    tmp12 = MULTIPLY(tmp14, FIX(1.197448846));             /* c5 */
    tmp10 = tmp11 + tmp12 + z4 - MULTIPLY(z1, FIX(1.126980169)); /* c3+c5-c1 */
    tmp14 = MULTIPLY(tmp14, FIX(0.752406978));             /* c9 */
    tmp16 = tmp14 - MULTIPLY(z1, FIX(1.061150426));        /* c9+c11-c13 */
    z1    -= z2;
    tmp15 = MULTIPLY(z1, FIX(0.467085129)) - z4;           /* c11 */
    tmp16 += tmp15;
    tmp13 = MULTIPLY(z2 + z3, - FIX(0.158341681)) - z4;    /* -c13 */
    tmp11 += tmp13 - MULTIPLY(z2, FIX(0.424103948));       /* c3-c9-c13 */
    tmp12 += tmp13 - MULTIPLY(z3, FIX(2.373959773));       /* c3+c5-c13 */
    tmp13 = MULTIPLY(z3 - z2, FIX(1.405321284));           /* c1 */
    tmp14 += tmp13 + z4 - MULTIPLY(z3, FIX(1.6906431334)); /* c1+c9-c11 */
    tmp15 += tmp13 + MULTIPLY(z2, FIX(0.674957567));       /* c1+c11-c5 */

    tmp13 = ((z1 - z3) << CONST_BITS) + z4;

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[13] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[12] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp26 + tmp16,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp26 - tmp16,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 15x15 output block.
 *
 * Optimized algorithm with 22 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/30).
 */

GLOBAL(void)
jpeg_idct_15x15 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*15];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z1 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS-PASS1_BITS-1);

    z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    tmp10 = MULTIPLY(z4, FIX(0.437016024)); /* c12 */
    tmp11 = MULTIPLY(z4, FIX(1.144122806)); /* c6 */

    tmp12 = z1 - tmp10;
    tmp13 = z1 + tmp11;
    z1 -= (tmp11 - tmp10) << 1;             /* c0 = (c6-c12)*2 */

    z4 = z2 - z3;
    z3 += z2;
    tmp10 = MULTIPLY(z3, FIX(1.337628990)); /* (c2+c4)/2 */
    tmp11 = MULTIPLY(z4, FIX(0.045680613)); /* (c2-c4)/2 */
    z2 = MULTIPLY(z2, FIX(1.439773946));    /* c4+c14 */

    tmp20 = tmp13 + tmp10 + tmp11;
    tmp23 = tmp12 - tmp10 + tmp11 + z2;

    tmp10 = MULTIPLY(z3, FIX(0.547059574)); /* (c8+c14)/2 */
    tmp11 = MULTIPLY(z4, FIX(0.399234004)); /* (c8-c14)/2 */

    tmp25 = tmp13 - tmp10 - tmp11;
    tmp26 = tmp12 + tmp10 - tmp11 - z2;

    tmp10 = MULTIPLY(z3, FIX(0.790569415)); /* (c6+c12)/2 */
    tmp11 = MULTIPLY(z4, FIX(0.353553391)); /* (c6-c12)/2 */

    tmp21 = tmp12 + tmp10 + tmp11;
    tmp24 = tmp13 - tmp10 + tmp11;
    tmp11 += tmp11;
    tmp22 = z1 + tmp11;                     /* c10 = c6-c12 */
    tmp27 = z1 - tmp11 - tmp11;             /* c0 = (c6-c12)*2 */

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z3 = MULTIPLY(z4, FIX(1.224744871));                    /* c5 */
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp13 = z2 - z4;
    tmp15 = MULTIPLY(z1 + tmp13, FIX(0.831253876));         /* c9 */
    tmp11 = tmp15 + MULTIPLY(z1, FIX(0.513743148));         /* c3-c9 */
    tmp14 = tmp15 - MULTIPLY(tmp13, FIX(2.176250899));      /* c3+c9 */

    tmp13 = MULTIPLY(z2, - FIX(0.831253876));               /* -c9 */
    tmp15 = MULTIPLY(z2, - FIX(1.344997024));               /* -c3 */
    z2 = z1 - z4;
    tmp12 = z3 + MULTIPLY(z2, FIX(1.406466353));            /* c1 */

    tmp10 = tmp12 + MULTIPLY(z4, FIX(2.457431844)) - tmp15; /* c1+c7 */
    tmp16 = tmp12 - MULTIPLY(z1, FIX(1.112434820)) + tmp13; /* c1-c13 */
    tmp12 = MULTIPLY(z2, FIX(1.224744871)) - z3;            /* c5 */
    z2 = MULTIPLY(z1 + z4, FIX(0.575212477));               /* c11 */
    tmp13 += z2 + MULTIPLY(z1, FIX(0.475753014)) - z3;      /* c7-c11 */
    tmp15 += z2 - MULTIPLY(z4, FIX(0.869244010)) + z3;      /* c11+c13 */

    /* Final output stage */

    wsptr[8*0]  = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*14] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1]  = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*13] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*2]  = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*12] = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*3]  = (int) RIGHT_SHIFT(tmp23 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*11] = (int) RIGHT_SHIFT(tmp23 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*4]  = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*10] = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*5]  = (int) RIGHT_SHIFT(tmp25 + tmp15, CONST_BITS-PASS1_BITS);
    wsptr[8*9]  = (int) RIGHT_SHIFT(tmp25 - tmp15, CONST_BITS-PASS1_BITS);
    wsptr[8*6]  = (int) RIGHT_SHIFT(tmp26 + tmp16, CONST_BITS-PASS1_BITS);
    wsptr[8*8]  = (int) RIGHT_SHIFT(tmp26 - tmp16, CONST_BITS-PASS1_BITS);
    wsptr[8*7]  = (int) RIGHT_SHIFT(tmp27, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 15 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 15; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z1 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z1 <<= CONST_BITS;

    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[4];
    z4 = (INT32) wsptr[6];

    tmp10 = MULTIPLY(z4, FIX(0.437016024)); /* c12 */
    tmp11 = MULTIPLY(z4, FIX(1.144122806)); /* c6 */

    tmp12 = z1 - tmp10;
    tmp13 = z1 + tmp11;
    z1 -= (tmp11 - tmp10) << 1;             /* c0 = (c6-c12)*2 */

    z4 = z2 - z3;
    z3 += z2;
    tmp10 = MULTIPLY(z3, FIX(1.337628990)); /* (c2+c4)/2 */
    tmp11 = MULTIPLY(z4, FIX(0.045680613)); /* (c2-c4)/2 */
    z2 = MULTIPLY(z2, FIX(1.439773946));    /* c4+c14 */

    tmp20 = tmp13 + tmp10 + tmp11;
    tmp23 = tmp12 - tmp10 + tmp11 + z2;

    tmp10 = MULTIPLY(z3, FIX(0.547059574)); /* (c8+c14)/2 */
    tmp11 = MULTIPLY(z4, FIX(0.399234004)); /* (c8-c14)/2 */

    tmp25 = tmp13 - tmp10 - tmp11;
    tmp26 = tmp12 + tmp10 - tmp11 - z2;

    tmp10 = MULTIPLY(z3, FIX(0.790569415)); /* (c6+c12)/2 */
    tmp11 = MULTIPLY(z4, FIX(0.353553391)); /* (c6-c12)/2 */

    tmp21 = tmp12 + tmp10 + tmp11;
    tmp24 = tmp13 - tmp10 + tmp11;
    tmp11 += tmp11;
    tmp22 = z1 + tmp11;                     /* c10 = c6-c12 */
    tmp27 = z1 - tmp11 - tmp11;             /* c0 = (c6-c12)*2 */

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z4 = (INT32) wsptr[5];
    z3 = MULTIPLY(z4, FIX(1.224744871));                    /* c5 */
    z4 = (INT32) wsptr[7];

    tmp13 = z2 - z4;
    tmp15 = MULTIPLY(z1 + tmp13, FIX(0.831253876));         /* c9 */
    tmp11 = tmp15 + MULTIPLY(z1, FIX(0.513743148));         /* c3-c9 */
    tmp14 = tmp15 - MULTIPLY(tmp13, FIX(2.176250899));      /* c3+c9 */

    tmp13 = MULTIPLY(z2, - FIX(0.831253876));               /* -c9 */
    tmp15 = MULTIPLY(z2, - FIX(1.344997024));               /* -c3 */
    z2 = z1 - z4;
    tmp12 = z3 + MULTIPLY(z2, FIX(1.406466353));            /* c1 */

    tmp10 = tmp12 + MULTIPLY(z4, FIX(2.457431844)) - tmp15; /* c1+c7 */
    tmp16 = tmp12 - MULTIPLY(z1, FIX(1.112434820)) + tmp13; /* c1-c13 */
    tmp12 = MULTIPLY(z2, FIX(1.224744871)) - z3;            /* c5 */
    z2 = MULTIPLY(z1 + z4, FIX(0.575212477));               /* c11 */
    tmp13 += z2 + MULTIPLY(z1, FIX(0.475753014)) - z3;      /* c7-c11 */
    tmp15 += z2 - MULTIPLY(z4, FIX(0.869244010)) + z3;      /* c11+c13 */

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[14] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[13] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[12] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp26 + tmp16,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp26 - tmp16,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp27,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 16x16 output block.
 *
 * Optimized algorithm with 28 multiplications in the 1-D kernel.
 * cK represents sqrt(2) * cos(K*pi/32).
 */

GLOBAL(void)
jpeg_idct_16x16 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		 JCOEFPTR coef_block,
		 JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3, tmp10, tmp11, tmp12, tmp13;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*16];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp0 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp0 += 1 << (CONST_BITS-PASS1_BITS-1);

    z1 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    tmp1 = MULTIPLY(z1, FIX(1.306562965));      /* c4[16] = c2[8] */
    tmp2 = MULTIPLY(z1, FIX_0_541196100);       /* c12[16] = c6[8] */

    tmp10 = tmp0 + tmp1;
    tmp11 = tmp0 - tmp1;
    tmp12 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);
    z3 = z1 - z2;
    z4 = MULTIPLY(z3, FIX(0.275899379));        /* c14[16] = c7[8] */
    z3 = MULTIPLY(z3, FIX(1.387039845));        /* c2[16] = c1[8] */

    tmp0 = z3 + MULTIPLY(z2, FIX_2_562915447);  /* (c6+c2)[16] = (c3+c1)[8] */
    tmp1 = z4 + MULTIPLY(z1, FIX_0_899976223);  /* (c6-c14)[16] = (c3-c7)[8] */
    tmp2 = z3 - MULTIPLY(z1, FIX(0.601344887)); /* (c2-c10)[16] = (c1-c5)[8] */
    tmp3 = z4 - MULTIPLY(z2, FIX(0.509795579)); /* (c10-c14)[16] = (c5-c7)[8] */

    tmp20 = tmp10 + tmp0;
    tmp27 = tmp10 - tmp0;
    tmp21 = tmp12 + tmp1;
    tmp26 = tmp12 - tmp1;
    tmp22 = tmp13 + tmp2;
    tmp25 = tmp13 - tmp2;
    tmp23 = tmp11 + tmp3;
    tmp24 = tmp11 - tmp3;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = z1 + z3;

    tmp1  = MULTIPLY(z1 + z2, FIX(1.353318001));   /* c3 */
    tmp2  = MULTIPLY(tmp11,   FIX(1.247225013));   /* c5 */
    tmp3  = MULTIPLY(z1 + z4, FIX(1.093201867));   /* c7 */
    tmp10 = MULTIPLY(z1 - z4, FIX(0.897167586));   /* c9 */
    tmp11 = MULTIPLY(tmp11,   FIX(0.666655658));   /* c11 */
    tmp12 = MULTIPLY(z1 - z2, FIX(0.410524528));   /* c13 */
    tmp0  = tmp1 + tmp2 + tmp3 -
	    MULTIPLY(z1, FIX(2.286341144));        /* c7+c5+c3-c1 */
    tmp13 = tmp10 + tmp11 + tmp12 -
	    MULTIPLY(z1, FIX(1.835730603));        /* c9+c11+c13-c15 */
    z1    = MULTIPLY(z2 + z3, FIX(0.138617169));   /* c15 */
    tmp1  += z1 + MULTIPLY(z2, FIX(0.071888074));  /* c9+c11-c3-c15 */
    tmp2  += z1 - MULTIPLY(z3, FIX(1.125726048));  /* c5+c7+c15-c3 */
    z1    = MULTIPLY(z3 - z2, FIX(1.407403738));   /* c1 */
    tmp11 += z1 - MULTIPLY(z3, FIX(0.766367282));  /* c1+c11-c9-c13 */
    tmp12 += z1 + MULTIPLY(z2, FIX(1.971951411));  /* c1+c5+c13-c7 */
    z2    += z4;
    z1    = MULTIPLY(z2, - FIX(0.666655658));      /* -c11 */
    tmp1  += z1;
    tmp3  += z1 + MULTIPLY(z4, FIX(1.065388962));  /* c3+c11+c15-c7 */
    z2    = MULTIPLY(z2, - FIX(1.247225013));      /* -c5 */
    tmp10 += z2 + MULTIPLY(z4, FIX(3.141271809));  /* c1+c5+c9-c13 */
    tmp12 += z2;
    z2    = MULTIPLY(z3 + z4, - FIX(1.353318001)); /* -c3 */
    tmp2  += z2;
    tmp3  += z2;
    z2    = MULTIPLY(z4 - z3, FIX(0.410524528));   /* c13 */
    tmp10 += z2;
    tmp11 += z2;

    /* Final output stage */

    wsptr[8*0]  = (int) RIGHT_SHIFT(tmp20 + tmp0,  CONST_BITS-PASS1_BITS);
    wsptr[8*15] = (int) RIGHT_SHIFT(tmp20 - tmp0,  CONST_BITS-PASS1_BITS);
    wsptr[8*1]  = (int) RIGHT_SHIFT(tmp21 + tmp1,  CONST_BITS-PASS1_BITS);
    wsptr[8*14] = (int) RIGHT_SHIFT(tmp21 - tmp1,  CONST_BITS-PASS1_BITS);
    wsptr[8*2]  = (int) RIGHT_SHIFT(tmp22 + tmp2,  CONST_BITS-PASS1_BITS);
    wsptr[8*13] = (int) RIGHT_SHIFT(tmp22 - tmp2,  CONST_BITS-PASS1_BITS);
    wsptr[8*3]  = (int) RIGHT_SHIFT(tmp23 + tmp3,  CONST_BITS-PASS1_BITS);
    wsptr[8*12] = (int) RIGHT_SHIFT(tmp23 - tmp3,  CONST_BITS-PASS1_BITS);
    wsptr[8*4]  = (int) RIGHT_SHIFT(tmp24 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*11] = (int) RIGHT_SHIFT(tmp24 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*5]  = (int) RIGHT_SHIFT(tmp25 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*10] = (int) RIGHT_SHIFT(tmp25 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*6]  = (int) RIGHT_SHIFT(tmp26 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*9]  = (int) RIGHT_SHIFT(tmp26 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*7]  = (int) RIGHT_SHIFT(tmp27 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*8]  = (int) RIGHT_SHIFT(tmp27 - tmp13, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 16 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 16; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp0 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp0 <<= CONST_BITS;

    z1 = (INT32) wsptr[4];
    tmp1 = MULTIPLY(z1, FIX(1.306562965));      /* c4[16] = c2[8] */
    tmp2 = MULTIPLY(z1, FIX_0_541196100);       /* c12[16] = c6[8] */

    tmp10 = tmp0 + tmp1;
    tmp11 = tmp0 - tmp1;
    tmp12 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;

    z1 = (INT32) wsptr[2];
    z2 = (INT32) wsptr[6];
    z3 = z1 - z2;
    z4 = MULTIPLY(z3, FIX(0.275899379));        /* c14[16] = c7[8] */
    z3 = MULTIPLY(z3, FIX(1.387039845));        /* c2[16] = c1[8] */

    tmp0 = z3 + MULTIPLY(z2, FIX_2_562915447);  /* (c6+c2)[16] = (c3+c1)[8] */
    tmp1 = z4 + MULTIPLY(z1, FIX_0_899976223);  /* (c6-c14)[16] = (c3-c7)[8] */
    tmp2 = z3 - MULTIPLY(z1, FIX(0.601344887)); /* (c2-c10)[16] = (c1-c5)[8] */
    tmp3 = z4 - MULTIPLY(z2, FIX(0.509795579)); /* (c10-c14)[16] = (c5-c7)[8] */

    tmp20 = tmp10 + tmp0;
    tmp27 = tmp10 - tmp0;
    tmp21 = tmp12 + tmp1;
    tmp26 = tmp12 - tmp1;
    tmp22 = tmp13 + tmp2;
    tmp25 = tmp13 - tmp2;
    tmp23 = tmp11 + tmp3;
    tmp24 = tmp11 - tmp3;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];

    tmp11 = z1 + z3;

    tmp1  = MULTIPLY(z1 + z2, FIX(1.353318001));   /* c3 */
    tmp2  = MULTIPLY(tmp11,   FIX(1.247225013));   /* c5 */
    tmp3  = MULTIPLY(z1 + z4, FIX(1.093201867));   /* c7 */
    tmp10 = MULTIPLY(z1 - z4, FIX(0.897167586));   /* c9 */
    tmp11 = MULTIPLY(tmp11,   FIX(0.666655658));   /* c11 */
    tmp12 = MULTIPLY(z1 - z2, FIX(0.410524528));   /* c13 */
    tmp0  = tmp1 + tmp2 + tmp3 -
	    MULTIPLY(z1, FIX(2.286341144));        /* c7+c5+c3-c1 */
    tmp13 = tmp10 + tmp11 + tmp12 -
	    MULTIPLY(z1, FIX(1.835730603));        /* c9+c11+c13-c15 */
    z1    = MULTIPLY(z2 + z3, FIX(0.138617169));   /* c15 */
    tmp1  += z1 + MULTIPLY(z2, FIX(0.071888074));  /* c9+c11-c3-c15 */
    tmp2  += z1 - MULTIPLY(z3, FIX(1.125726048));  /* c5+c7+c15-c3 */
    z1    = MULTIPLY(z3 - z2, FIX(1.407403738));   /* c1 */
    tmp11 += z1 - MULTIPLY(z3, FIX(0.766367282));  /* c1+c11-c9-c13 */
    tmp12 += z1 + MULTIPLY(z2, FIX(1.971951411));  /* c1+c5+c13-c7 */
    z2    += z4;
    z1    = MULTIPLY(z2, - FIX(0.666655658));      /* -c11 */
    tmp1  += z1;
    tmp3  += z1 + MULTIPLY(z4, FIX(1.065388962));  /* c3+c11+c15-c7 */
    z2    = MULTIPLY(z2, - FIX(1.247225013));      /* -c5 */
    tmp10 += z2 + MULTIPLY(z4, FIX(3.141271809));  /* c1+c5+c9-c13 */
    tmp12 += z2;
    z2    = MULTIPLY(z3 + z4, - FIX(1.353318001)); /* -c3 */
    tmp2  += z2;
    tmp3  += z2;
    z2    = MULTIPLY(z4 - z3, FIX(0.410524528));   /* c13 */
    tmp10 += z2;
    tmp11 += z2;

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp0,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[15] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp0,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp1,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[14] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp1,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp2,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[13] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp2,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp3,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[12] = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp3,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp26 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp26 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp27 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp27 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 16x8 output block.
 *
 * 8-point IDCT in pass 1 (columns), 16-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_16x8 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3, tmp10, tmp11, tmp12, tmp13;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*8];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * Note results are scaled up by sqrt(8) compared to a true IDCT;
   * furthermore, we scale the results by 2**PASS1_BITS.
   * 8-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/16).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = DCTSIZE; ctr > 0; ctr--) {
    /* Due to quantization, we will usually find that many of the input
     * coefficients are zero, especially the AC terms.  We can exploit this
     * by short-circuiting the IDCT calculation for any column in which all
     * the AC terms are zero.  In that case each output is equal to the
     * DC coefficient (with scale factor as needed).
     * With typical images and quantization tables, half or more of the
     * column DCT calculations can be simplified this way.
     */

    if (inptr[DCTSIZE*1] == 0 && inptr[DCTSIZE*2] == 0 &&
	inptr[DCTSIZE*3] == 0 && inptr[DCTSIZE*4] == 0 &&
	inptr[DCTSIZE*5] == 0 && inptr[DCTSIZE*6] == 0 &&
	inptr[DCTSIZE*7] == 0) {
      /* AC terms all zero */
      int dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]) << PASS1_BITS;

      wsptr[DCTSIZE*0] = dcval;
      wsptr[DCTSIZE*1] = dcval;
      wsptr[DCTSIZE*2] = dcval;
      wsptr[DCTSIZE*3] = dcval;
      wsptr[DCTSIZE*4] = dcval;
      wsptr[DCTSIZE*5] = dcval;
      wsptr[DCTSIZE*6] = dcval;
      wsptr[DCTSIZE*7] = dcval;

      inptr++;			/* advance pointers to next column */
      quantptr++;
      wsptr++;
      continue;
    }

    /* Even part: reverse the even part of the forward DCT.
     * The rotator is c(-6).
     */

    z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);       /* c6 */
    tmp2 = z1 + MULTIPLY(z2, FIX_0_765366865);     /* c2-c6 */
    tmp3 = z1 - MULTIPLY(z3, FIX_1_847759065);     /* c2+c6 */

    z2 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z2 <<= CONST_BITS;
    z3 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z2 += ONE << (CONST_BITS-PASS1_BITS-1);

    tmp0 = z2 + z3;
    tmp1 = z2 - z3;

    tmp10 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;
    tmp11 = tmp1 + tmp3;
    tmp12 = tmp1 - tmp3;

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
    tmp1 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    tmp3 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);

    z2 = tmp0 + tmp2;
    z3 = tmp1 + tmp3;

    z1 = MULTIPLY(z2 + z3, FIX_1_175875602);       /*  c3 */
    z2 = MULTIPLY(z2, - FIX_1_961570560);          /* -c3-c5 */
    z3 = MULTIPLY(z3, - FIX_0_390180644);          /* -c3+c5 */
    z2 += z1;
    z3 += z1;

    z1 = MULTIPLY(tmp0 + tmp3, - FIX_0_899976223); /* -c3+c7 */
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);        /* -c1+c3+c5-c7 */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);        /*  c1+c3-c5-c7 */
    tmp0 += z1 + z2;
    tmp3 += z1 + z3;

    z1 = MULTIPLY(tmp1 + tmp2, - FIX_2_562915447); /* -c1-c3 */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);        /*  c1+c3-c5+c7 */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);        /*  c1+c3+c5-c7 */
    tmp1 += z1 + z3;
    tmp2 += z1 + z2;

    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    wsptr[DCTSIZE*0] = (int) RIGHT_SHIFT(tmp10 + tmp3, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*7] = (int) RIGHT_SHIFT(tmp10 - tmp3, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*1] = (int) RIGHT_SHIFT(tmp11 + tmp2, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*6] = (int) RIGHT_SHIFT(tmp11 - tmp2, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*2] = (int) RIGHT_SHIFT(tmp12 + tmp1, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*5] = (int) RIGHT_SHIFT(tmp12 - tmp1, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*3] = (int) RIGHT_SHIFT(tmp13 + tmp0, CONST_BITS-PASS1_BITS);
    wsptr[DCTSIZE*4] = (int) RIGHT_SHIFT(tmp13 - tmp0, CONST_BITS-PASS1_BITS);

    inptr++;			/* advance pointers to next column */
    quantptr++;
    wsptr++;
  }

  /* Pass 2: process 8 rows from work array, store into output array.
   * 16-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/32).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp0 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp0 <<= CONST_BITS;

    z1 = (INT32) wsptr[4];
    tmp1 = MULTIPLY(z1, FIX(1.306562965));      /* c4[16] = c2[8] */
    tmp2 = MULTIPLY(z1, FIX_0_541196100);       /* c12[16] = c6[8] */

    tmp10 = tmp0 + tmp1;
    tmp11 = tmp0 - tmp1;
    tmp12 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;

    z1 = (INT32) wsptr[2];
    z2 = (INT32) wsptr[6];
    z3 = z1 - z2;
    z4 = MULTIPLY(z3, FIX(0.275899379));        /* c14[16] = c7[8] */
    z3 = MULTIPLY(z3, FIX(1.387039845));        /* c2[16] = c1[8] */

    tmp0 = z3 + MULTIPLY(z2, FIX_2_562915447);  /* (c6+c2)[16] = (c3+c1)[8] */
    tmp1 = z4 + MULTIPLY(z1, FIX_0_899976223);  /* (c6-c14)[16] = (c3-c7)[8] */
    tmp2 = z3 - MULTIPLY(z1, FIX(0.601344887)); /* (c2-c10)[16] = (c1-c5)[8] */
    tmp3 = z4 - MULTIPLY(z2, FIX(0.509795579)); /* (c10-c14)[16] = (c5-c7)[8] */

    tmp20 = tmp10 + tmp0;
    tmp27 = tmp10 - tmp0;
    tmp21 = tmp12 + tmp1;
    tmp26 = tmp12 - tmp1;
    tmp22 = tmp13 + tmp2;
    tmp25 = tmp13 - tmp2;
    tmp23 = tmp11 + tmp3;
    tmp24 = tmp11 - tmp3;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];

    tmp11 = z1 + z3;

    tmp1  = MULTIPLY(z1 + z2, FIX(1.353318001));   /* c3 */
    tmp2  = MULTIPLY(tmp11,   FIX(1.247225013));   /* c5 */
    tmp3  = MULTIPLY(z1 + z4, FIX(1.093201867));   /* c7 */
    tmp10 = MULTIPLY(z1 - z4, FIX(0.897167586));   /* c9 */
    tmp11 = MULTIPLY(tmp11,   FIX(0.666655658));   /* c11 */
    tmp12 = MULTIPLY(z1 - z2, FIX(0.410524528));   /* c13 */
    tmp0  = tmp1 + tmp2 + tmp3 -
	    MULTIPLY(z1, FIX(2.286341144));        /* c7+c5+c3-c1 */
    tmp13 = tmp10 + tmp11 + tmp12 -
	    MULTIPLY(z1, FIX(1.835730603));        /* c9+c11+c13-c15 */
    z1    = MULTIPLY(z2 + z3, FIX(0.138617169));   /* c15 */
    tmp1  += z1 + MULTIPLY(z2, FIX(0.071888074));  /* c9+c11-c3-c15 */
    tmp2  += z1 - MULTIPLY(z3, FIX(1.125726048));  /* c5+c7+c15-c3 */
    z1    = MULTIPLY(z3 - z2, FIX(1.407403738));   /* c1 */
    tmp11 += z1 - MULTIPLY(z3, FIX(0.766367282));  /* c1+c11-c9-c13 */
    tmp12 += z1 + MULTIPLY(z2, FIX(1.971951411));  /* c1+c5+c13-c7 */
    z2    += z4;
    z1    = MULTIPLY(z2, - FIX(0.666655658));      /* -c11 */
    tmp1  += z1;
    tmp3  += z1 + MULTIPLY(z4, FIX(1.065388962));  /* c3+c11+c15-c7 */
    z2    = MULTIPLY(z2, - FIX(1.247225013));      /* -c5 */
    tmp10 += z2 + MULTIPLY(z4, FIX(3.141271809));  /* c1+c5+c9-c13 */
    tmp12 += z2;
    z2    = MULTIPLY(z3 + z4, - FIX(1.353318001)); /* -c3 */
    tmp2  += z2;
    tmp3  += z2;
    z2    = MULTIPLY(z4 - z3, FIX(0.410524528));   /* c13 */
    tmp10 += z2;
    tmp11 += z2;

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp0,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[15] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp0,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp1,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[14] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp1,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp2,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[13] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp2,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp3,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[12] = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp3,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp26 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp26 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp27 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp27 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 14x7 output block.
 *
 * 7-point IDCT in pass 1 (columns), 14-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_14x7 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*7];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 7-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/14).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp23 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp23 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp23 += ONE << (CONST_BITS-PASS1_BITS-1);

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    tmp20 = MULTIPLY(z2 - z3, FIX(0.881747734));       /* c4 */
    tmp22 = MULTIPLY(z1 - z2, FIX(0.314692123));       /* c6 */
    tmp21 = tmp20 + tmp22 + tmp23 - MULTIPLY(z2, FIX(1.841218003)); /* c2+c4-c6 */
    tmp10 = z1 + z3;
    z2 -= tmp10;
    tmp10 = MULTIPLY(tmp10, FIX(1.274162392)) + tmp23; /* c2 */
    tmp20 += tmp10 - MULTIPLY(z3, FIX(0.077722536));   /* c2-c4-c6 */
    tmp22 += tmp10 - MULTIPLY(z1, FIX(2.470602249));   /* c2+c4+c6 */
    tmp23 += MULTIPLY(z2, FIX(1.414213562));           /* c0 */

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);

    tmp11 = MULTIPLY(z1 + z2, FIX(0.935414347));       /* (c3+c1-c5)/2 */
    tmp12 = MULTIPLY(z1 - z2, FIX(0.170262339));       /* (c3+c5-c1)/2 */
    tmp10 = tmp11 - tmp12;
    tmp11 += tmp12;
    tmp12 = MULTIPLY(z2 + z3, - FIX(1.378756276));     /* -c1 */
    tmp11 += tmp12;
    z2 = MULTIPLY(z1 + z3, FIX(0.613604268));          /* c5 */
    tmp10 += z2;
    tmp12 += z2 + MULTIPLY(z3, FIX(1.870828693));      /* c3+c1-c5 */

    /* Final output stage */

    wsptr[8*0] = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*6] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1] = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*5] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*2] = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*4] = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*3] = (int) RIGHT_SHIFT(tmp23, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 7 rows from work array, store into output array.
   * 14-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/28).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 7; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z1 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z1 <<= CONST_BITS;
    z4 = (INT32) wsptr[4];
    z2 = MULTIPLY(z4, FIX(1.274162392));         /* c4 */
    z3 = MULTIPLY(z4, FIX(0.314692123));         /* c12 */
    z4 = MULTIPLY(z4, FIX(0.881747734));         /* c8 */

    tmp10 = z1 + z2;
    tmp11 = z1 + z3;
    tmp12 = z1 - z4;

    tmp23 = z1 - ((z2 + z3 - z4) << 1);          /* c0 = (c4+c12-c8)*2 */

    z1 = (INT32) wsptr[2];
    z2 = (INT32) wsptr[6];

    z3 = MULTIPLY(z1 + z2, FIX(1.105676686));    /* c6 */

    tmp13 = z3 + MULTIPLY(z1, FIX(0.273079590)); /* c2-c6 */
    tmp14 = z3 - MULTIPLY(z2, FIX(1.719280954)); /* c6+c10 */
    tmp15 = MULTIPLY(z1, FIX(0.613604268)) -     /* c10 */
	    MULTIPLY(z2, FIX(1.378756276));      /* c2 */

    tmp20 = tmp10 + tmp13;
    tmp26 = tmp10 - tmp13;
    tmp21 = tmp11 + tmp14;
    tmp25 = tmp11 - tmp14;
    tmp22 = tmp12 + tmp15;
    tmp24 = tmp12 - tmp15;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];
    z4 <<= CONST_BITS;

    tmp14 = z1 + z3;
    tmp11 = MULTIPLY(z1 + z2, FIX(1.334852607));           /* c3 */
    tmp12 = MULTIPLY(tmp14, FIX(1.197448846));             /* c5 */
    tmp10 = tmp11 + tmp12 + z4 - MULTIPLY(z1, FIX(1.126980169)); /* c3+c5-c1 */
    tmp14 = MULTIPLY(tmp14, FIX(0.752406978));             /* c9 */
    tmp16 = tmp14 - MULTIPLY(z1, FIX(1.061150426));        /* c9+c11-c13 */
    z1    -= z2;
    tmp15 = MULTIPLY(z1, FIX(0.467085129)) - z4;           /* c11 */
    tmp16 += tmp15;
    tmp13 = MULTIPLY(z2 + z3, - FIX(0.158341681)) - z4;    /* -c13 */
    tmp11 += tmp13 - MULTIPLY(z2, FIX(0.424103948));       /* c3-c9-c13 */
    tmp12 += tmp13 - MULTIPLY(z3, FIX(2.373959773));       /* c3+c5-c13 */
    tmp13 = MULTIPLY(z3 - z2, FIX(1.405321284));           /* c1 */
    tmp14 += tmp13 + z4 - MULTIPLY(z3, FIX(1.6906431334)); /* c1+c9-c11 */
    tmp15 += tmp13 + MULTIPLY(z2, FIX(0.674957567));       /* c1+c11-c5 */

    tmp13 = ((z1 - z3) << CONST_BITS) + z4;

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[13] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[12] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp26 + tmp16,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp26 - tmp16,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 12x6 output block.
 *
 * 6-point IDCT in pass 1 (columns), 12-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_12x6 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*6];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 6-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/12).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp10 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp10 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp10 += ONE << (CONST_BITS-PASS1_BITS-1);
    tmp12 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    tmp20 = MULTIPLY(tmp12, FIX(0.707106781));   /* c4 */
    tmp11 = tmp10 + tmp20;
    tmp21 = RIGHT_SHIFT(tmp10 - tmp20 - tmp20, CONST_BITS-PASS1_BITS);
    tmp20 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    tmp10 = MULTIPLY(tmp20, FIX(1.224744871));   /* c2 */
    tmp20 = tmp11 + tmp10;
    tmp22 = tmp11 - tmp10;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    tmp11 = MULTIPLY(z1 + z3, FIX(0.366025404)); /* c5 */
    tmp10 = tmp11 + ((z1 + z2) << CONST_BITS);
    tmp12 = tmp11 + ((z3 - z2) << CONST_BITS);
    tmp11 = (z1 - z2 - z3) << PASS1_BITS;

    /* Final output stage */

    wsptr[8*0] = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*5] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*1] = (int) (tmp21 + tmp11);
    wsptr[8*4] = (int) (tmp21 - tmp11);
    wsptr[8*2] = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*3] = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 6 rows from work array, store into output array.
   * 12-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/24).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 6; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z3 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z3 <<= CONST_BITS;

    z4 = (INT32) wsptr[4];
    z4 = MULTIPLY(z4, FIX(1.224744871)); /* c4 */

    tmp10 = z3 + z4;
    tmp11 = z3 - z4;

    z1 = (INT32) wsptr[2];
    z4 = MULTIPLY(z1, FIX(1.366025404)); /* c2 */
    z1 <<= CONST_BITS;
    z2 = (INT32) wsptr[6];
    z2 <<= CONST_BITS;

    tmp12 = z1 - z2;

    tmp21 = z3 + tmp12;
    tmp24 = z3 - tmp12;

    tmp12 = z4 + z2;

    tmp20 = tmp10 + tmp12;
    tmp25 = tmp10 - tmp12;

    tmp12 = z4 - z1 - z2;

    tmp22 = tmp11 + tmp12;
    tmp23 = tmp11 - tmp12;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z4 = (INT32) wsptr[7];

    tmp11 = MULTIPLY(z2, FIX(1.306562965));                  /* c3 */
    tmp14 = MULTIPLY(z2, - FIX_0_541196100);                 /* -c9 */

    tmp10 = z1 + z3;
    tmp15 = MULTIPLY(tmp10 + z4, FIX(0.860918669));          /* c7 */
    tmp12 = tmp15 + MULTIPLY(tmp10, FIX(0.261052384));       /* c5-c7 */
    tmp10 = tmp12 + tmp11 + MULTIPLY(z1, FIX(0.280143716));  /* c1-c5 */
    tmp13 = MULTIPLY(z3 + z4, - FIX(1.045510580));           /* -(c7+c11) */
    tmp12 += tmp13 + tmp14 - MULTIPLY(z3, FIX(1.478575242)); /* c1+c5-c7-c11 */
    tmp13 += tmp15 - tmp11 + MULTIPLY(z4, FIX(1.586706681)); /* c1+c11 */
    tmp15 += tmp14 - MULTIPLY(z1, FIX(0.676326758)) -        /* c7-c11 */
	     MULTIPLY(z4, FIX(1.982889723));                 /* c5+c7 */

    z1 -= z4;
    z2 -= z3;
    z3 = MULTIPLY(z1 + z2, FIX_0_541196100);                 /* c9 */
    tmp11 = z3 + MULTIPLY(z1, FIX_0_765366865);              /* c3-c9 */
    tmp14 = z3 - MULTIPLY(z2, FIX_1_847759065);              /* c3+c9 */

    /* Final output stage */

    outptr[0]  = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[11] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[1]  = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[10] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[2]  = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[9]  = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[3]  = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[8]  = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[4]  = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[7]  = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[5]  = range_limit[(int) RIGHT_SHIFT(tmp25 + tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];
    outptr[6]  = range_limit[(int) RIGHT_SHIFT(tmp25 - tmp15,
					       CONST_BITS+PASS1_BITS+3)
			     & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 10x5 output block.
 *
 * 5-point IDCT in pass 1 (columns), 10-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_10x5 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*5];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 5-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/10).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp12 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp12 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp12 += ONE << (CONST_BITS-PASS1_BITS-1);
    tmp13 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    tmp14 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z1 = MULTIPLY(tmp13 + tmp14, FIX(0.790569415)); /* (c2+c4)/2 */
    z2 = MULTIPLY(tmp13 - tmp14, FIX(0.353553391)); /* (c2-c4)/2 */
    z3 = tmp12 + z2;
    tmp10 = z3 + z1;
    tmp11 = z3 - z1;
    tmp12 -= z2 << 2;

    /* Odd part */

    z2 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);

    z1 = MULTIPLY(z2 + z3, FIX(0.831253876));       /* c3 */
    tmp13 = z1 + MULTIPLY(z2, FIX(0.513743148));    /* c1-c3 */
    tmp14 = z1 - MULTIPLY(z3, FIX(2.176250899));    /* c1+c3 */

    /* Final output stage */

    wsptr[8*0] = (int) RIGHT_SHIFT(tmp10 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*4] = (int) RIGHT_SHIFT(tmp10 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*1] = (int) RIGHT_SHIFT(tmp11 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*3] = (int) RIGHT_SHIFT(tmp11 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[8*2] = (int) RIGHT_SHIFT(tmp12, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 5 rows from work array, store into output array.
   * 10-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/20).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 5; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    z3 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z3 <<= CONST_BITS;
    z4 = (INT32) wsptr[4];
    z1 = MULTIPLY(z4, FIX(1.144122806));         /* c4 */
    z2 = MULTIPLY(z4, FIX(0.437016024));         /* c8 */
    tmp10 = z3 + z1;
    tmp11 = z3 - z2;

    tmp22 = z3 - ((z1 - z2) << 1);               /* c0 = (c4-c8)*2 */

    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[6];

    z1 = MULTIPLY(z2 + z3, FIX(0.831253876));    /* c6 */
    tmp12 = z1 + MULTIPLY(z2, FIX(0.513743148)); /* c2-c6 */
    tmp13 = z1 - MULTIPLY(z3, FIX(2.176250899)); /* c2+c6 */

    tmp20 = tmp10 + tmp12;
    tmp24 = tmp10 - tmp12;
    tmp21 = tmp11 + tmp13;
    tmp23 = tmp11 - tmp13;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    z3 <<= CONST_BITS;
    z4 = (INT32) wsptr[7];

    tmp11 = z2 + z4;
    tmp13 = z2 - z4;

    tmp12 = MULTIPLY(tmp13, FIX(0.309016994));        /* (c3-c7)/2 */

    z2 = MULTIPLY(tmp11, FIX(0.951056516));           /* (c3+c7)/2 */
    z4 = z3 + tmp12;

    tmp10 = MULTIPLY(z1, FIX(1.396802247)) + z2 + z4; /* c1 */
    tmp14 = MULTIPLY(z1, FIX(0.221231742)) - z2 + z4; /* c9 */

    z2 = MULTIPLY(tmp11, FIX(0.587785252));           /* (c1-c9)/2 */
    z4 = z3 - tmp12 - (tmp13 << (CONST_BITS - 1));

    tmp12 = ((z1 - tmp13) << CONST_BITS) - z3;

    tmp11 = MULTIPLY(z1, FIX(1.260073511)) - z2 - z4; /* c3 */
    tmp13 = MULTIPLY(z1, FIX(0.642039522)) - z2 + z4; /* c7 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[9] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[8] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[7] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp23 + tmp13,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) RIGHT_SHIFT(tmp23 - tmp13,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp24 + tmp14,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp24 - tmp14,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 8;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 8x4 output block.
 *
 * 4-point IDCT in pass 1 (columns), 8-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_8x4 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3;
  INT32 tmp10, tmp11, tmp12, tmp13;
  INT32 z1, z2, z3;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*4];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 4-point IDCT kernel,
   * cK represents sqrt(2) * cos(K*pi/16) [refers to 8-point IDCT].
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);

    tmp10 = (tmp0 + tmp2) << PASS1_BITS;
    tmp12 = (tmp0 - tmp2) << PASS1_BITS;

    /* Odd part */
    /* Same rotation as in the even part of the 8x8 LL&M IDCT */

    z2 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);               /* c6 */
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS-PASS1_BITS-1);
    tmp0 = RIGHT_SHIFT(z1 + MULTIPLY(z2, FIX_0_765366865), /* c2-c6 */
		       CONST_BITS-PASS1_BITS);
    tmp2 = RIGHT_SHIFT(z1 - MULTIPLY(z3, FIX_1_847759065), /* c2+c6 */
		       CONST_BITS-PASS1_BITS);

    /* Final output stage */

    wsptr[8*0] = (int) (tmp10 + tmp0);
    wsptr[8*3] = (int) (tmp10 - tmp0);
    wsptr[8*1] = (int) (tmp12 + tmp2);
    wsptr[8*2] = (int) (tmp12 - tmp2);
  }

  /* Pass 2: process rows from work array, store into output array.
   * Note that we must descale the results by a factor of 8 == 2**3,
   * and also undo the PASS1_BITS scaling.
   * 8-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/16).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 4; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part: reverse the even part of the forward DCT.
     * The rotator is c(-6).
     */

    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[6];

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);       /* c6 */
    tmp2 = z1 + MULTIPLY(z2, FIX_0_765366865);     /* c2-c6 */
    tmp3 = z1 - MULTIPLY(z3, FIX_1_847759065);     /* c2+c6 */

    /* Add fudge factor here for final descale. */
    z2 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z3 = (INT32) wsptr[4];

    tmp0 = (z2 + z3) << CONST_BITS;
    tmp1 = (z2 - z3) << CONST_BITS;

    tmp10 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;
    tmp11 = tmp1 + tmp3;
    tmp12 = tmp1 - tmp3;

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */

    tmp0 = (INT32) wsptr[7];
    tmp1 = (INT32) wsptr[5];
    tmp2 = (INT32) wsptr[3];
    tmp3 = (INT32) wsptr[1];

    z2 = tmp0 + tmp2;
    z3 = tmp1 + tmp3;

    z1 = MULTIPLY(z2 + z3, FIX_1_175875602);       /*  c3 */
    z2 = MULTIPLY(z2, - FIX_1_961570560);          /* -c3-c5 */
    z3 = MULTIPLY(z3, - FIX_0_390180644);          /* -c3+c5 */
    z2 += z1;
    z3 += z1;

    z1 = MULTIPLY(tmp0 + tmp3, - FIX_0_899976223); /* -c3+c7 */
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);        /* -c1+c3+c5-c7 */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);        /*  c1+c3-c5-c7 */
    tmp0 += z1 + z2;
    tmp3 += z1 + z3;

    z1 = MULTIPLY(tmp1 + tmp2, - FIX_2_562915447); /* -c1-c3 */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);        /*  c1+c3-c5+c7 */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);        /*  c1+c3+c5-c7 */
    tmp1 += z1 + z3;
    tmp2 += z1 + z2;

    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[7] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp11 + tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) RIGHT_SHIFT(tmp11 - tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12 + tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp12 - tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp13 + tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp13 - tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += DCTSIZE;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a reduced-size 6x3 output block.
 *
 * 3-point IDCT in pass 1 (columns), 6-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_6x3 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp10, tmp11, tmp12;
  INT32 z1, z2, z3;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[6*3];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 3-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/6).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 6; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp0 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp0 += ONE << (CONST_BITS-PASS1_BITS-1);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    tmp12 = MULTIPLY(tmp2, FIX(0.707106781)); /* c2 */
    tmp10 = tmp0 + tmp12;
    tmp2 = tmp0 - tmp12 - tmp12;

    /* Odd part */

    tmp12 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    tmp0 = MULTIPLY(tmp12, FIX(1.224744871)); /* c1 */

    /* Final output stage */

    wsptr[6*0] = (int) RIGHT_SHIFT(tmp10 + tmp0, CONST_BITS-PASS1_BITS);
    wsptr[6*2] = (int) RIGHT_SHIFT(tmp10 - tmp0, CONST_BITS-PASS1_BITS);
    wsptr[6*1] = (int) RIGHT_SHIFT(tmp2, CONST_BITS-PASS1_BITS);
  }
  
  /* Pass 2: process 3 rows from work array, store into output array.
   * 6-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/12).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 3; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp0 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp0 <<= CONST_BITS;
    tmp2 = (INT32) wsptr[4];
    tmp10 = MULTIPLY(tmp2, FIX(0.707106781));   /* c4 */
    tmp1 = tmp0 + tmp10;
    tmp11 = tmp0 - tmp10 - tmp10;
    tmp10 = (INT32) wsptr[2];
    tmp0 = MULTIPLY(tmp10, FIX(1.224744871));   /* c2 */
    tmp10 = tmp1 + tmp0;
    tmp12 = tmp1 - tmp0;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    tmp1 = MULTIPLY(z1 + z3, FIX(0.366025404)); /* c5 */
    tmp0 = tmp1 + ((z1 + z2) << CONST_BITS);
    tmp2 = tmp1 + ((z3 - z2) << CONST_BITS);
    tmp1 = (z1 - z2 - z3) << CONST_BITS;

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp11 + tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp11 - tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12 + tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp12 - tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 6;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 4x2 output block.
 *
 * 2-point IDCT in pass 1 (columns), 4-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_4x2 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp2, tmp10, tmp12;
  INT32 z1, z2, z3;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  INT32 * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  INT32 workspace[4*2];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array. */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 4; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp10 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);

    /* Odd part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);

    /* Final output stage */

    wsptr[4*0] = tmp10 + tmp0;
    wsptr[4*1] = tmp10 - tmp0;
  }

  /* Pass 2: process 2 rows from work array, store into output array.
   * 4-point IDCT kernel,
   * cK represents sqrt(2) * cos(K*pi/16) [refers to 8-point IDCT].
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 2; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp0 = wsptr[0] + (ONE << 2);
    tmp2 = wsptr[2];

    tmp10 = (tmp0 + tmp2) << CONST_BITS;
    tmp12 = (tmp0 - tmp2) << CONST_BITS;

    /* Odd part */
    /* Same rotation as in the even part of the 8x8 LL&M IDCT */

    z2 = wsptr[1];
    z3 = wsptr[3];

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);   /* c6 */
    tmp0 = z1 + MULTIPLY(z2, FIX_0_765366865); /* c2-c6 */
    tmp2 = z1 - MULTIPLY(z3, FIX_1_847759065); /* c2+c6 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp0,
					      CONST_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp0,
					      CONST_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp12 + tmp2,
					      CONST_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12 - tmp2,
					      CONST_BITS+3)
			    & RANGE_MASK];

    wsptr += 4;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 2x1 output block.
 *
 * 1-point IDCT in pass 1 (columns), 2-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_2x1 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1;
  ISLOW_MULT_TYPE * quantptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  SHIFT_TEMPS

  /* Pass 1: empty. */

  /* Pass 2: process 1 row from input, store into output array. */

  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  outptr = output_buf[0] + output_col;

  /* Even part */

  tmp0 = DEQUANTIZE(coef_block[0], quantptr[0]);
  /* Add fudge factor here for final descale. */
  tmp0 += ONE << 2;

  /* Odd part */

  tmp1 = DEQUANTIZE(coef_block[1], quantptr[1]);

  /* Final output stage */

  outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp0 + tmp1, 3) & RANGE_MASK];
  outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp0 - tmp1, 3) & RANGE_MASK];
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 8x16 output block.
 *
 * 16-point IDCT in pass 1 (columns), 8-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_8x16 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3, tmp10, tmp11, tmp12, tmp13;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[8*16];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 16-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/32).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp0 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp0 += ONE << (CONST_BITS-PASS1_BITS-1);

    z1 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    tmp1 = MULTIPLY(z1, FIX(1.306562965));      /* c4[16] = c2[8] */
    tmp2 = MULTIPLY(z1, FIX_0_541196100);       /* c12[16] = c6[8] */

    tmp10 = tmp0 + tmp1;
    tmp11 = tmp0 - tmp1;
    tmp12 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);
    z3 = z1 - z2;
    z4 = MULTIPLY(z3, FIX(0.275899379));        /* c14[16] = c7[8] */
    z3 = MULTIPLY(z3, FIX(1.387039845));        /* c2[16] = c1[8] */

    tmp0 = z3 + MULTIPLY(z2, FIX_2_562915447);  /* (c6+c2)[16] = (c3+c1)[8] */
    tmp1 = z4 + MULTIPLY(z1, FIX_0_899976223);  /* (c6-c14)[16] = (c3-c7)[8] */
    tmp2 = z3 - MULTIPLY(z1, FIX(0.601344887)); /* (c2-c10)[16] = (c1-c5)[8] */
    tmp3 = z4 - MULTIPLY(z2, FIX(0.509795579)); /* (c10-c14)[16] = (c5-c7)[8] */

    tmp20 = tmp10 + tmp0;
    tmp27 = tmp10 - tmp0;
    tmp21 = tmp12 + tmp1;
    tmp26 = tmp12 - tmp1;
    tmp22 = tmp13 + tmp2;
    tmp25 = tmp13 - tmp2;
    tmp23 = tmp11 + tmp3;
    tmp24 = tmp11 - tmp3;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = z1 + z3;

    tmp1  = MULTIPLY(z1 + z2, FIX(1.353318001));   /* c3 */
    tmp2  = MULTIPLY(tmp11,   FIX(1.247225013));   /* c5 */
    tmp3  = MULTIPLY(z1 + z4, FIX(1.093201867));   /* c7 */
    tmp10 = MULTIPLY(z1 - z4, FIX(0.897167586));   /* c9 */
    tmp11 = MULTIPLY(tmp11,   FIX(0.666655658));   /* c11 */
    tmp12 = MULTIPLY(z1 - z2, FIX(0.410524528));   /* c13 */
    tmp0  = tmp1 + tmp2 + tmp3 -
	    MULTIPLY(z1, FIX(2.286341144));        /* c7+c5+c3-c1 */
    tmp13 = tmp10 + tmp11 + tmp12 -
	    MULTIPLY(z1, FIX(1.835730603));        /* c9+c11+c13-c15 */
    z1    = MULTIPLY(z2 + z3, FIX(0.138617169));   /* c15 */
    tmp1  += z1 + MULTIPLY(z2, FIX(0.071888074));  /* c9+c11-c3-c15 */
    tmp2  += z1 - MULTIPLY(z3, FIX(1.125726048));  /* c5+c7+c15-c3 */
    z1    = MULTIPLY(z3 - z2, FIX(1.407403738));   /* c1 */
    tmp11 += z1 - MULTIPLY(z3, FIX(0.766367282));  /* c1+c11-c9-c13 */
    tmp12 += z1 + MULTIPLY(z2, FIX(1.971951411));  /* c1+c5+c13-c7 */
    z2    += z4;
    z1    = MULTIPLY(z2, - FIX(0.666655658));      /* -c11 */
    tmp1  += z1;
    tmp3  += z1 + MULTIPLY(z4, FIX(1.065388962));  /* c3+c11+c15-c7 */
    z2    = MULTIPLY(z2, - FIX(1.247225013));      /* -c5 */
    tmp10 += z2 + MULTIPLY(z4, FIX(3.141271809));  /* c1+c5+c9-c13 */
    tmp12 += z2;
    z2    = MULTIPLY(z3 + z4, - FIX(1.353318001)); /* -c3 */
    tmp2  += z2;
    tmp3  += z2;
    z2    = MULTIPLY(z4 - z3, FIX(0.410524528));   /* c13 */
    tmp10 += z2;
    tmp11 += z2;

    /* Final output stage */

    wsptr[8*0]  = (int) RIGHT_SHIFT(tmp20 + tmp0,  CONST_BITS-PASS1_BITS);
    wsptr[8*15] = (int) RIGHT_SHIFT(tmp20 - tmp0,  CONST_BITS-PASS1_BITS);
    wsptr[8*1]  = (int) RIGHT_SHIFT(tmp21 + tmp1,  CONST_BITS-PASS1_BITS);
    wsptr[8*14] = (int) RIGHT_SHIFT(tmp21 - tmp1,  CONST_BITS-PASS1_BITS);
    wsptr[8*2]  = (int) RIGHT_SHIFT(tmp22 + tmp2,  CONST_BITS-PASS1_BITS);
    wsptr[8*13] = (int) RIGHT_SHIFT(tmp22 - tmp2,  CONST_BITS-PASS1_BITS);
    wsptr[8*3]  = (int) RIGHT_SHIFT(tmp23 + tmp3,  CONST_BITS-PASS1_BITS);
    wsptr[8*12] = (int) RIGHT_SHIFT(tmp23 - tmp3,  CONST_BITS-PASS1_BITS);
    wsptr[8*4]  = (int) RIGHT_SHIFT(tmp24 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*11] = (int) RIGHT_SHIFT(tmp24 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[8*5]  = (int) RIGHT_SHIFT(tmp25 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*10] = (int) RIGHT_SHIFT(tmp25 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[8*6]  = (int) RIGHT_SHIFT(tmp26 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*9]  = (int) RIGHT_SHIFT(tmp26 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[8*7]  = (int) RIGHT_SHIFT(tmp27 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[8*8]  = (int) RIGHT_SHIFT(tmp27 - tmp13, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process rows from work array, store into output array.
   * Note that we must descale the results by a factor of 8 == 2**3,
   * and also undo the PASS1_BITS scaling.
   * 8-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/16).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 16; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part: reverse the even part of the forward DCT.
     * The rotator is c(-6).
     */

    z2 = (INT32) wsptr[2];
    z3 = (INT32) wsptr[6];

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);       /* c6 */
    tmp2 = z1 + MULTIPLY(z2, FIX_0_765366865);     /* c2-c6 */
    tmp3 = z1 - MULTIPLY(z3, FIX_1_847759065);     /* c2+c6 */

    /* Add fudge factor here for final descale. */
    z2 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    z3 = (INT32) wsptr[4];

    tmp0 = (z2 + z3) << CONST_BITS;
    tmp1 = (z2 - z3) << CONST_BITS;

    tmp10 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;
    tmp11 = tmp1 + tmp3;
    tmp12 = tmp1 - tmp3;

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */

    tmp0 = (INT32) wsptr[7];
    tmp1 = (INT32) wsptr[5];
    tmp2 = (INT32) wsptr[3];
    tmp3 = (INT32) wsptr[1];

    z2 = tmp0 + tmp2;
    z3 = tmp1 + tmp3;

    z1 = MULTIPLY(z2 + z3, FIX_1_175875602);       /*  c3 */
    z2 = MULTIPLY(z2, - FIX_1_961570560);          /* -c3-c5 */
    z3 = MULTIPLY(z3, - FIX_0_390180644);          /* -c3+c5 */
    z2 += z1;
    z3 += z1;

    z1 = MULTIPLY(tmp0 + tmp3, - FIX_0_899976223); /* -c3+c7 */
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);        /* -c1+c3+c5-c7 */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);        /*  c1+c3-c5-c7 */
    tmp0 += z1 + z2;
    tmp3 += z1 + z3;

    z1 = MULTIPLY(tmp1 + tmp2, - FIX_2_562915447); /* -c1-c3 */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);        /*  c1+c3-c5+c7 */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);        /*  c1+c3+c5-c7 */
    tmp1 += z1 + z3;
    tmp2 += z1 + z2;

    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[7] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp3,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp11 + tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) RIGHT_SHIFT(tmp11 - tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12 + tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp12 - tmp1,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp13 + tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp13 - tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += DCTSIZE;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 7x14 output block.
 *
 * 14-point IDCT in pass 1 (columns), 7-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_7x14 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[7*14];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 14-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/28).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 7; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z1 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z1 += ONE << (CONST_BITS-PASS1_BITS-1);
    z4 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z2 = MULTIPLY(z4, FIX(1.274162392));         /* c4 */
    z3 = MULTIPLY(z4, FIX(0.314692123));         /* c12 */
    z4 = MULTIPLY(z4, FIX(0.881747734));         /* c8 */

    tmp10 = z1 + z2;
    tmp11 = z1 + z3;
    tmp12 = z1 - z4;

    tmp23 = RIGHT_SHIFT(z1 - ((z2 + z3 - z4) << 1), /* c0 = (c4+c12-c8)*2 */
			CONST_BITS-PASS1_BITS);

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    z3 = MULTIPLY(z1 + z2, FIX(1.105676686));    /* c6 */

    tmp13 = z3 + MULTIPLY(z1, FIX(0.273079590)); /* c2-c6 */
    tmp14 = z3 - MULTIPLY(z2, FIX(1.719280954)); /* c6+c10 */
    tmp15 = MULTIPLY(z1, FIX(0.613604268)) -     /* c10 */
	    MULTIPLY(z2, FIX(1.378756276));      /* c2 */

    tmp20 = tmp10 + tmp13;
    tmp26 = tmp10 - tmp13;
    tmp21 = tmp11 + tmp14;
    tmp25 = tmp11 - tmp14;
    tmp22 = tmp12 + tmp15;
    tmp24 = tmp12 - tmp15;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
    tmp13 = z4 << CONST_BITS;

    tmp14 = z1 + z3;
    tmp11 = MULTIPLY(z1 + z2, FIX(1.334852607));           /* c3 */
    tmp12 = MULTIPLY(tmp14, FIX(1.197448846));             /* c5 */
    tmp10 = tmp11 + tmp12 + tmp13 - MULTIPLY(z1, FIX(1.126980169)); /* c3+c5-c1 */
    tmp14 = MULTIPLY(tmp14, FIX(0.752406978));             /* c9 */
    tmp16 = tmp14 - MULTIPLY(z1, FIX(1.061150426));        /* c9+c11-c13 */
    z1    -= z2;
    tmp15 = MULTIPLY(z1, FIX(0.467085129)) - tmp13;        /* c11 */
    tmp16 += tmp15;
    z1    += z4;
    z4    = MULTIPLY(z2 + z3, - FIX(0.158341681)) - tmp13; /* -c13 */
    tmp11 += z4 - MULTIPLY(z2, FIX(0.424103948));          /* c3-c9-c13 */
    tmp12 += z4 - MULTIPLY(z3, FIX(2.373959773));          /* c3+c5-c13 */
    z4    = MULTIPLY(z3 - z2, FIX(1.405321284));           /* c1 */
    tmp14 += z4 + tmp13 - MULTIPLY(z3, FIX(1.6906431334)); /* c1+c9-c11 */
    tmp15 += z4 + MULTIPLY(z2, FIX(0.674957567));          /* c1+c11-c5 */

    tmp13 = (z1 - z3) << PASS1_BITS;

    /* Final output stage */

    wsptr[7*0]  = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[7*13] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[7*1]  = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[7*12] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[7*2]  = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[7*11] = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[7*3]  = (int) (tmp23 + tmp13);
    wsptr[7*10] = (int) (tmp23 - tmp13);
    wsptr[7*4]  = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[7*9]  = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[7*5]  = (int) RIGHT_SHIFT(tmp25 + tmp15, CONST_BITS-PASS1_BITS);
    wsptr[7*8]  = (int) RIGHT_SHIFT(tmp25 - tmp15, CONST_BITS-PASS1_BITS);
    wsptr[7*6]  = (int) RIGHT_SHIFT(tmp26 + tmp16, CONST_BITS-PASS1_BITS);
    wsptr[7*7]  = (int) RIGHT_SHIFT(tmp26 - tmp16, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 14 rows from work array, store into output array.
   * 7-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/14).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 14; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp23 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp23 <<= CONST_BITS;

    z1 = (INT32) wsptr[2];
    z2 = (INT32) wsptr[4];
    z3 = (INT32) wsptr[6];

    tmp20 = MULTIPLY(z2 - z3, FIX(0.881747734));       /* c4 */
    tmp22 = MULTIPLY(z1 - z2, FIX(0.314692123));       /* c6 */
    tmp21 = tmp20 + tmp22 + tmp23 - MULTIPLY(z2, FIX(1.841218003)); /* c2+c4-c6 */
    tmp10 = z1 + z3;
    z2 -= tmp10;
    tmp10 = MULTIPLY(tmp10, FIX(1.274162392)) + tmp23; /* c2 */
    tmp20 += tmp10 - MULTIPLY(z3, FIX(0.077722536));   /* c2-c4-c6 */
    tmp22 += tmp10 - MULTIPLY(z1, FIX(2.470602249));   /* c2+c4+c6 */
    tmp23 += MULTIPLY(z2, FIX(1.414213562));           /* c0 */

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];

    tmp11 = MULTIPLY(z1 + z2, FIX(0.935414347));       /* (c3+c1-c5)/2 */
    tmp12 = MULTIPLY(z1 - z2, FIX(0.170262339));       /* (c3+c5-c1)/2 */
    tmp10 = tmp11 - tmp12;
    tmp11 += tmp12;
    tmp12 = MULTIPLY(z2 + z3, - FIX(1.378756276));     /* -c1 */
    tmp11 += tmp12;
    z2 = MULTIPLY(z1 + z3, FIX(0.613604268));          /* c5 */
    tmp10 += z2;
    tmp12 += z2 + MULTIPLY(z3, FIX(1.870828693));      /* c3+c1-c5 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[6] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp23,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 7;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 6x12 output block.
 *
 * 12-point IDCT in pass 1 (columns), 6-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_6x12 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24, tmp25;
  INT32 z1, z2, z3, z4;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[6*12];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 12-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/24).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 6; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z3 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z3 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z3 += ONE << (CONST_BITS-PASS1_BITS-1);

    z4 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z4 = MULTIPLY(z4, FIX(1.224744871)); /* c4 */

    tmp10 = z3 + z4;
    tmp11 = z3 - z4;

    z1 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z4 = MULTIPLY(z1, FIX(1.366025404)); /* c2 */
    z1 <<= CONST_BITS;
    z2 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);
    z2 <<= CONST_BITS;

    tmp12 = z1 - z2;

    tmp21 = z3 + tmp12;
    tmp24 = z3 - tmp12;

    tmp12 = z4 + z2;

    tmp20 = tmp10 + tmp12;
    tmp25 = tmp10 - tmp12;

    tmp12 = z4 - z1 - z2;

    tmp22 = tmp11 + tmp12;
    tmp23 = tmp11 - tmp12;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = MULTIPLY(z2, FIX(1.306562965));                  /* c3 */
    tmp14 = MULTIPLY(z2, - FIX_0_541196100);                 /* -c9 */

    tmp10 = z1 + z3;
    tmp15 = MULTIPLY(tmp10 + z4, FIX(0.860918669));          /* c7 */
    tmp12 = tmp15 + MULTIPLY(tmp10, FIX(0.261052384));       /* c5-c7 */
    tmp10 = tmp12 + tmp11 + MULTIPLY(z1, FIX(0.280143716));  /* c1-c5 */
    tmp13 = MULTIPLY(z3 + z4, - FIX(1.045510580));           /* -(c7+c11) */
    tmp12 += tmp13 + tmp14 - MULTIPLY(z3, FIX(1.478575242)); /* c1+c5-c7-c11 */
    tmp13 += tmp15 - tmp11 + MULTIPLY(z4, FIX(1.586706681)); /* c1+c11 */
    tmp15 += tmp14 - MULTIPLY(z1, FIX(0.676326758)) -        /* c7-c11 */
	     MULTIPLY(z4, FIX(1.982889723));                 /* c5+c7 */

    z1 -= z4;
    z2 -= z3;
    z3 = MULTIPLY(z1 + z2, FIX_0_541196100);                 /* c9 */
    tmp11 = z3 + MULTIPLY(z1, FIX_0_765366865);              /* c3-c9 */
    tmp14 = z3 - MULTIPLY(z2, FIX_1_847759065);              /* c3+c9 */

    /* Final output stage */

    wsptr[6*0]  = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[6*11] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[6*1]  = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[6*10] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[6*2]  = (int) RIGHT_SHIFT(tmp22 + tmp12, CONST_BITS-PASS1_BITS);
    wsptr[6*9]  = (int) RIGHT_SHIFT(tmp22 - tmp12, CONST_BITS-PASS1_BITS);
    wsptr[6*3]  = (int) RIGHT_SHIFT(tmp23 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[6*8]  = (int) RIGHT_SHIFT(tmp23 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[6*4]  = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[6*7]  = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
    wsptr[6*5]  = (int) RIGHT_SHIFT(tmp25 + tmp15, CONST_BITS-PASS1_BITS);
    wsptr[6*6]  = (int) RIGHT_SHIFT(tmp25 - tmp15, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 12 rows from work array, store into output array.
   * 6-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/12).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 12; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp10 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp10 <<= CONST_BITS;
    tmp12 = (INT32) wsptr[4];
    tmp20 = MULTIPLY(tmp12, FIX(0.707106781));   /* c4 */
    tmp11 = tmp10 + tmp20;
    tmp21 = tmp10 - tmp20 - tmp20;
    tmp20 = (INT32) wsptr[2];
    tmp10 = MULTIPLY(tmp20, FIX(1.224744871));   /* c2 */
    tmp20 = tmp11 + tmp10;
    tmp22 = tmp11 - tmp10;

    /* Odd part */

    z1 = (INT32) wsptr[1];
    z2 = (INT32) wsptr[3];
    z3 = (INT32) wsptr[5];
    tmp11 = MULTIPLY(z1 + z3, FIX(0.366025404)); /* c5 */
    tmp10 = tmp11 + ((z1 + z2) << CONST_BITS);
    tmp12 = tmp11 + ((z3 - z2) << CONST_BITS);
    tmp11 = (z1 - z2 - z3) << CONST_BITS;

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp20 + tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[5] = range_limit[(int) RIGHT_SHIFT(tmp20 - tmp10,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp21 + tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp21 - tmp11,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp22 + tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp22 - tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 6;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 5x10 output block.
 *
 * 10-point IDCT in pass 1 (columns), 5-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_5x10 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
		JCOEFPTR coef_block,
		JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp10, tmp11, tmp12, tmp13, tmp14;
  INT32 tmp20, tmp21, tmp22, tmp23, tmp24;
  INT32 z1, z2, z3, z4, z5;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[5*10];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 10-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/20).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 5; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    z3 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z3 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z3 += ONE << (CONST_BITS-PASS1_BITS-1);
    z4 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z1 = MULTIPLY(z4, FIX(1.144122806));         /* c4 */
    z2 = MULTIPLY(z4, FIX(0.437016024));         /* c8 */
    tmp10 = z3 + z1;
    tmp11 = z3 - z2;

    tmp22 = RIGHT_SHIFT(z3 - ((z1 - z2) << 1),   /* c0 = (c4-c8)*2 */
			CONST_BITS-PASS1_BITS);

    z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    z1 = MULTIPLY(z2 + z3, FIX(0.831253876));    /* c6 */
    tmp12 = z1 + MULTIPLY(z2, FIX(0.513743148)); /* c2-c6 */
    tmp13 = z1 - MULTIPLY(z3, FIX(2.176250899)); /* c2+c6 */

    tmp20 = tmp10 + tmp12;
    tmp24 = tmp10 - tmp12;
    tmp21 = tmp11 + tmp13;
    tmp23 = tmp11 - tmp13;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    z4 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);

    tmp11 = z2 + z4;
    tmp13 = z2 - z4;

    tmp12 = MULTIPLY(tmp13, FIX(0.309016994));        /* (c3-c7)/2 */
    z5 = z3 << CONST_BITS;

    z2 = MULTIPLY(tmp11, FIX(0.951056516));           /* (c3+c7)/2 */
    z4 = z5 + tmp12;

    tmp10 = MULTIPLY(z1, FIX(1.396802247)) + z2 + z4; /* c1 */
    tmp14 = MULTIPLY(z1, FIX(0.221231742)) - z2 + z4; /* c9 */

    z2 = MULTIPLY(tmp11, FIX(0.587785252));           /* (c1-c9)/2 */
    z4 = z5 - tmp12 - (tmp13 << (CONST_BITS - 1));

    tmp12 = (z1 - tmp13 - z3) << PASS1_BITS;

    tmp11 = MULTIPLY(z1, FIX(1.260073511)) - z2 - z4; /* c3 */
    tmp13 = MULTIPLY(z1, FIX(0.642039522)) - z2 + z4; /* c7 */

    /* Final output stage */

    wsptr[5*0] = (int) RIGHT_SHIFT(tmp20 + tmp10, CONST_BITS-PASS1_BITS);
    wsptr[5*9] = (int) RIGHT_SHIFT(tmp20 - tmp10, CONST_BITS-PASS1_BITS);
    wsptr[5*1] = (int) RIGHT_SHIFT(tmp21 + tmp11, CONST_BITS-PASS1_BITS);
    wsptr[5*8] = (int) RIGHT_SHIFT(tmp21 - tmp11, CONST_BITS-PASS1_BITS);
    wsptr[5*2] = (int) (tmp22 + tmp12);
    wsptr[5*7] = (int) (tmp22 - tmp12);
    wsptr[5*3] = (int) RIGHT_SHIFT(tmp23 + tmp13, CONST_BITS-PASS1_BITS);
    wsptr[5*6] = (int) RIGHT_SHIFT(tmp23 - tmp13, CONST_BITS-PASS1_BITS);
    wsptr[5*4] = (int) RIGHT_SHIFT(tmp24 + tmp14, CONST_BITS-PASS1_BITS);
    wsptr[5*5] = (int) RIGHT_SHIFT(tmp24 - tmp14, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 10 rows from work array, store into output array.
   * 5-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/10).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 10; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp12 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp12 <<= CONST_BITS;
    tmp13 = (INT32) wsptr[2];
    tmp14 = (INT32) wsptr[4];
    z1 = MULTIPLY(tmp13 + tmp14, FIX(0.790569415)); /* (c2+c4)/2 */
    z2 = MULTIPLY(tmp13 - tmp14, FIX(0.353553391)); /* (c2-c4)/2 */
    z3 = tmp12 + z2;
    tmp10 = z3 + z1;
    tmp11 = z3 - z1;
    tmp12 -= z2 << 2;

    /* Odd part */

    z2 = (INT32) wsptr[1];
    z3 = (INT32) wsptr[3];

    z1 = MULTIPLY(z2 + z3, FIX(0.831253876));       /* c3 */
    tmp13 = z1 + MULTIPLY(z2, FIX(0.513743148));    /* c1-c3 */
    tmp14 = z1 - MULTIPLY(z3, FIX(2.176250899));    /* c1+c3 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp13,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[4] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp13,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp11 + tmp14,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp11 - tmp14,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 5;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 4x8 output block.
 *
 * 8-point IDCT in pass 1 (columns), 4-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_4x8 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp3;
  INT32 tmp10, tmp11, tmp12, tmp13;
  INT32 z1, z2, z3;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[4*8];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * Note results are scaled up by sqrt(8) compared to a true IDCT;
   * furthermore, we scale the results by 2**PASS1_BITS.
   * 8-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/16).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 4; ctr > 0; ctr--) {
    /* Due to quantization, we will usually find that many of the input
     * coefficients are zero, especially the AC terms.  We can exploit this
     * by short-circuiting the IDCT calculation for any column in which all
     * the AC terms are zero.  In that case each output is equal to the
     * DC coefficient (with scale factor as needed).
     * With typical images and quantization tables, half or more of the
     * column DCT calculations can be simplified this way.
     */

    if (inptr[DCTSIZE*1] == 0 && inptr[DCTSIZE*2] == 0 &&
	inptr[DCTSIZE*3] == 0 && inptr[DCTSIZE*4] == 0 &&
	inptr[DCTSIZE*5] == 0 && inptr[DCTSIZE*6] == 0 &&
	inptr[DCTSIZE*7] == 0) {
      /* AC terms all zero */
      int dcval = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]) << PASS1_BITS;

      wsptr[4*0] = dcval;
      wsptr[4*1] = dcval;
      wsptr[4*2] = dcval;
      wsptr[4*3] = dcval;
      wsptr[4*4] = dcval;
      wsptr[4*5] = dcval;
      wsptr[4*6] = dcval;
      wsptr[4*7] = dcval;

      inptr++;			/* advance pointers to next column */
      quantptr++;
      wsptr++;
      continue;
    }

    /* Even part: reverse the even part of the forward DCT.
     * The rotator is c(-6).
     */

    z2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*6], quantptr[DCTSIZE*6]);

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);       /* c6 */
    tmp2 = z1 + MULTIPLY(z2, FIX_0_765366865);     /* c2-c6 */
    tmp3 = z1 - MULTIPLY(z3, FIX_1_847759065);     /* c2+c6 */

    z2 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    z2 <<= CONST_BITS;
    z3 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    z2 += ONE << (CONST_BITS-PASS1_BITS-1);

    tmp0 = z2 + z3;
    tmp1 = z2 - z3;

    tmp10 = tmp0 + tmp2;
    tmp13 = tmp0 - tmp2;
    tmp11 = tmp1 + tmp3;
    tmp12 = tmp1 - tmp3;

    /* Odd part per figure 8; the matrix is unitary and hence its
     * transpose is its inverse.  i0..i3 are y7,y5,y3,y1 respectively.
     */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*7], quantptr[DCTSIZE*7]);
    tmp1 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    tmp3 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);

    z2 = tmp0 + tmp2;
    z3 = tmp1 + tmp3;

    z1 = MULTIPLY(z2 + z3, FIX_1_175875602);       /*  c3 */
    z2 = MULTIPLY(z2, - FIX_1_961570560);          /* -c3-c5 */
    z3 = MULTIPLY(z3, - FIX_0_390180644);          /* -c3+c5 */
    z2 += z1;
    z3 += z1;

    z1 = MULTIPLY(tmp0 + tmp3, - FIX_0_899976223); /* -c3+c7 */
    tmp0 = MULTIPLY(tmp0, FIX_0_298631336);        /* -c1+c3+c5-c7 */
    tmp3 = MULTIPLY(tmp3, FIX_1_501321110);        /*  c1+c3-c5-c7 */
    tmp0 += z1 + z2;
    tmp3 += z1 + z3;

    z1 = MULTIPLY(tmp1 + tmp2, - FIX_2_562915447); /* -c1-c3 */
    tmp1 = MULTIPLY(tmp1, FIX_2_053119869);        /*  c1+c3-c5+c7 */
    tmp2 = MULTIPLY(tmp2, FIX_3_072711026);        /*  c1+c3+c5-c7 */
    tmp1 += z1 + z3;
    tmp2 += z1 + z2;

    /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

    wsptr[4*0] = (int) RIGHT_SHIFT(tmp10 + tmp3, CONST_BITS-PASS1_BITS);
    wsptr[4*7] = (int) RIGHT_SHIFT(tmp10 - tmp3, CONST_BITS-PASS1_BITS);
    wsptr[4*1] = (int) RIGHT_SHIFT(tmp11 + tmp2, CONST_BITS-PASS1_BITS);
    wsptr[4*6] = (int) RIGHT_SHIFT(tmp11 - tmp2, CONST_BITS-PASS1_BITS);
    wsptr[4*2] = (int) RIGHT_SHIFT(tmp12 + tmp1, CONST_BITS-PASS1_BITS);
    wsptr[4*5] = (int) RIGHT_SHIFT(tmp12 - tmp1, CONST_BITS-PASS1_BITS);
    wsptr[4*3] = (int) RIGHT_SHIFT(tmp13 + tmp0, CONST_BITS-PASS1_BITS);
    wsptr[4*4] = (int) RIGHT_SHIFT(tmp13 - tmp0, CONST_BITS-PASS1_BITS);

    inptr++;			/* advance pointers to next column */
    quantptr++;
    wsptr++;
  }

  /* Pass 2: process 8 rows from work array, store into output array.
   * 4-point IDCT kernel,
   * cK represents sqrt(2) * cos(K*pi/16) [refers to 8-point IDCT].
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 8; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp0 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp2 = (INT32) wsptr[2];

    tmp10 = (tmp0 + tmp2) << CONST_BITS;
    tmp12 = (tmp0 - tmp2) << CONST_BITS;

    /* Odd part */
    /* Same rotation as in the even part of the 8x8 LL&M IDCT */

    z2 = (INT32) wsptr[1];
    z3 = (INT32) wsptr[3];

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);   /* c6 */
    tmp0 = z1 + MULTIPLY(z2, FIX_0_765366865); /* c2-c6 */
    tmp2 = z1 - MULTIPLY(z3, FIX_1_847759065); /* c2+c6 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[3] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp12 + tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp12 - tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 4;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a reduced-size 3x6 output block.
 *
 * 6-point IDCT in pass 1 (columns), 3-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_3x6 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1, tmp2, tmp10, tmp11, tmp12;
  INT32 z1, z2, z3;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  int * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  int workspace[3*6];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 6-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/12).
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 3; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp0 <<= CONST_BITS;
    /* Add fudge factor here for final descale. */
    tmp0 += ONE << (CONST_BITS-PASS1_BITS-1);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*4], quantptr[DCTSIZE*4]);
    tmp10 = MULTIPLY(tmp2, FIX(0.707106781));   /* c4 */
    tmp1 = tmp0 + tmp10;
    tmp11 = RIGHT_SHIFT(tmp0 - tmp10 - tmp10, CONST_BITS-PASS1_BITS);
    tmp10 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);
    tmp0 = MULTIPLY(tmp10, FIX(1.224744871));   /* c2 */
    tmp10 = tmp1 + tmp0;
    tmp12 = tmp1 - tmp0;

    /* Odd part */

    z1 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z2 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*5], quantptr[DCTSIZE*5]);
    tmp1 = MULTIPLY(z1 + z3, FIX(0.366025404)); /* c5 */
    tmp0 = tmp1 + ((z1 + z2) << CONST_BITS);
    tmp2 = tmp1 + ((z3 - z2) << CONST_BITS);
    tmp1 = (z1 - z2 - z3) << PASS1_BITS;

    /* Final output stage */

    wsptr[3*0] = (int) RIGHT_SHIFT(tmp10 + tmp0, CONST_BITS-PASS1_BITS);
    wsptr[3*5] = (int) RIGHT_SHIFT(tmp10 - tmp0, CONST_BITS-PASS1_BITS);
    wsptr[3*1] = (int) (tmp11 + tmp1);
    wsptr[3*4] = (int) (tmp11 - tmp1);
    wsptr[3*2] = (int) RIGHT_SHIFT(tmp12 + tmp2, CONST_BITS-PASS1_BITS);
    wsptr[3*3] = (int) RIGHT_SHIFT(tmp12 - tmp2, CONST_BITS-PASS1_BITS);
  }

  /* Pass 2: process 6 rows from work array, store into output array.
   * 3-point IDCT kernel, cK represents sqrt(2) * cos(K*pi/6).
   */

  wsptr = workspace;
  for (ctr = 0; ctr < 6; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp0 = (INT32) wsptr[0] + (ONE << (PASS1_BITS+2));
    tmp0 <<= CONST_BITS;
    tmp2 = (INT32) wsptr[2];
    tmp12 = MULTIPLY(tmp2, FIX(0.707106781)); /* c2 */
    tmp10 = tmp0 + tmp12;
    tmp2 = tmp0 - tmp12 - tmp12;

    /* Odd part */

    tmp12 = (INT32) wsptr[1];
    tmp0 = MULTIPLY(tmp12, FIX(1.224744871)); /* c1 */

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[2] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp0,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp2,
					      CONST_BITS+PASS1_BITS+3)
			    & RANGE_MASK];

    wsptr += 3;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 2x4 output block.
 *
 * 4-point IDCT in pass 1 (columns), 2-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_2x4 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp2, tmp10, tmp12;
  INT32 z1, z2, z3;
  JCOEFPTR inptr;
  ISLOW_MULT_TYPE * quantptr;
  INT32 * wsptr;
  JSAMPROW outptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  int ctr;
  INT32 workspace[2*4];	/* buffers data between passes */
  SHIFT_TEMPS

  /* Pass 1: process columns from input, store into work array.
   * 4-point IDCT kernel,
   * cK represents sqrt(2) * cos(K*pi/16) [refers to 8-point IDCT].
   */

  inptr = coef_block;
  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;
  wsptr = workspace;
  for (ctr = 0; ctr < 2; ctr++, inptr++, quantptr++, wsptr++) {
    /* Even part */

    tmp0 = DEQUANTIZE(inptr[DCTSIZE*0], quantptr[DCTSIZE*0]);
    tmp2 = DEQUANTIZE(inptr[DCTSIZE*2], quantptr[DCTSIZE*2]);

    tmp10 = (tmp0 + tmp2) << CONST_BITS;
    tmp12 = (tmp0 - tmp2) << CONST_BITS;

    /* Odd part */
    /* Same rotation as in the even part of the 8x8 LL&M IDCT */

    z2 = DEQUANTIZE(inptr[DCTSIZE*1], quantptr[DCTSIZE*1]);
    z3 = DEQUANTIZE(inptr[DCTSIZE*3], quantptr[DCTSIZE*3]);

    z1 = MULTIPLY(z2 + z3, FIX_0_541196100);   /* c6 */
    tmp0 = z1 + MULTIPLY(z2, FIX_0_765366865); /* c2-c6 */
    tmp2 = z1 - MULTIPLY(z3, FIX_1_847759065); /* c2+c6 */

    /* Final output stage */

    wsptr[2*0] = tmp10 + tmp0;
    wsptr[2*3] = tmp10 - tmp0;
    wsptr[2*1] = tmp12 + tmp2;
    wsptr[2*2] = tmp12 - tmp2;
  }

  /* Pass 2: process 4 rows from work array, store into output array. */

  wsptr = workspace;
  for (ctr = 0; ctr < 4; ctr++) {
    outptr = output_buf[ctr] + output_col;

    /* Even part */

    /* Add fudge factor here for final descale. */
    tmp10 = wsptr[0] + (ONE << (CONST_BITS+2));

    /* Odd part */

    tmp0 = wsptr[1];

    /* Final output stage */

    outptr[0] = range_limit[(int) RIGHT_SHIFT(tmp10 + tmp0, CONST_BITS+3)
			    & RANGE_MASK];
    outptr[1] = range_limit[(int) RIGHT_SHIFT(tmp10 - tmp0, CONST_BITS+3)
			    & RANGE_MASK];

    wsptr += 2;		/* advance pointer to next row */
  }
}


/*
 * Perform dequantization and inverse DCT on one block of coefficients,
 * producing a 1x2 output block.
 *
 * 2-point IDCT in pass 1 (columns), 1-point in pass 2 (rows).
 */

GLOBAL(void)
jpeg_idct_1x2 (j_decompress_ptr cinfo, jpeg_component_info * compptr,
	       JCOEFPTR coef_block,
	       JSAMPARRAY output_buf, JDIMENSION output_col)
{
  INT32 tmp0, tmp1;
  ISLOW_MULT_TYPE * quantptr;
  JSAMPLE *range_limit = IDCT_range_limit(cinfo);
  SHIFT_TEMPS

  /* Process 1 column from input, store into output array. */

  quantptr = (ISLOW_MULT_TYPE *) compptr->dct_table;

  /* Even part */
    
  tmp0 = DEQUANTIZE(coef_block[DCTSIZE*0], quantptr[DCTSIZE*0]);
  /* Add fudge factor here for final descale. */
  tmp0 += ONE << 2;

  /* Odd part */

  tmp1 = DEQUANTIZE(coef_block[DCTSIZE*1], quantptr[DCTSIZE*1]);

  /* Final output stage */

  output_buf[0][output_col] = range_limit[(int) RIGHT_SHIFT(tmp0 + tmp1, 3)
					  & RANGE_MASK];
  output_buf[1][output_col] = range_limit[(int) RIGHT_SHIFT(tmp0 - tmp1, 3)
					  & RANGE_MASK];
}

#endif /* IDCT_SCALING_SUPPORTED */
#endif /* DCT_ISLOW_SUPPORTED */
