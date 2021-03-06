// Copyright 2011 Google Inc. All Rights Reserved.
//
// Use of this source code is governed by a BSD-style license
// that can be found in the COPYING file in the root of the source
// tree. An additional intellectual property rights grant can be found
// in the file PATENTS. All contributing project authors may
// be found in the AUTHORS file in the root of the source tree.
// -----------------------------------------------------------------------------
//
// VP8Iterator: block iterator
//
// Author: Skal (pascal.massimino@gmail.com)

#include <string.h>

#include "./vp8enci.h"

//------------------------------------------------------------------------------
// VP8Iterator
//------------------------------------------------------------------------------

static void InitLeft(VP8EncIterator* const it) {
  it->y_left_[-1] = it->u_left_[-1] = it->v_left_[-1] =
      (it->y_ > 0) ? 129 : 127;
  memset(it->y_left_, 129, 16);
  memset(it->u_left_, 129, 8);
  memset(it->v_left_, 129, 8);
  it->left_nz_[8] = 0;
}

static void InitTop(VP8EncIterator* const it) {
  const VP8Encoder* const enc = it->enc_;
  const size_t top_size = enc->mb_w_ * 16;
  memset(enc->y_top_, 127, 2 * top_size);
  memset(enc->nz_, 0, enc->mb_w_ * sizeof(*enc->nz_));
}

void VP8IteratorSetRow(VP8EncIterator* const it, int y) {
  VP8Encoder* const enc = it->enc_;
  it->x_ = 0;
  it->y_ = y;
  it->bw_ = &enc->parts_[y & (enc->num_parts_ - 1)];
  it->preds_ = enc->preds_ + y * 4 * enc->preds_w_;
  it->nz_ = enc->nz_;
  it->mb_ = enc->mb_info_ + y * enc->mb_w_;
  it->y_top_ = enc->y_top_;
  it->uv_top_ = enc->uv_top_;
  InitLeft(it);
}

void VP8IteratorReset(VP8EncIterator* const it) {
  VP8Encoder* const enc = it->enc_;
  VP8IteratorSetRow(it, 0);
  VP8IteratorSetCountDown(it, enc->mb_w_ * enc->mb_h_);  // default
  InitTop(it);
  InitLeft(it);
  memset(it->bit_count_, 0, sizeof(it->bit_count_));
  it->do_trellis_ = 0;
}

void VP8IteratorSetCountDown(VP8EncIterator* const it, int count_down) {
  it->count_down_ = it->count_down0_ = count_down;
}

int VP8IteratorIsDone(const VP8EncIterator* const it) {
  return (it->count_down_ <= 0);
}

void VP8IteratorInit(VP8Encoder* const enc, VP8EncIterator* const it) {
  it->enc_ = enc;
  it->y_stride_  = enc->pic_->y_stride;
  it->uv_stride_ = enc->pic_->uv_stride;
  it->yuv_in_   = (uint8_t*)DO_ALIGN(it->yuv_mem_);
  it->yuv_out_  = it->yuv_in_ + YUV_SIZE;
  it->yuv_out2_ = it->yuv_out_ + YUV_SIZE;
  it->yuv_p_    = it->yuv_out2_ + YUV_SIZE;
  it->lf_stats_ = enc->lf_stats_;
  it->percent0_ = enc->percent_;
  it->y_left_ = (uint8_t*)DO_ALIGN(it->yuv_left_mem_ + 1);
  it->u_left_ = it->y_left_ + 16 + 16;
  it->v_left_ = it->u_left_ + 16;
  VP8IteratorReset(it);
}

int VP8IteratorProgress(const VP8EncIterator* const it, int delta) {
  VP8Encoder* const enc = it->enc_;
  if (delta && enc->pic_->progress_hook != NULL) {
    const int done = it->count_down0_ - it->count_down_;
    const int percent = (it->count_down0_ <= 0)
                      ? it->percent0_
                      : it->percent0_ + delta * done / it->count_down0_;
    return WebPReportProgress(enc->pic_, percent, &enc->percent_);
  }
  return 1;
}

//------------------------------------------------------------------------------
// Import the source samples into the cache. Takes care of replicating
// boundary pixels if necessary.

