/*
 * jmemmgr.c
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * Modified 2011-2012 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains the JPEG system-independent memory management
 * routines.  This code is usable across a wide variety of machines; most
 * of the system dependencies have been isolated in a separate file.
 * The major functions provided here are:
 *   * pool-based allocation and freeing of memory;
 *   * policy decisions about how to divide available memory among the
 *     virtual arrays;
 *   * control logic for swapping virtual arrays between main memory and
 *     backing storage.
 * The separate system-dependent file provides the actual backing-storage
 * access code, and it contains the policy decision about how much total
 * main memory to use.
 * This file is system-dependent in the sense that some of its functions
 * are unnecessary in some systems.  For example, if there is enough virtual
 * memory so that backing storage will never be used, much of the virtual
 * array control logic could be removed.  (Of course, if you have that much
 * memory then you shouldn't care about a little bit of unused code...)
 */

#define JPEG_INTERNALS
#define AM_MEMORY_MANAGER	/* we define jvirt_Xarray_control structs */
#include "jinclude.h"
#include "jpeglib.h"
#include "jmemsys.h"		/* import the system-dependent declarations */

#ifndef NO_GETENV
#ifndef HAVE_STDLIB_H		/* <stdlib.h> should declare getenv() */
extern char * getenv JPP((const char * name));
#endif
#endif


/*
 * Some important notes:
 *   The allocation routines provided here must never return NULL.
 *   They should exit to error_exit if unsuccessful.
 *
 *   It's not a good idea to try to merge the sarray and barray routines,
 *   even though they are textually almost the same, because samples are
 *   usually stored as bytes while coefficients are shorts or ints.  Thus,
 *   in machines where byte pointers have a different representation from
 *   word pointers, the resulting machine code could not be the same.
 */


/*
 * Many machines require storage alignment: longs must start on 4-byte
 * boundaries, doubles on 8-byte boundaries, etc.  On such machines, malloc()
 * always returns pointers that are multiples of the worst-case alignment
 * requirement, and we had better do so too.
 * There isn't any really portable way to determine the worst-case alignment
 * requirement.  This module assumes that the alignment requirement is
 * multiples of sizeof(ALIGN_TYPE).
 * By default, we define ALIGN_TYPE as double.  This is necessary on some
 * workstations (where doubles really do need 8-byte alignment) and will work
 * fine on nearly everything.  If your machine has lesser alignment needs,
 * you can save a few bytes by making ALIGN_TYPE smaller.
 * The only place I know of where this will NOT work is certain Macintosh
 * 680x0 compilers that define double as a 10-byte IEEE extended float.
 * Doing 10-byte alignment is counterproductive because longwords won't be
 * aligned well.  Put "#define ALIGN_TYPE long" in jconfig.h if you have
 * such a compiler.
 */

#ifndef ALIGN_TYPE		/* so can override from jconfig.h */
#define ALIGN_TYPE  double
#endif


/*
 * We allocate objects from "pools", where each pool is gotten with a single
 * request to jpeg_get_small() or jpeg_get_large().  There is no per-object
 * overhead within a pool, except for alignment padding.  Each pool has a
 * header with a link to the next pool of the same class.
 * Small and large pool headers are identical except that the latter's
 * link pointer must be FAR on 80x86 machines.
 * Notice that the "real" header fields are union'ed with a dummy ALIGN_TYPE
 * field.  This forces the compiler to make SIZEOF(small_pool_hdr) a multiple
 * of the alignment requirement of ALIGN_TYPE.
 */

typedef union small_pool_struct * small_pool_ptr;

typedef union small_pool_struct {
  struct {
    small_pool_ptr next;	/* next in list of pools */
    size_t bytes_used;		/* how many bytes already used within pool */
    size_t bytes_left;		/* bytes still available in this pool */
  } hdr;
  ALIGN_TYPE dummy;		/* included in union to ensure alignment */
} small_pool_hdr;

typedef union large_pool_struct FAR * large_pool_ptr;

