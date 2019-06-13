/***
*sal.h - markers for documenting the semantics of APIs
*
* Copyright © Microsoft Corp.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* • Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
* • Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*Purpose:
*       sal.h provides a set of annotations to describe how a function uses its
*       parameters - the assumptions it makes about them, and the guarantees it makes
*       upon finishing.
*
*       [Public]
*
****/

/* 
 -------------------------------------------------------------------------------
 Introduction

 sal.h provides a set of annotations to describe how a function uses its
 parameters - the assumptions it makes about them, and the guarantees it makes
 upon finishing.

 Annotations may be placed before either a function parameter's type or its return
 type, and describe the function's behavior regarding the parameter or return value.
 There are two classes of annotations: buffer annotations and advanced annotations.
 Buffer annotations describe how functions use their pointer parameters, and
 advanced annotations either describe complex/unusual buffer behavior, or provide
 additional information about a parameter that is not otherwise expressible.

 -------------------------------------------------------------------------------
 Buffer Annotations

 The most important annotations in sal.h provide a consistent way to annotate
 buffer parameters or return values for a function. Each of these annotations describes
 a single buffer (which could be a string, a fixed-length or variable-length array,
 or just a pointer) that the function interacts with: where it is, how large it is,
 how much is initialized, and what the function does with it.

 The appropriate macro for a given buffer can be constructed using the table below.
 Just pick the appropriate values from each category, and combine them together
 with a leading underscore. Some combinations of values do not make sense as buffer
 annotations. Only meaningful annotations can be added to your code; for a list of
 these, see the buffer annotation definitions section.

 Only a single buffer annotation should be used for each parameter.

 |------------|------------|---------|--------|----------|----------|---------------|
 |   Level    |   Usage    |  Size   | Output | NullTerm | Optional |  Parameters   |
 |------------|------------|---------|--------|----------|----------|---------------|
 | <>         | <>         | <>      | <>     | _z       | <>       | <>            |
 | _deref     | _in        | _ecount | _full  | _nz      | _opt     | (size)        |
 | _deref_opt | _out       | _bcount | _part  |          |          | (size,length) |
 |            | _inout     |         |        |          |          |               |
 |            |            |         |        |          |          |               |
 |------------|------------|---------|--------|----------|----------|---------------|

 Level: Describes the buffer pointer's level of indirection from the parameter or
          return value 'p'.

 <>         : p is the buffer pointer.
 _deref     : *p is the buffer pointer. p must not be NULL.
 _deref_opt : *p may be the buffer pointer. p may be NULL, in which case the rest of
                the annotation is ignored.

 Usage: Describes how the function uses the buffer.

 <>     : The buffer is not accessed. If used on the return value or with _deref, the
            function will provide the buffer, and it will be uninitialized at exit.
            Otherwise, the caller must provide the buffer. This should only be used
            for alloc and free functions.
 _in    : The function will only read from the buffer. The caller must provide the
            buffer and initialize it. Cannot be used with _deref.
 _out   : The function will only write to the buffer. If used on the return value or
            with _deref, the function will provide the buffer and initialize it.
            Otherwise, the caller must provide the buffer, and the function will
            initialize it.
 _inout : The function may freely read from and write to the buffer. The caller must
            provide the buffer and initialize it. If used with _deref, the buffer may
            be reallocated by the function.

 Size: Describes the total size of the buffer. This may be less than the space actually
         allocated for the buffer, in which case it describes the accessible amount.

 <>      : No buffer size is given. If the type specifies the buffer size (such as
             with LPSTR and LPWSTR), that amount is used. Otherwise, the buffer is one
             element long. Must be used with _in, _out, or _inout.
 _ecount : The buffer size is an explicit element count.
 _bcount : The buffer size is an explicit byte count.

 Output: Describes how much of the buffer will be initialized by the function. For
           _inout buffers, this also describes how much is initialized at entry. Omit this
           category for _in buffers; they must be fully initialized by the caller.

 <>    : The type specifies how much is initialized. For instance, a function initializing
           an LPWSTR must NULL-terminate the string.
 _full : The function initializes the entire buffer.
 _part : The function initializes part of the buffer, and explicitly indicates how much.

 NullTerm: States if the present of a '\0' marks the end of valid elements in the buffer.
 _z    : A '\0' indicated the end of the buffer
 _nz	 : The buffer may not be null terminated and a '\0' does not indicate the end of the
          buffer.
 Optional: Describes if the buffer itself is optional.

 <>   : The pointer to the buffer must not be NULL.
 _opt : The pointer to the buffer might be NULL. It will be checked before being dereferenced.

 Parameters: Gives explicit counts for the size and length of the buffer.

 <>            : There is no explicit count. Use when neither _ecount nor _bcount is used.
 (size)        : Only the buffer's total size is given. Use with _ecount or _bcount but not _part.
 (size,length) : The buffer's total size and initialized length are given. Use with _ecount_part
                   and _bcount_part.

 -------------------------------------------------------------------------------
 Buffer Annotation Examples

 LWSTDAPI_(BOOL) StrToIntExA(
     LPCSTR pszString,                    -- No annotation required, const implies __in.
     DWORD dwFlags,
     __out int *piRet                     -- A pointer whose dereference will be filled in.
 );

 void MyPaintingFunction(
     __in HWND hwndControl,               -- An initialized read-only parameter.
     __in_opt HDC hdcOptional,            -- An initialized read-only parameter that might be NULL.
     __inout IPropertyStore *ppsStore     -- An initialized parameter that may be freely used
                                          --   and modified.
 );

 LWSTDAPI_(BOOL) PathCompactPathExA(
     __out_ecount(cchMax) LPSTR pszOut,   -- A string buffer with cch elements that will
                                          --   be NULL terminated on exit.
     LPCSTR pszSrc,                       -- No annotation required, const implies __in.
     UINT cchMax,
     DWORD dwFlags
 );

 HRESULT SHLocalAllocBytes(
     size_t cb,
     __deref_bcount(cb) T **ppv           -- A pointer whose dereference will be set to an
                                          --   uninitialized buffer with cb bytes.
 );

 __inout_bcount_full(cb) : A buffer with cb elements that is fully initialized at
     entry and exit, and may be written to by this function.

 __out_ecount_part(count, *countOut) : A buffer with count elements that will be
     partially initialized by this function. The function indicates how much it
     initialized by setting *countOut.

 -------------------------------------------------------------------------------
 Advanced Annotations

 Advanced annotations describe behavior that is not expressible with the regular
 buffer macros. These may be used either to annotate buffer parameters that involve
 complex or conditional behavior, or to enrich existing annotations with additional
 information.

 __success(expr) f :
     <expr> indicates whether function f succeeded or not. If <expr> is true at exit,
     all the function's guarantees (as given by other annotations) must hold. If <expr>
     is false at exit, the caller should not expect any of the function's guarantees
     to hold. If not used, the function must always satisfy its guarantees. Added
     automatically to functions that indicate success in standard ways, such as by
     returning an HRESULT.

 __nullterminated p :
     Pointer p is a buffer that may be read or written up to and including the first
     NULL character or pointer. May be used on typedefs, which marks valid (properly
     initialized) instances of that type as being NULL-terminated.

 __nullnullterminated p :
     Pointer p is a buffer that may be read or written up to and including the first
     sequence of two NULL characters or pointers. May be used on typedefs, which marks
     valid instances of that type as being double-NULL terminated.

 __reserved v :
     Value v must be 0/NULL, reserved for future use.

 __checkReturn v :
     Return value v must not be ignored by callers of this function.

 __typefix(ctype) v :
     Value v should be treated as an instance of ctype, rather than its declared type.

 __override f :
     Specify C#-style 'override' behaviour for overriding virtual methods.

 __callback f :
     Function f can be used as a function pointer.

 __format_string p :
     Pointer p is a string that contains % markers in the style of printf.

 __blocksOn(resource) f :
     Function f blocks on the resource 'resource'.

 __fallthrough :
     Annotates switch statement labels where fall-through is desired, to distinguish
     from forgotten break statements.

 -------------------------------------------------------------------------------
 Advanced Annotation Examples

 __success(return == TRUE) LWSTDAPI_(BOOL) 
 PathCanonicalizeA(__out_ecount(MAX_PATH) LPSTR pszBuf, LPCSTR pszPath) :
     pszBuf is only guaranteed to be NULL-terminated when TRUE is returned.

 typedef __nullterminated WCHAR* LPWSTR : Initialized LPWSTRs are NULL-terminated strings.

 __out_ecount(cch) __typefix(LPWSTR) void *psz : psz is a buffer parameter which will be
     a NULL-terminated WCHAR string at exit, and which initially contains cch WCHARs.

 -------------------------------------------------------------------------------
*/