static WEBP_INLINE int MinSize(int a, int b) { return (a < b) ? a : b; }

static void ImportBlock(const uint8_t* src, int src_stride,
                        uint8_t* dst, int w, int h, int size) {
  int i;
  for (i = 0; i < h; ++i) {
    memcpy(dst, src, w);
    if (w < size) {
      memset(dst + w, dst[w - 1], size - w);
    }
    dst += BPS;
    src += src_stride;
  }
  for (i = h; i < size; ++i) {
    memcpy(dst, dst - BPS, size);
    dst += BPS;
  }
}

static void ImportLine(const uint8_t* src, int src_stride,
                       uint8_t* dst, int len, int total_len) {
  int i;
  for (i = 0; i < len;_istream<unsigned short,std::char_traits<unsigned short> > S �  std::basic_istream<unsigned short,std::char_traits<unsigned short> >::sentry Y �  std::basic_istream<unsigned short,std::char_traits<unsigned short> >::_Sentry_base  �$  std::float_denorm_style  �$  std::float_round_style v 0<  std::_Tree_buy<std::pair<Imf::Name const ,Imf::Slice>,std::allocator<std::pair<Imf::Name const ,Imf::Slice> > > s �<  std::map<Imf::Name,Imf::Slice,std::less<Imf::Name>,std::allocator<std::pair<Imf::Name const ,Imf::Slice> > > ! s  std::hash<std::error_code> M D$  std::_String_iterator<std::_String_val<std::_Simple_types<wchar_t> > >  K.  std::map<Imf::Name,Imf::Attribute *,std::less<Imf::Name>,std::allocator<std::pair<Imf::Name const ,Imf::Attribute *> > >  �  std::allocator<char> . t  std::unary_function<float,unsigned int> z $  std::_Iterator012<std::random_access_iterator_tag,wchar_t,int,wchar_t const *,wchar_t const &,std::_Iterator_base0> T �  std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > � �*  std::_Iterator012<std::bidirectional_iterator_tag,std::pair<Imf::Name const ,Imf::Channel>,int,std::pair<Imf::Name const ,Imf::Channel> const *,std::pair<Imf::Name const ,Imf::Channel> const &,std::_Iterator_base0> 7 0  std::basic_ostream<char,std::char_traits<char> > ? Q#  std::basic_ostream<char,std::char_traits<char> >::sentry E C#  std::basic_ostream<char,std::char_traits<char> >::_Sentry_base J �;  std::pointer_traits<std::pair<Imf::Name const ,Imf::Slice> const *> % �(  std::integral_constant<bool,1>  h  generic_errno \ �3  std::money_put<wchar_t,std::ostreambuf_iterator<wchar_t,std::char_traits<wchar_t> > >  ?%  std::char_traits<char>  �2  std::money_base  ,3  std::money_base::pattern     streamoff  x$  std::bad_cast F L-  std::_Allocator_base<std::pair<Imf::Name const ,Imf::Channel> > , y  std::unary_function<int,unsigned int>    stringstream [ l4  std::time_put<wchar_t,std::ostreambuf_iterator<wchar_t,std::char_traits<wchar_t> > > ! %  std::numeric_limits<float> C �!  std::ostreambuf_iterator<wchar_t,std::char_traits<wchar_t> >  �(  std::output_iterator_tag  �-  std::less<Imf::Name> R ,4  std::time_put<char,std::ostreambuf_iterator<char,std::char_traits<char> > >  o"  std::_Nil # %  std::numeric_limits<__int64> 6 T%  std::allocator_traits<std::allocator<wchar_t> >  5  std::_Tuple_alloc_t  �  std::system_error = �   std::istreambuf_iterator<char,std::char_traits<char> >  �  std::invalid_argument  '3  std::moneypunct<char,1> t �@  std::allocator_traits<std::allocator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > > � *  std::_Tree_unchecked_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Attribute *> > >,std::_Iterator_base0>   )+  std::_Rand_urng_from_func  "   _Atomic_counter_t    std::hash<float> > �!  std::basic_iostream<wchar_t,std::char_traits<wchar_t> > k �-  std::_Wrap_alloc<std::allocator<std::_Tree_node<std::pair<Imf::Name const ,Imf::Channel>,void *> > >  �  std::allocator_arg_t    std::codecvt_base 5 �-  std::binary_function<Imf::Name,Imf::Name,bool> � e-  std::_Tmap_traits<Imf::Name,Imf::Channel,std::less<Imf::Name>,std::allocator<std::pair<Imf::Name const ,Imf::Channel> >,0>    nullptr_t 8 �  std::basic_iostream<char,std::char_traits<char> > c �1  std::initializer_list<std::basic_string<char,std::char_traits<char>,std::allocator<char> > >  �  std::memory_order 3 �  std::basic_ios<char,std::char_traits<char> >  W   std::numpunct<char> � �5  std::_Tree_unchecked_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Slice> > >,std::_Iterator_base0>  �  std::_Container_proxy  S  std::underflow_error B b  std::iterator<std::output_iterator_tag,void,void,void,void> t 	6  std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Slice> > > > b �1  std::_Allocator_base<std::basic_string<char,std::char_traits<char>,std::allocator<char> > >  �  std::bad_exception { ^  std::num_put<char,std::back_insert_iterator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > > > <  std::allocator<std::pair<Imf::Name const ,Imf::Slice> >   %  std::numeric_limits<bool> � �?  std::_Vector_alloc<0,std::_Vec_base_types<std::basic_string<char,std::char_traits<char>,std::allocator<char> >,std::allocator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > > >  �  std::_System_error * 
%  std::numeric_limits<unsigned short>  v  std::_Container_base0    std::logic_error X �;  std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Slice> > > ` �;  std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Slice> > >::_Redbl � �5  std::_Iterator012<std::bidirectional_iterator_tag,std::pair<Imf::Name const ,Imf::Slice>,int,std::pair<Imf::Name const ,Imf::Slice> const *,std::pair<Imf::Name const ,Imf::Slice> const &,std::_Iterator_base0> 7 &  std::basic_istream<char,std::char_traits<char> > ? B  std::basic_istream<char,std::char_traits<char> >::sentry E 3  std::basic_istream<char,std::char_traits<char> >::_Sentry_base � (<  std::_Tree_alloc<0,std::_Tree_base_types<std::pair<Imf::Name const ,Imf::Slice>,std::allocator<std::pair<Imf::Name const ,Imf::Slice> > > > z �,  std::_Tree_buy<std::pair<Imf::Name const ,Imf::Channel>,std::allocator<std::pair<Imf::Name const ,Imf::Channel> > > � �,  std::_Tree_alloc<0,std::_Tree_base_types<std::pair<Imf::Name const ,Imf::Channel>,std::allocator<std::pair<Imf::Name const ,Imf::Channel> > > > " �(  std::initializer_list<char>  �   std::numpunct<wchar_t> C @!  std::istreambuf_iterator<wchar_t,std::char_traits<wchar_t> > 5 ,  std::_String_val<std::_Simple_types<wchar_t> > < #  std::_String_val<std::_Simple_types<wchar_t> >::_Bxty G l2  std::initializer_list<std::pair<Imf::Name const ,Imf::Channel> >  �!  std::allocator<wchar_t>  �(  true_type Q 4  std::_String_alloc<0,std::_String_base_types<char,std::allocator<char> > > ) %  std::numeric_limits<unsigned long> z **  std::_Tree_const_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Attribute *> > > > \ �3  std::money_get<wchar_t,std::istreambuf_iterator<wchar_t,std::char_traits<wchar_t> > > F �,  std::pointer_traits<std::pair<Imf::Name const ,Imf::Channel> *> Z �,  std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Channel> > > b �,  std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Channel> > >::_Redbl  �  std::_Uninitialized  �$  std::_Num_int_base  +  std::_Iosb<int>   &  std::_Iosb<int>::_Seekdir ! #  std::_Iosb<int>::_Openmode      std::_Iosb<int>::_Iostate !   std::_Iosb<int>::_Fmtflags #   std::_Iosb<int>::_Dummy_enum 4 �  std::unary_function<long double,unsigned int> Q   std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > , Q  std::codecvt<unsigned short,char,int> S y3  std::money_put<char,std::ostreambuf_iterator<char,std::char_traits<char> > >  _  std::error_condition � ;<  std::_Tree_comp<0,std::_Tmap_traits<Imf::Name,Imf::Slice,std::less<Imf::Name>,std::allocator<std::pair<Imf::Name const ,Imf::Slice> >,0> > ! �  std::_Bitwise_hash<double> = �   std::ostreambuf_iterator<char,std::char_traits<char> >    std::exception_ptr  %  std::numeric_limits<int>  0   _Bool # �  std::_Generic_error_category t =*  std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Attribute *> > > >  �$  float_denorm_style   �$  std::forward_iterator_tag � @  std::vector<std::basic_string<char,std::char_traits<char>,std::allocator<char> >,std::allocator<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > >  j  std::io_errc  |(  false_type � �<  std::_Tree<std::_Tmap_traits<Imf::Name,Imf::Slice,std::less<Imf::Name>,std::allocator<std::pair<Imf::Name const ,Imf::Slice> >,0> > N G"  std::basic_stringbuf<char,std::char_traits<char>,std::allocator<char> > $ �  std::_Iostream_error_category  �  std::_Iterator_base0 @ b-  std::allocator<std::pair<Imf::Name const ,Imf::Channel> >  �  std::ctype<wchar_t> �  -  std::_Tree<std::_Tmap_traits<Imf::Name,Imf::Channel,std::less<Imf::Name>,std::allocator<std::pair<Imf::Name const ,Imf::Channel> >,0> > L �,  std::pointer_traits<std::pair<Imf::Name const ,Imf::Channel> const *> G q,  std::_Tree_node<std::pair<Imf::Name const ,Imf::Channel>,void *> D �;  std::pointer_traits<std::pair<Imf::Name const ,Imf::Slice> *>  3  std::_Wrap_int ' 8%  std::char_traits<unsigned short>   �$  std::numeric_limits<char> 3 J%  std::allocator_traits<std::allocator<char> > i �<  std::_Wrap_alloc<std::allocator<std::_Tree_node<std::pair<Imf::Name const ,Imf::Slice>,void *> > > S [3  std::money_get<char,std::istreambuf_iterator<char,std::char_traits<char> > >  �  std::domain_error " %  std::numeric_limits<double> ' %  std::numeric_limits<long double> - 36  std::pair<Imf::Name const ,Imf::Slice> / �*  std::pair<Imf::Name const ,Imf::Channel> ! l  std::piecewise_construct_t s �?  std::_Vector_val<std::_Simple_types<std::basic_string<char,std::char_traits<char>,std::allocator<char> > > >  �3  std::_Mpunct<wchar_t>  %  std::_Num_float_base ! �3  std::moneypunct<wchar_t,0> Z ,!  std::num_get<wchar_t,std::istreambuf_iterator<wchar_t,std::char_traits<wchar_t> > >   �  std::bad_array_new_length  �  std::_Container_base12 D �;  std::_Allocator_base<std::pair<Imf::Name const ,Imf::Slice> >  �  std::hash<double> ! �  std::_Allocator_base<char>  "   _Atomic_integral_t  A  std::overflow_error J �,  std::pointer_traits<std::pair<Imf::Name const ,Imf::Attribute *> *>  �  std::_Yarn<char> !   std::ctype<unsigned short> n 6  std::_Tree_iterator<std::_Tree_val<std::_Tree_simple_types<std::pair<Imf::Name const ,Imf::Slice> > > >  v  _Container_base % �(  std::initializer_list<wchar_t>  A3  std::moneypunct<char,0>  d  std::range_error  �2  std::time_base   �2  std::time_base::dateorder p �  std::back_insert_iterator<std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > >     streamsize V �<  std::allocator<std::_Tree_node<std::pair<Imf::Name const ,Imf::Slice>,void *> > \ �<  std::_Allocator_base<std::_Tree_node<std::pair<Imf::Name const ,Imf::Slice>,void *> >  �  std::exception    std::_Yarn<wchar_t> � �  std::num_put<wchar_t,std::back_insert_iterator<std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > > > 9 �  std::basic_ios<wchar_t,std::char