typedef union large_pool_struct {
  struct {
    large_pool_ptr next;	/* next in list of pools */
    size_t bytes_used;		/* how many bytes already used within pool */
    size_t bytes_left;		/* bytes still available in this pool */
  } hdr;
  ALIGN_TYPE dummy;		/* included in union to ensure alignment */
} large_pool_hdr;


/*
 * Here is the full definition of a memory manager object.
 */

typedef struct {
  struct jpeg_memory_mgr pub;	/* public fields */

  /* Each pool identifier (lifetime class) names a linked list of pools. */
  small_pool_ptr small_list[JPOOL_NUMPOOLS];
  large_pool_ptr large_list[JPOOL_NUMPOOLS];

  /* Since we only have one lifetime class of virtual arrays, only one
   * linked list is necessary (for each datatype).  Note that the virtual
   * array control blocks being linked together are actually stored somewhere
   * in the small-pool list.
   */
  jvirt_sarray_ptr virt_sarray_list;
  jvirt_barray_ptr virt_barray_list;

  /* This counts total space obtained from jpeg_get_small/large */
  long total_space_allocated;

  /* alloc_sarray and alloc_barray set this value for use by virtual
   * array routines.
   */
  JDIMENSION last_rowsperchunk;	/* from most recent alloc_sarray/barray */
} my_memory_mgr;

typedef my_memory_mgr * my_mem_ptr;


/*
 * The control blocks for virtual arrays.
 * Note that these blocks are allocated in the "small" pool area.
 * System-dependent info for the associated backing store (if any) is hidden
 * inside the backing_store_info struct.
 */

struct jvirt_sarray_control {
  JSAMPARRAY mem_buffer;	/* => the in-memory buffer */
  JDIMENSION rows_in_array;	/* total virtual array height */
  JDIMENSION samplesperrow;	/* width of array (and of memory buffer) */
  JDIMENSION maxaccess;		/* max rows accessed by access_virt_sarray */
  JDIMENSION rows_in_mem;	/* height of memory buffer */
  JDIMENSION rowsperchunk;	/* allocation chunk size in mem_buffer */
  JDIMENSION cur_start_row;	/* first logical row # in the buffer */
  JDIMENSION first_undef_row;	/* row # of first uninitialized row */
  boolean pre_zero;		/* pre-zero mode requested? */
  boolean dirty;		/* do current buffer contents need written? */
  boolean b_s_open;		/* is backing-store data valid? */
  jvirt_sarray_ptr next;	/* link to next virtual sarray control block */
  backing_store_info b_s_info;	/* System-dependent control info */
};

struct jvirt_barray_control {
  JBLOCKARRAY mem_buffer;	/* => the in-memory buffer */
  JDIMENSION rows_in_array;	/* total virtual array height */
  JDIMENSION blocksperrow;	/* width of array (and of memory buffer) */
  JDIMENSION maxaccess;		/* max rows accessed by access_virt_barray */
  JDIMENSION rows_in_mem;	/* height of memory buffer */
  JDIMENSION rowsperchunk;	/* allocation chunk size in mem_buffer */
  JDIMENSION cur_start_row;	/* first logical row # in the buffer */
  JDIMENSION first_undef_row;	/* row # of first uninitialized row */
  boolean pre_zero;		/* pre-zero mode requested? */
  boolean dirty;		/* do current buffer contents need written? */
  boolean b_s_open;		/* is backing-store data valid? */
  jvirt_barray_ptr next;	/* link to next virtual barray control block */
  backing_store_info b_s_info;	/* System-dependent control info */
};


#ifdef MEM_STATS		/* optional extra stuff for statistics */

LOCAL(void)
print_mem_stats (j_common_ptr cinfo, int pool_id)
{
  my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
  small_pool_ptr shdr_ptr;
  large_pool_ptr lhdr_ptr;

  /* Since this is only a debugging stub, we can cheat a little by using
   * fprintf directly rather than going through the trace message code.
   * This is helpful because message parm array can't handle longs.
   */
  fprintf(stderr, "Freeing pool %d, total space = %ld\n",
	  pool_id, mem->total_space_allocated);

  for (lhdr_ptr = mem->large_list[pool_id]; lhdr_ptr != NULL;
       lhdr_ptr = lhdr_ptr->hdr.next) {
    fprintf(stderr, "  Large chunk used %ld\n",
	    (long) lhdr_ptr->hdr.bytes_used);
  }

  for (shdr_ptr = mem->small_list[pool_id]; shdr_ptr != NULL;
       shdr_ptr = shdr_ptr->hdr.next) {
    fprintf(stderr, "  Small chunk used %ld free %ld\n",
	    (long) shdr_ptr->hdr.bytes_used,
	    (long) shdr_ptr->hdr.bytes_left);
  }
}