#pragma once
#ifndef __specstrings
#define __specstrings

#ifdef  __cplusplus
#ifndef __nothrow
# define __nothrow __declspec(nothrow)
#endif
extern "C" {
#else
#ifndef __nothrow
# define __nothrow
#endif
#endif  /* #ifdef __cplusplus */

/*
 -------------------------------------------------------------------------------
 Helper Macro Definitions

 These express behavior common to many of the high-level annotations.
 DO NOT USE THESE IN YOUR CODE.
 -------------------------------------------------------------------------------
*/

/*
The helper annotations are only understood by the compiler version used by various
defect detection tools. When the regular compiler is running, they are defined into
nothing, and do not affect the compiled code.
*/

#if !defined(__midl) && defined(_PREFAST_) 

    /*
     In the primitive __declspec("SAL_*") annotations "SAL" stands for Standard
     Annotation Language.  These __declspec("SAL_*") annotations are the
     primitives the compiler understands and all high-level SpecString MACROs
     will decompose into these primivates.
    */

    #define SPECSTRINGIZE( x ) #x

    /*
     __null p
     __notnull p
     __maybenull p
    
     Annotates a pointer p. States that pointer p is null. Commonly used
     in the negated form __notnull or the possibly null form __maybenull.
    */

//    #define __null                  __declspec("SAL_null")
    #define __notnull               __declspec("SAL_notnull")
    #define __maybenull             __declspec("SAL_maybenull")

    /*
     __readonly l
     __notreadonly l
     __mabyereadonly l
    
     Annotates a location l. States that location l is not modified after
     this point.  If the annotation is placed on the precondition state of
     a function, the restriction only applies until the postcondition state
     of the function.  __maybereadonly states that the annotated location
     may be modified, whereas __notreadonly states that a location must be
     modified.
    */

    #define __readonly              __declspec("SAL_readonly")
    #define __notreadonly           __declspec("SAL_notreadonly")
    #define __maybereadonly         __declspec("SAL_maybereadonly")

    /*
     __valid v
     __notvalid v
     __maybevalid v
    
     Annotates any value v. States that the value satisfies all properties of
     valid values of its type. For example, for a string buffer, valid means
     that the buffer pointer is either NULL or points to a NULL-terminated string.
    */

    #define __valid                 __declspec("SAL_valid")
    #define __notvalid              __declspec("SAL_notvalid")
    #define __maybevalid            __declspec("SAL_maybevalid")

    /*
     __readableTo(extent) p
    
     Annotates a buffer pointer p.  If the buffer can be read, extent describes
     how much of the buffer is readable. For a reader of the buffer, this is
     an explicit permission to read up to that amount, rather than a restriction to
     read only up to it.
    */

    #define __readableTo(extent)    __declspec("SAL_readableTo("SPECSTRINGIZE(extent)")")

    /*
    
     __elem_readableTo(size)
    
     Annotates a buffer pointer p as being readable to size elements.
    */

    #define __elem_readableTo(size)   __declspec("SAL_readableTo(elementCount("SPECSTRINGIZE(size)"))")
    
    /*
     __byte_readableTo(size)
    
     Annotates a buffer pointer p as being readable to size bytes.
    */
    #define __byte_readableTo(size)   __declspec("SAL_readableTo(byteCount("SPECSTRINGIZE(size)"))")
    
    /*
     __writableTo(extent) p
    
     Annotates a buffer pointer p. If the buffer can be modified, extent
     describes how much of the buffer is writable (usually the allocation
     size). For a writer of the buffer, this is an explicit permission to
     write up to that amount, rather than a restriction to write only up to it.
    */
    #define __writableTo(size)   __declspec("SAL_writableTo("SPECSTRINGIZE(size)")")

    /*
     __elem_writableTo(size)
    
     Annotates a buffer pointer p as being writable to size elements.
    */
    #define __elem_writableTo(size)   __declspec("SAL_writableTo(elementCount("SPECSTRINGIZE(size)"))")
    
    /*
     __byte_writableTo(size)
    
     Annotates a buffer pointer p as being writable to size bytes.
    */
    #define __byte_writableTo(size)   __declspec("SAL_writableTo(byteCount("SPECSTRINGIZE(size)"))")

    /*
     __deref p
    
     Annotates a pointer p. The next annotation applies one dereference down
     in the type. If readableTo(p, size) then the next annotation applies to
     all elements *(p+i) for which i satisfies the size. If p is a pointer
     to a struct, the next annotation applies to all fields of the struct.
    */
    #define __deref                 __declspec("SAL_deref")
    
    /*
     __pre __next_annotation
    
     The next annotation applies in the precondition state
    */
    #define __pre                   __declspec("SAL_pre")
    
    /*
     __post __next_annotation
    
     The next annotation applies in the postcondition state
    */
    #define __post                  __declspec("SAL_post")
    
    /*
     __precond(<expr>)
    
     When <expr> is true, the next annotation applies in the precondition state
     (currently not enabled)
    */
    #define __precond(expr)         __pre

    /*
     __postcond(<expr>)
    
     When <expr> is true, the next annotation applies in the postcondition state
     (currently not enabled)
    */
    #define __postcond(expr)        __post

    /*
     __exceptthat
    
     Given a set of annotations Q containing __exceptthat maybeP, the effect of
     the except clause is to erase any P or notP annotations (explicit or
     implied) within Q at the same level of dereferencing that the except
     clause appears, and to replace it with maybeP.
    
      Example 1: __valid __exceptthat __maybenull on a pointer p means that the
                 pointer may be null, and is otherwise valid, thus overriding
                 the implicit notnull annotation implied by __valid on
                 pointers.
    
      Example 2: __valid __deref __exceptthat __maybenull on an int **p means
                 that p is not null (implied by valid), but the elements
                 pointed to by p could be null, and are otherwise valid. 
    */
    #define __exceptthat                __declspec("SAL_except")
    #define __execeptthat               __exceptthat
 
    /*
     _refparam
    
     Added to all out parameter macros to indicate that they are all reference
     parameters.
    */
    #define __refparam                  __deref __notreadonly

    /*
     __inner_*
    
     Helper macros that directly correspond to certain high-level annotations.
    
    */

    /*
     Macros to classify the entrypoints and indicate their category.
    
     Pre-defined control point categories include: RPC, LPC, DeviceDriver, UserToKernel, ISAPI, COM.
    
    */
    #define __inner_control_entrypoint(category) __declspec("SAL_entrypoint(controlEntry, "SPECSTRINGIZE(category)")")

    /*
     Pre-defined data entry point categories include: Registry, File, Network.
    */
    #define __inner_data_entrypoint(category)    __declspec("SAL_entrypoint(dataEntry, "SPECSTRINGIZE(category)")")

    #define __inner_success(expr)               __declspec("SAL_success("SPECSTRINGIZE(expr)")")
    #define __inner_checkReturn                 __declspec("SAL_checkReturn")
    #define __inner_typefix(ctype)              __declspec("SAL_typefix("SPECSTRINGIZE(ctype)")")
    #define __inner_override                    __declspec("__override")
    #define __inner_callback                    __declspec("__callback")
    #define __inner_blocksOn(resource)          __declspec("SAL_blocksOn("SPECSTRINGIZE(resource)")")
    #define __inner_fallthrough_dec             __inline __nothrow void __FallThrough() {}
    #define __inner_fallthrough                 __FallThrough();

#else
//  This conflicts with gcc definition of __null.
//    #define __null
    #define __notnull
    #define __maybenull
    #define __readonly
    #define __notreadonly
    #define __maybereadonly
    #define __valid
    #define __notvalid
    #define __maybevalid
    #define __readableTo(extent)
    #define __elem_readableTo(size)
    #define __byte_readableTo(size)
    #define __writableTo(size)
    #define __elem_writableTo(size)
    #define __byte_writableTo(size)
    #define __deref
    #define __pre
    #define __post
    #define __precond(expr)
    #define __postcond(expr)
    #define __exceptthat
    #define __execeptthat
    #define __inner_success(expr)
    #define __inner_checkReturn
    #define __inner_typefix(ctype)
    #define __inner_override
    #define __inner_callback
    #define __inner_blocksOn(resource)
    #define __inner_fallthrough_dec
    #define __inner_fallthrough
    #define __refparam
    #define __inner_control_entrypoint(category)
    #define __inner_data_entrypoint(category)
#endif /* #if !defined(__midl) && defined(_PREFAST_) */

/* 
-------------------------------------------------------------------------------
Buffer Annotation Definitions

Any of these may be used to directly annotate functions, but only one should
be used for each parameter. To determine which annotation to use for a given
buffer, use the table in the buffer annotations section.
-------------------------------------------------------------------------------
*/

#define __ecount(size)                                          __notnull __elem_writableTo(size)
#define __bcount(size)                                          __notnull __byte_writableTo(size)
//#define __in                                                    __pre __valid __pre __deref __readonly
#define __in_win                                                __pre __valid __pre __deref __readonly

#define __in_ecount(size)                                       __in_win __pre __elem_readableTo(size)
#define __in_bcount(size)                                       __in_win __pre __byte_readableTo(size)
#define __in_z                                                  __in_win __pre __nullterminated
#define __in_ecount_z(size)                                     __in_ecount(size) __pre __nullterminated
#define __in_bcount_z(size)                                     __in_bcount(size) __pre __nullterminated
#define __in_nz                                                 __in_win
#define __in_ecount_nz(size)                                    __in_ecount(size)
#define __in_bcount_nz(size)                                    __in_bcount(size)

//#define __out                                                   __ecount(1) __post __valid __refparam
#define __out_win                                                 __ecount(1) __post __valid __refparam

#define __out_ecount(size)                                      __ecount(size) __post __valid __refparam
#define __out_bcount(size)                                      __bcount(size) __post __valid __refparam
#define __out_ecount_part(size,length)                          __out_ecount(size) __post __elem_readableTo(length)
#define __out_bcount_part(size,length)                          __out_bcount(size) __post __byte_readableTo(length)
#define __out_ecount_full(size)                                 __out_ecount_part(size,size)
#define __out_bcount_full(size)                                 __out_bcount_part(size,size)
#define __out_z                                                 __post __valid __refparam __post __nullterminated
#define __out_z_opt                                             __post __valid __refparam __post __nullterminated __exceptthat __maybenull
#define __out_ecount_z(size)                                    __ecount(size) __post __valid __refparam __post __nullterminated
#define __out_bcount_z(size)                                    __bcount(size) __post __valid __refparam __post __nullterminated
#define __out_ecount_part_z(size,length)                        __out_ecount_part(size,length) __post __nullterminated
#define __out_bcount_part_z(size,length)                        __out_bcount_part(size,length) __post __nullterminated
#define __out_ecount_full_z(size)                               __out_ecount_full(size) __post __nullterminated
#define __out_bcount_full_z(size)                               __out_bcount_full(size) __post __nullterminated
#define __out_nz                                                __post __valid __refparam __post
#define __out_nz_opt                                            __post __valid __refparam __post __exceptthat __maybenull
#define __out_ecount_nz(size)                                   __ecount(size) __post __valid __refparam
#define __out_bcount_nz(size)                                   __bcount(size) __post __valid __refparam
#define __inout                                                 __pre __valid __post __valid __refparam
#define __inout_ecount(size)                                    __out_ecount(size) __pre __valid
#define __inout_bcount(size)                                    __out_bcount(size) __pre __valid
#define __inout_ecount_part(size,length)                        __out_ecount_part(size,length) __pre __valid __pre __elem_readableTo(length)
#define __inout_bcount_part(size,length)                        __out_bcount_part(size,length) __pre __valid __pre __byte_readableTo(length)
#define __inout_ecount_full(size)                               __inout_ecount_part(size,size)
#define __inout_bcount_full(size)                               __inout_bcount_part(size,size)
#define __inout_z                                               __inout __pre __nullterminated __post __nullterminated
#define __inout_ecount_z(size)                                  __inout_ecount(size) __pre __nullterminated __post __nullterminated
#define __inout_bcount_z(size)                                  __inout_bcount(size) __pre __nullterminated __post __nullterminated
#define __inout_nz                                              __inout
#define __inout_ecount_nz(size)                                 __inout_ecount(size) 
#define __inout_bcount_nz(size)                                 __inout_bcount(size) 
#define __ecount_opt(size)                                      __ecount(size)                              __exceptthat __maybenull
#define __bcount_opt(size)                                      __bcount(size)                              __exceptthat __maybenull
#define __in_opt                                                __in_win                                        __exceptthat __maybenull
#define __in_ecount_opt(size)                                   __in_ecount(size)                           __exceptthat __maybenull
#define __in_bcount_opt(size)                                   __in_bcount(size)                           __exceptthat __maybenull
#define __in_z_opt                                              __in_opt __pre __nullterminated 
#define __in_ecount_z_opt(size)                                 __in_ecount_opt(size) __pre __nullterminated 
#define __in_bcount_z_opt(size)                                 __in_bcount_opt(size) __pre __nullterminated
#define __in_nz_opt                                             __in_opt                                     
#define __in_ecount_nz_opt(size)                                __in_ecount_opt(size)                         
#define __in_bcount_nz_opt(size)                                __in_bcount_opt(size)                         
#define __out_opt                                               __out_win                                       __exceptthat __maybenull
#define __out_ecount_opt(size)                                  __out_ecount(size)                          __exceptthat __maybenull
#define __out_bcount_opt(size)                                  __out_bcount(size)                          __exceptthat __maybenull
#define __out_ecount_part_opt(size,length)                      __out_ecount_part(size,length)              __exceptthat __maybenull
#define __out_bcount_part_opt(size,length)                      __out_bcount_part(size,length)              __exceptthat __maybenull
#define __out_ecount_full_opt(size)                             __out_ecount_full(size)                     __exceptthat __maybenull
#define __out_bcount_full_opt(size)                             __out_bcount_full(size)                     __exceptthat __maybenull
#define __out_ecount_z_opt(size)                                __out_ecount_opt(size) __post __nullterminated
#define __out_bcount_z_opt(size)                                __out_bcount_opt(size) __post __nullterminated
#define __out_ecount_part_z_opt(size,length)                    __out_ecount_part_opt(size,length) __post __nullterminated
#define __out_bcount_part_z_opt(size,length)                    __out_bcount_part_opt(size,length) __post __nullterminated
#define __out_ecount_full_z_opt(size)                           __out_ecount_full_opt(size) __post __nullterminated
#define __out_bcount_full_z_opt(size)                           __out_bcount_full_opt(size) __post __nullterminated
#define __out_ecount_nz_opt(size)                               __out_ecount_opt(size) __post __nullterminated                       
#define __out_bcount_nz_opt(size)                               __out_bcount_opt(size) __post __nullterminated                        
#define __inout_opt                                             __inout                                     __exceptthat __maybenull
#define __inout_ecount_opt(size)                                __inout_ecount(size)                        __exceptthat __maybenull
#define __inout_bcount_opt(size)                                __inout_bcount(size)                        __exceptthat __maybenull
#define __inout_ecount_part_opt(size,length)                    __inout_ecount_part(size,length)            __exceptthat __maybenull
#define __inout_bcount_part_opt(size,length)                    __inout_bcount_part(size,length)            __exceptthat __maybenull
#define __inout_ecount_full_opt(size)                           __inout_ecount_full(size)                   __exceptthat __maybenull
#define __inout_bcount_full_opt(size)                           __inout_bcount_full(size)                   __exceptthat __maybenull
#define __inout_z_opt                                           __inout_opt __pre __nullterminated __post __nullterminated
#define __inout_ecount_z_opt(size)                              __inout_ecount_opt(size) __pre __nullterminated __post __nullterminated
#define __inout_ecount_z_opt(size)                              __inout_ecount_opt(size) __pre __nullterminated __post __nullterminated
#define __inout_bcount_z_opt(size)                              __inout_bcount_opt(size) 
#define __inout_nz_opt                                          __inout_opt
#define __inout_ecount_nz_opt(size)                             __inout_ecount_opt(size)
#define __inout_bcount_nz_opt(size)                             __inout_bcount_opt(size)
#define __deref_ecount(size)                                    __ecount(1) __post __elem_readableTo(1) __post __deref __notnull __post __deref __elem_writableTo(size)
#define __deref_bcount(size)                                    __ecount(1) __post __elem_readableTo(1) __post __deref __notnull __post __deref __byte_writableTo(size)
#define __deref_out                                             __deref_ecount(1) __post __deref __valid __refparam
#define __deref_out_ecount(size)                                __deref_ecount(size) __post __deref __valid __refparam
#define __deref_out_bcount(size)                                __deref_bcount(size) __post __deref __valid __refparam
#define __deref_out_ecount_part(size,length)                    __deref_out_ecount(size) __post __deref __elem_readableTo(length)
#define __deref_out_bcount_part(size,length)                    __deref_out_bcount(size) __post __deref __byte_readableTo(length)
#define __deref_out_ecount_full(size)                           __deref_out_ecount_part(size,size)
#define __deref_out_bcount_full(size)                           __deref_out_bcount_part(size,size)
#define __deref_out_z                                           __post __deref __valid __refparam __post __deref __nullterminated
#define __deref_out_ecount_z(size)                              __deref_out_ecount(size) __post __deref __nullterminated  
#define __deref_out_bcount_z(size)                              __deref_out_ecount(size) __post __deref __nullterminated  
#define __deref_out_nz                                          __deref_out
#define __deref_out_ecount_nz(size)                             __deref_out_ecount(size)   
#define __deref_out_bcount_nz(size)                             __deref_out_ecount(size)   
#define __deref_inout                                           __notnull __elem_readableTo(1) __pre __deref __valid __post __deref __valid __refparam
#define __deref_inout_z                                         __deref_inout __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_inout_ecount(size)                              __deref_inout __pre __deref __elem_writableTo(size) __post __deref __elem_writableTo(size)
#define __deref_inout_bcount(size)                              __deref_inout __pre __deref __byte_writableTo(size) __post __deref __byte_writableTo(size)
#define __deref_inout_ecount_part(size,length)                  __deref_inout_ecount(size) __pre __deref __elem_readableTo(length) __post __deref __elem_readableTo(length)
#define __deref_inout_bcount_part(size,length)                  __deref_inout_bcount(size) __pre __deref __byte_readableTo(length) __post __deref __byte_readableTo(length)
#define __deref_inout_ecount_full(size)                         __deref_inout_ecount_part(size,size)
#define __deref_inout_bcount_full(size)                         __deref_inout_bcount_part(size,size)
#define __deref_inout_z                                         __deref_inout __pre __deref __nullterminated __post __deref __nullterminated
#define __deref_inout_ecount_z(size)                            __deref_inout_ecount(size) __pre __deref __nullterminated __post __deref __nullterminated   
#define __deref_inout_bcount_z(size)                            __deref_inout_ecount(size) __pre __deref __nullterminated __post __deref __nullterminated  
#define __deref_inout_nz                                        __deref_inout
#define __deref_inout_ecount_nz(size)                           __deref_inout_ecount(size)   
#define __deref_inout_bcount_nz(size)                           __deref_inout_ecount(size)   
#define __deref_ecount_opt(size)                                __deref_ecount(size)                        __post __deref __exceptthat __maybenull
#define __deref_bcount_opt(size)                                __deref_bcount(size)                        __post __deref __exceptthat __maybenull
#define __deref_out_opt                                         __deref_out                                 __post __deref __exceptthat __maybenull
#define __deref_out_ecount_opt(size)                            __deref_out_ecount(size)                    __post __deref __exceptthat __maybenull
#define __deref_out_bcount_opt(size)                            __deref_out_bcount(size)                    __post __deref __exceptthat __maybenull
#define __deref_out_ecount_part_opt(size,length)                __deref_out_ecount_part(size,length)        __post __deref __exceptthat __maybenull
#define __deref_out_bcount_part_opt(size,length)                __deref_out_bcount_part(size,length)        __post __deref __exceptthat __maybenull
#define __deref_out_ecount_full_opt(size)                       __deref_out_ecount_full(size)               __post __deref __exceptthat __maybenull
#define __deref_out_bcount_full_opt(size)                       __deref_out_bcount_full(size)               __post __deref __exceptthat __maybenull
#define __deref_out_z_opt                                       __post __deref __valid __refparam __execeptthat __maybenull __post __deref __nullterminated
#define __deref_out_ecount_z_opt(size)                          __deref_out_ecount_opt(size) __post __deref __nullterminated
#define __derefÊ±I<>
@ rƒ       " ö          ÿ“e                         Ù   ä   Ú   Ú  p n g _                €€  png_get_sPLTint PNGAPI          þ”e            “e      
     Ú  ' Ú  ! Ú  ' Ú  p n g _               €€  png_ptrpng_const_structrp          þ•e            “e      
    * Ú  < Ú  5 Ú  < Ú  i n f o               €€  info_ptrpng_inforp          þ–e            “e      
     Û   Û   Û   Û  s p a l               €	€  spalettespng_sPLT_tpp          ÿ—e                         è   ö   é   é  p n g _                €€! ! png_get_hISTpng_uint_32 PNGAPI          þ˜e            —e      
     é  ' é  ! é  ' é  p n g _               €€  png_ptrpng_const_structrp          þ™e            —e      
    * é  < é  5 é  < é  i n f o               €€  info_ptrpng_inforp          þše            —e      
     ê   ê   ê   ê  h i s t               €€  histpng_uint_16p *          ÿ›e                         ù      ú   ú  p n g _                €€! ! png_get_IHDRpng_uint_32 PNGAPI          þœe            ›e      
     ú  ' ú  ! ú  ' ú  p n g _               €€  png_ptrpng_const_structrp          þe            ›e      
    * ú  B ú  ; ú  B ú  i n f o               €€  info_ptrpng_const_inforp          þže            ›e      
     û   û   û   û  w i d t               €€  widthpng_uint_32 *          þŸe            ›e      
     û  + û  & û  + û  h e i g               €€  heightpng_uint_32 *          þ e            ›e      
    . û  ; û  3 û  ; û  b i t _               €	€  bit_depthint *          þ¡e            ›e      
     ü   ü  
 ü   ü  c o l o               €
€  color_typeint *          þ¢e            ›e      
     ü  ( ü   ü  ( ü  i n t e               €€  interlace_typeint *          þ£e            ›e      
    + ü  ? ü  0 ü  ? ü  c o m p               €€  compression_typeint *          þ¤e            ›e      
     ý   ý  
 ý   ý  f i l t 	              €€  filter_typeint *          ÿ¥e                             0   !   !  p n g _                €€! ! png_get_oFFspng_uint_32 PNGAPI          þ¦e            ¥e      
     !  ' !  ! !  ' !  p n g _               €€  png_ptrpng_const_structrp          þ§e            ¥e      
    * !  B !  ; !  B !  i n f o               €€  info_ptrpng_const_inforp          þ¨e            ¥e      
     "   "   "   "  o f f s               €€  offset_xpng_int_32 *          þ©e            ¥e      
     "  . "  ' "  . "  o f f s               €€  offset_ypng_int_32 *          þªe            ¥e      
    1 "  > "  6 "  > "  u n i t               €	€  unit_typeint *          ÿ«e                         4   J   5   5  p n g _                €€! ! png_get_pCALpng_uint_32 PNGAPI          þ¬e            «e      
     5  ' 5  ! 5  ' 5  p n g _               €€  png_ptrpng_const_structrp          þ­e            «e      
    * 5  < 5  5 5  < 5  i n f o               €€  info_ptrpng_inforp          þ®e            «e      
     6   6   6   6  p u r p               €€  purposepng_charp *          þ¯e            «e      
     6  & 6  % 6  & 6  x 0                   €€  X0png_int_32 *          þ°e            «e      
    ) 6  6 6  5 6  6 6  x 1                   €€  X1png_int_32 *          þ±e            «e      
    9 6  A 6  > 6  A 6  t y p e               €€  typeint *          þ²e            «e      
    D 6  O 6  I 6  O 6  n p a r               €€  nparamsint *          þ³e            «e      
     7   7   7   7  u n i t               €€  unitspng_charp *   ¤      þ´e            «e      
     7  ( 7  # 7  ( 7!ÿ°CÞÙ­jýûüˆ<<›«¬ú7ZÂYI™Ï¨OÞhgôF„ö¡E.5µ”;¤»H3Õ2[¢Ûeaò0x  €Â8µÿ@
@ Á„       " ö       p a r a 	              €€  paramspng_charpp *          ÿµe                         Q   d   R   R  p n g _                €€' ' png_get_sCAL_fixedpng_uint_32 PNGAPI          þ¶e            µe      
     R  - R  ' R  - R  p n g _               €€  png_ptrpng_const_structrp          þ·e            µe      
    0 R  H R  A R  H R  i n f o               €€  info_ptrpng_const_inforp          þ¸e            µe      
     S   S  
 S   S  u n i t               €€  unitint *          þ¹e            µe      
     S  % S  ! S  % S  w i d t               €€  widthpng_fixed_point *          þºe            µe      
    ( S  > S  9 S  > S  h e i g               €€  heightpng_fixed_point *          ÿ»e                         h   v   i   i  p n g _                €€! ! png_get_sCALpng_uint_32 PNGAPI          þ¼e            »e      
     i  ' i  ! i  ' i  p n g _               €€  png_ptrpng_const_structrp          þ½e            »e      
    * i  B i  ; i  B i  i n f o               €€  info_ptrpng_const_inforp          þ¾e            »e      
     j   j  
 j   j  u n i t               €€  unitint *          þ¿e            »e      
     j   j   j   j  w i d t               €€  widthdouble *          þÀe            »e      
     j  , j  ' j  , j  h e i g               €€  heightdouble *          ÿÁe                         x   †   y   y  p n g _                €€# # png_get_sCAL_spng_uint_32 PNGAPI          þÂe            Áe      
     y  ) y  # y  ) y  p n g _               €€  png_ptrpng_const_structrp          þÃe            Áe      
    , y  D y  = y  D y  i n f o               €€  info_ptrpng_const_inforp          þÄe            Áe      
     z   z  
 z   z  u n i t               €€  unitint *          þÅe            Áe      
     z   z   z   z  w i d t               €€  widthpng_charpp          þÆe            Áe      
    " z  2 z  - z  2 z  h e i g               €€  heightpng_charpp          ÿÇe                         Š   ©   ‹   ‹  p n g _                €€! ! png_get_pHYspng_uint_32 PNGAPI          þÈe            Çe      
     ‹  ' ‹  ! ‹  ' ‹  p n g _               €€  png_ptrpng_const_structrp          þÉe            Çe      
    * ‹  B ‹  ; ‹  B ‹  i n f o               €€  info_ptrpng_const_inforp          þÊe            Çe      
     Œ   Œ   Œ   Œ  r e s _               €€  res_xpng_uint_32 *          þËe            Çe      
     Œ  * Œ  & Œ  * Œ  r e s _               €€  res_ypng_uint_32 *          þÌe            Çe      
    - Œ  : Œ  2 Œ  : Œ  u n i t               €	€  unit_typeint *          ÿÍe                         ¬   ¼   ­   ­  p n g _                €€! ! png_get_PLTEpng_uint_32 PNGAPI          þÎe            Íe      
     ­  ' ­  ! ­  ' ­  p n g _               €€  png_ptrpng_const_structrp          þÏe            Íe      
    * ­  < ­  5 ­  < ­  i n f o               €€  info_ptrpng_inforp          þÐe            Íe      
     ®   ®   ®   ®  p a l e               €€  palettepng_colorp *          þÑe            Íe      
     ®  ) ®   ®  ) ®  n u m _               €€  num_paletteint *          ÿÒe                         ¿   Í   À   À  p n g _                €€! ! png_get_sBITpng_uint_32 PNGAPI          þÓe            Òe      
     À  ' À  ! À  ' À  p n g _               €€  png_ptrpng_const_structrp          þÔe            Òe      
    * À  < À  5 À  < À  i n f o               €€  info_ptrpng_inforp  ¤      þÕe            Òe      
     Á   Á   Á   Á  s i g _            _˜>®ž-¹LF<Ì«LÝúj:÷9~™©„£9§À–F¦Ä%Uõçä}¤”Š£