#endif /* MEM_STATS */


LOCAL(noreturn_t)
out_of_memory (j_common_ptr cinfo, int which)
/* Report an out-of-memory error and stop execution */
/* If we compiled MEM_STATS support, report alloc requests before dying */
{
#ifdef MEM_STATS
  cinfo->err->trace_level = 2;	/* force self_destruct to report stats */
#endif
  ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, which);
}


/*
 * Allocation of "small" objects.
 *
 * For these, we use pooled storage.  When a new pool must be created,
 * we try to get enough space for the current request plus a "slop" factor,
 * where the slop will be the amount of leftover space in the new pool.
 * The speed vs. space tradeoff is largely determined by the slop values.
 * A different slop value is provided for each pool class (lifetime),
 * and we also distinguish the first pool of a class from later ones.
 * NOTE: the values given work fairly well on both 16- and 32-bit-int
 * machines, but may be too small if longs are 64 bits or more.
 */

static const size_t first_pool_slop[JPOOL_NUMPOOLS] = 
{
	1600,			/* first PERMANENT pool */
	16000			/* first IMAGE pool */
};

static const size_t extra_pool_slop[JPOOL_NUMPOOLS] = 
{
	0,			/* additional PERMANENT pools */
	5000			/* additional IMAGE pools */
};

#define MIN_SLOP  50		/* greater than 0 to avoid futile looping */


METHODDEF(void *)
alloc_small (j_common_ptr cinfo, int pool_id, size_t sizeofobject)
/* Allocate a "small" object */
{
  my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
  small_pool_ptr hdr_ptr, prev_hdr_ptr;
  char * data_ptr;
  size_t odd_bytes, min_request, slop;

  /* Check for unsatisfiable request (do now to ensure no overflow below) */
  if (sizeofobject > (size_t) (MAX_ALLOC_CHUNK-SIZEOF(small_pool_hdr)))
    out_of_memory(cinfo, 1);	/* request exceeds malloc's ability */

  /* Round up the requested size to a multiple of SIZEOF(ALIGN_TYPE) */
  odd_bytes = sizeofobject % SIZEOF(ALIGN_TYPE);
  if (odd_bytes > 0)
    sizeofobject += SIZEOF(ALIGN_TYPE) - odd_bytes;

  /* See if space is available in any existing pool */
  if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS)
    ERREXIT1(cinfo, JERR_BAD_POOL_ID, pool_id);	/* safety check */
  prev_hdr_ptr = NULL;
  hdr_ptr = mem->small_list[pool_id];
  while (hdr_ptr != NULL) {
    if (hdr_ptr->hdr.bytes_left >= sizeofobject)
      break;			/* found pool with enough space */
    prev_hdr_ptr = hdr_ptr;
    hdr_ptr = hdr_ptr->hdr.next;
  }

  /* Time to make a new pool? */
  if (hdr_ptr == NULL) {
    /* min_request is what we need now, slop is what will be leftover */
    min_request = sizeofobject + SIZEOF(small_pool_hdr);
    if (prev_hdr_ptr == NULL)	/* first pool in class? */
      slop = first_pool_slop[pool_id];
    else
      slop = extra_pool_slop[pool_id];
    /* Don't ask for more than MAX_ALLOC_CHUNK */
    if (slop > (size_t) (MAX_ALLOC_CHUNK-min_request))
      slop = (size_t) (MAX_ALLOC_CHUNK-min_request);
    /* Try to get space, if fail reduce slop and try again */
    for (;;) {
      hdr_ptr = (small_pool_ptr) jpeg_get_small(cinfo, min_request + slop);
      if (hdr_ptr != NULL)
	break;
      slop /= 2;
      if (slop < MIN_SLOP)	/* give up when it gets real small */
	out_of_memory(cinfo, 2); /* jpeg_get_small failed */
    }
    mem->total_space_allocated += min_request + slop;
    /* Success, initialize the new pool header and add to end of list */
    hdr_ptr->hdr.next = NULL;
    hdr_ptr->hdr.bytes_used = 0;
    hdr_ptr->hdr.bytes_left = sizeofobject + slop;
    if (prev_hdr_ptr == NULL)	/* first pool in class? */
      mem->small_list[pool_id] = hdr_ptr;
    else
      prev_hdr_ptr->hdr.next = hdr_ptr;
  }

  /* OK, allocate the object from the current pool */
  data_ptr = (char *) (hdr_ptr + 1); /* point to first data byte in pool */
  data_ptr += hdr_ptr->hdr.bytes_used; /* point to place for object */
  hdr_ptr->hdr.bytes_used += sizeofobject;
  hdr_ptr->hdr.bytes_left -= sizeofobject;

  return (void *) data_ptr;
}


/*
 * Allocation of "large" objects.
 *
 * The external semantics of these are the same as "small" objects,
 * except that FAR pointers are used on 80x86.  However the pool
 * management heuristics are quite different.  We assume that each
 * request is large enough that it may as well be passed directly to
 * jpeg_get_large; the pool management just links everything together
 * so that we can free it all on demand.
 * Note: the major use of "large" objects is in JSAMPARRAY and JBLOCKARRAY
 * structures.  The routines that create these structures (see below)
 * deliberately bunch rows together to ensure a large request size.
 */

METHODDEF(void FAR *)
alloc_large (j_common_ptr cinfo, int pool_id, size_t sizeofobject)
/* Allocate a "large" object */
{
  my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
  large_pool_ptr hdr_ptr;
  size_t odd_bytes;

  /* Check for unsatisfiable request (do now to ensure no overflow below) */
  if (sizeofobject > (size_t) (MAX_ALLOC_CHUNK-SIZEOF(large_pool_hdr)))
    out_of_memory(cinfo, 3);	/* request exceeds malloc's ability */

  /* Round up the requested size to a multiple of SIZEOF(ALIGN_TYPE) */
  odd_bytes = sizeofobject % SIZEOF(ALIGN_TYPE);
  if (odd_bytes > 0)
    sizeofobject += SIZEOF(ALIGN_TYPE) - odd_bytes;

  /* Always make a new pool */
  if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS)
    ERREXIT1(cinfo, JERR_BAD_POOL_ID, pool_id);	/* safety check */

  hdr_ptr = (large_pool_ptr) jpeg_get_large(cinfo, sizeofobject +
					    SIZEOF(large_pool_hdr));
  if (hdr_ptr == NULL)
    out_of_memory(cinfo, 4);	/* jpeg_get_large failed */
  mem->total_space_allocated += sizeofobject + SIZEOF(large_pool_hdr);

  /* Success, initialize the new pool header and add to list */
  hdr_ptr->hdr.next = mem->large_list[pool_id];
  /* We maintain space counts in each pool header for statistical purposes,
   * even though they are not needed for allocation.
   */
  hdr_ptr->hdr.bytes_used = sizeofobject;
  hdr_ptr->hdr.bytes_left = 0;
  mem->large_list[pool_id] = hdr_ptr;

  return (void FAR *) (hdr_ptr + 1); /* point to first data byte in pool */
}


/*
 * Creation of 2-D sample arrays.
 * The pointers are in near heap, the samples themselves in FAR heap.
 *
 * To minimize allocation overhead and to allow I/O of large contiguous
 * blocks, we allocate the sample rows in groups of as many rows as possible
 * without exceeding MAX_ALLOC_CHUNK total bytes per allocation request.
 * NB: the virtual array control routines, later in this file, know about
 * this chunking of rows.  The rowsperchunk value is left in the mem manager
 * object so that it can be saved away if this sarray is the workspace for
 * a virtual array.
 */