’‚r±¡8‘¾ 8` € 64=§A
@ †       !@ö        €€  sig_bitpng_color_8p *          ÿÖe                         Ñ   ç   Ò   Ò  p n g _                €€  png_get_textint PNGAPI          þ×e            Öe      
     Ò  ' Ò  ! Ò  ' Ò  p n g _               €€  png_ptrpng_const_structrp          þØe            Öe      
    * Ò  < Ò  5 Ò  < Ò  i n f o               €€  info_ptrpng_inforp          þÙe            Öe      
     Ó   Ó   Ó   Ó  t e x t               €€  text_ptrpng_textp *          þÚe            Öe      
     Ó  & Ó   Ó  & Ó  n u m _               €€  num_textint *          ÿÛe                         ë   ù   ì   ì  p n g _                €€! ! png_get_tIMEpng_uint_32 PNGAPI          þÜe            Ûe      
     ì  ' ì  ! ì  ' ì  p n g _               €€  png_ptrpng_const_structrp          þÝe            Ûe      
    * ì  < ì  5 ì  < ì  i n f o               €€  info_ptrpng_inforp          þÞe            Ûe      
     í   í   í   í  m o d _               €€  mod_timepng_timep *          ÿße                         ý   &   þ   þ  p n g _                €€! ! png_get_tRNSpng_uint_32 PNGAPI          þàe            ße      
     þ  ' þ  ! þ  ' þ  p n g _               €€  png_ptrpng_const_structrp          þáe            ße      
    * þ  < þ  5 þ  < þ  i n f o               €€  info_ptrpng_inforp          þâe            ße      
     ÿ   ÿ   ÿ   ÿ  t r a n               €€  trans_alphapng_bytep *          þãe            ße      
     ÿ  * ÿ  " ÿ  * ÿ  n u m _               €	€  num_transint *          þäe            ße      
    - ÿ  F ÿ  < ÿ  F ÿ  t r a n               €€  trans_colorpng_color_16p *          ÿåe                         *   5   +   +  p n g _                €€# # png_get_unknown_chunksint PNGAPI          þæe            åe      
     +  1 +  + +  1 +  p n g _               €€  png_ptrpng_const_structrp          þçe            åe      
    4 +  F +  ? +  F +  i n f o               €€  info_ptrpng_inforp          þèe            åe      
     ,    ,   ,    ,  u n k n               €€  unknownspng_unknown_chunkpp          ÿée                         9   =   :   :  p n g _                €€, , png_get_rgb_to_gray_s