METHODDEF(JSAMPARRAY)
alloc_sarray (j_common_ptr cinfo, int pool_id,
	      JDIMENSION samplesperrow, JDIMENSION numrows)
/* Allocate a 2-D sample array */
{
  my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
  JSAMPARRAY result;
  JSAMPROW workspace;
  JDIMENSION rowsperchunk, currow, i;
  long ltemp;

  /* Calculate max # of rows allowed in one allocation chunk */
  ltemp = (MAX_ALLOC_CHUNK-SIZEOF(large_pool_hdr)) /
	  ((long) samplesperrow * SIZEOF(JSAMPLE));
  if (ltemp <= 0)
    ERREXIT(cinfo, JERR_WIDTH_OVERFLOW);
  if (ltemp < (long) numrows)
    rowsperchunk = (JDIMENSION) ltemp;
  else
    rowsperchunk = numrows;
  mem->last_rowsperchunk = rowsperchunk;

  /* Get space for row pointers (small object) */
  result = (JSAM¶º$rºP ’K      »$Ø Ì£  !  !  !    !     A    !             Ä        A´™™=          €C                            Ä⁄C                             =C                             fC                             LC                             JC                            Ä‹C                             fC                            ÄáC            	                 iC            
                 ÜC                             ZC                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ?f1(ΩP L      ∞!Ø Ì£  5  5  5    5     AxëÅ:5             ‡        A∏‘ê:           @                     ¿@   @  @@                     ‡A   @   @                      @   @  Ä?                     C   @   @            f         ¿@  Ä?  Ä?            —         `A  Ä?  Ä@            ‡         àA  Ä?  Ä?            Ú         †@  Ä?  Ä@            ¯         ¿@  Ä?  @@            ˇ         ‡@  ‡?  Ä@                    †@   @  Ä?                    @@  Ä?  @@                    Ä@   @  @@                    Ä@  Ä?  @@                    ¿A  Ä?  Ä?            ,        0A  Ä?  ¿@            8                ¿@            9        ¿@  ¿?  @@            >        @@  Ä?  @@            B        Ä@   @  Ä?            E        ‡@UU@  @@            I         @  Ä?  @@            L         A  Ä?  @@            U        ‡@  Ä?  @@            ]        pA  Ä?  Ä?            m        ‡@33≥?  @@            s        @@  Ä?  @@            w        ¿@  ¿?  @@            |        Ä@  Ä?  @@            Å        `AUUï?  @@            é        ¿@  ¿?  @@            ì        @Aöôô?  @@            û        pA  Ä?  Ä?            Æ        `A33≥?  @@            π        pA  Ä?  Ä?            …         A  Ä?  @@            ‘        †@  Ä?  @@            ⁄        PA—Eó?  @@            Ê        Ä@  Ä?  @@            Î         A  Ä?  @@            Ù        pA  Ä?  Ä?                    pA;±ì?  @@                    †@  Ä?  @@                    ‡@  Ä?  Ä?                     @A  Ä?  @@            -        pA  @@  @@            3        PA—Eó?  @@            ?        ¯A  Ä?  Ä?            _        pA  @@  Ä?            e        ‡@  Ä?  Ä?            m        PA  Ä?  @@            {        pA  Ä?  Ä?            ã        ÿAq=ä?  @@            •        pA  Ä?  Ä?            µ        †@UU’?  @@            π        Ä@   @  Ä?            º        ¿@  @@  @@            ø        ∏A  Ä?  Ä?            ◊        `AUUï?  @@            ‰        ¿@  ¿?  @A            È        †@  †@  ‡@            Î        @A  @A  @A            Ì        Ä@  Ä@  @A            Ô                @A                    Ä@  Ä@  ¿@            Ú         @   @  A            Ù        Ä@  Ä@  @A            ˆ        @A  @A  @A            ¯                 A            ˘        ¿@  ¿@  @A            ˚        pA  @@   @                    ¿@  @@  Ä@                    @@  @@  A                            @A                            ‡@                    Ä@  Ä@  †@            
        @@  @@  Ä@                     AUUU@  Ä@                    Ä@´™™?  @@                     @  Ä?  @A                     @  Ä?   A                    Ä?  Ä?  @@                    ‡@33≥?  Ä@            "        †@   @  @@            %        Ä@´™™?  @A            )        @A  @A  @A            +                @A            ,        C  @A  @A            :                @A            ;                @A            <        B  @A  @A            @                @A            A        B  @A  @A            E                @A            F        @A  @A  @A            H                @A            I                @A            J                @A            K        B  @A  @A            O                 A            P                @A            Q        Ä@  Ä@   A            S                @A            T        ‡@  `@  @@            W        †@   @  Ä@            Z        @A  Ä@  Ä@            ^        ‡@  `@  ‡@            a         A  Ä@  @@            d         A  Ä@  Ä?            g        @@  Ä?  @A            k        Ä?  Ä?  @A            m        ¿@  @@  @A            p        ‡@  `@  @A            s        1D  @A  @A            Ø                @A            ∞        |C  @A  @A            ∆        †A  Ä@  Ä@            Ã        @A  Ä@  Ä?            –        ‡@UU@  @@            ‘        @A  @A  @A            ÷                @A            ◊        C  0A  0A            Â        ‡@  `@  Ä@            Ë         A  Ä@  Ä?            Î                @@Xë°æP ÇK      / G S   / a n a l y z e -   / W 3   / G y -   / Z c : w c h a r _ t   / I " . / s r c "   / Z i   / G m -   / O d   / F d " D e b u g \ v c 1 2 0 . p d b "   / f p : p r e c i s e   / D   " W I N 3 2 "   / D   " _ W I N D O W S "   / D   " _ D E B U G "   / D   " _ L I B "   / D   " W I N 3 2 _ L E A N _ A N D _ M E A N "   / e r r o r R e p o r t : p r o m p t   / G F -   / W X -   / Z c : f o r S c o p e   / R T C 1   / G d   / O y -   / M D d   / F a " D e b u g \ "   / n o l o g o   / F o " D e b u g \ "   / F p " D e b u g \ L i b W e b P . p c h "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 ;…˜∂¿P §K      C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ i n c l u d e ; C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ a t l m f c \ i n c l u d e ; C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ W i n d o w s   K i t s \ 8 . 1 \ I n c l u d e \ u m ; C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ W i n d o w s   K i t s \ 8 . 1 \ I n c l u d e \ s h a r e d ; C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ W i n d o w s   K i t s \ 8 . 1 \ I n c l u d e \ w i n r t ; ;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     ˇí0B¡P ßK        / I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ i n c l u d e "   / I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ a t l m f c \ i n c l u d e "   / I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ W i n d o w s   K i t s \ 8 . 1 \ I n c l u d e \ u m "   / I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ W i n d o w s   K i t s \ 8 . 1 \ I n c l u d e \ s h a r e d "   / I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ W i n d o w s   K i t s \ 8 . 1 \ I n c l u d e \ w i n r t "   / A I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ a t l m f c \ l i b "   / A I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ l i b "   / A I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ l i b "   / A I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ M i c r o s o f t   V i s u a l   S t u d i o   1 2 . 0 \ V C \ a t l m f c \ l i b "   / A I   " C : \ P r o g r a m   F i l e s   ( x 8 6 ) \ W i n d o w s   K i t s \ 8 . 1 \ l i b \ w i n v 6 . 3 \ u m \ x 8 6 "   / B C D   " F : \ P r o j e c t s \ I E 2 D \ L i b r a r i e s \ S o u r c e s \ F r e e I m a g e   3 . 1 6 . 0 \ S o u r c e \ L i b W e b P "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           ˘
ËY√@ ^        #PÛ         Äˇ                      !            	        c a c h                Ä   CACHEFILE_H        Äˇ                      #                        f r e e                Ä   FreeImage.h        Äˇ                      #                        u t i l                Ä   Utilities.h         ˝                       #       !               c a c h                Ä
ÄÄ CACHE_SIZEint = 32         ˝                       #   !   . !    !    !   b l o c                Ä
ÄÄ BLOCK_SIZEint = (64*1024)-8        Äˇ                           +    /    +    +   b l o c                Ä   Block         ˇ                          ,    ,    ,    ,   n r                    ÄÄ  nrunsigned          ˇ                          -    -    -    -   n e x t                ÄÄ  nextunsigned          ˇ	                          .    .    .    .   d a t a                ÄÄ  dataBYTE *         Äˇ
                           9    Z    9    9   c a c h                Ä	 	 	 CacheFile         ˇ              
       /     :   & :    :   % :   p a g e                Ä	Ä  PageCachestd::list<Block*>          ˇ              
       /     ;   2 ;   ' ;   1 ;   p a g e                ÄÄ( ( PageCacheItstd::list<Block*>::iterator          ˇ              
       /     <   , <   % <   + <   p a g e                ÄÄ" " PageMapstd::map<int,PageCacheIt>          ˇ              
       /     =   8 =   / =   7 =   p a g e                Ä	Ä. . PageMapItstd::map<int,PageCacheIt>::iterator          ˇ              
            @   < @    @   
 @   c a c h                Ä	Ä  CacheFile         ˛                     
     @   % @    @   % @   f i l e               ÄÄ  filenameconst std::string          ˛                     
    ( @   : @   - @   : @   k e e p               ÄÄ  keep_in_memoryBOOL          ˇ              
            A    A    A    A   ~ c a c                Ä
Ä  ~CacheFile         ˇ              
            C    C    C   
 C   o p e n                ÄÄ  openBOOL          ˇ              
            D    D    D    D   c l o s                ÄÄ  closevoid          ˇ              
            E   - E    E    E   r e a d                ÄÄ  readFileBOOL          ˛                     
     E    E    E    E   d a t a               ÄÄ  dataBYTE *          ˛                     
     E   ! E     E   ! E   n r                   ÄÄ  nrint          ˛                     
    $ E   + E   ( E   + E   s i z e               ÄÄ	 	 sizeint          ˇ              
            F   % F    F    F   w r i t                Ä	Ä  writeFileint          ˛                     
     F    F    F    F   d a t a               ÄÄ  dataBYTE *          ˛                     
     F   # F     F   # F   s i z e               ÄÄ	 	 sizeint          ˇ              
            G    G    G    G   d e l e                Ä
Ä  deleteFilevoid          ˛                     
     G    G    G    G   n r                   ÄÄ  nrint          ˇ              
            J    J    J    J   c l e a                ÄÄ  cleanupMemCachevoid          ˇ              
            K    K    K    K   a l l o                ÄÄ  allocateBlockint          ˇ               
            L    L   	 L    L   l o c k                Ä	Ä  lockBlockBlock *          ˛!                      
     L    L    L    L   n r                   ÄÄ  nrint          ˇ"              
            M    M    M    M   u n l o                ÄÄ  unlockBlockBOOL          ˛#              "       
     M    M    M    M   n r                   ÄÄ  nrint                                        œn^¯mÜ--ü\9l…aãˆ∫àÍ ä∫iOπ·Ëv∏®°∑.7¥¶J¶Ω’<µt:¥“Ég≥˙“è≤+B∞±>!‘†j† †Z°ŸM≈@ ~_        " ˆ          ˇ$              
            N    N    N    N   d e l e                ÄÄ  deleteBlockBOOL          ˛%              $       
     N    N    N    N   n r                   ÄÄ  nrint          ˇ&              
            Q    Q    Q    Q   m _ f i                ÄÄ  m_fileFILE *          ˇ'              
            R    R    R    R   m _ f i                Ä
Ä  m_filenamestd::string          ˇ(              
            S    S    S    S   m _ f r                ÄÄ  m_free_pagesstd::list<int>          ˇ)              
            T    T    T    T   m _ p a                ÄÄ  m_page_cache_memPageCache          ˇ*              
            U    U    U    U   m _ p a                ÄÄ  m_page_cache_diskPageCache          ˇ+              
            V    V   
 V    V   m _ p a                Ä
Ä  m_page_mapPageMap          ˇ,              
            W    W    W    W   m _ p a                ÄÄ  m_page_countint          ˇ-              
            X    X   	 X    X   m _ c u                ÄÄ  m_current_blockBlock *          ˇ.   