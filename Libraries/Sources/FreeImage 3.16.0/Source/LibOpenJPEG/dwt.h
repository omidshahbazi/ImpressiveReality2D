        �             H        
   �             �         �     Z G            �      �   �         Imf::TypedAttribute<Imf::PreviewImage>::writeValueTo                       @  �>   this     �  os     t   version  95       �   9c       �   9�       �   9�       �   9�       �   9�       �   O�   �           �   �     t       ;  �   <  �8   =  �f   ?  �m   @  �p   B  �t   <  ��   D  ��   E  ��   F  ��   G  ��   B  ��   I  ��   $           #            �  )     �   �   d        __ehhandler$?readValueFrom@?$TypedAttribute@VPreviewImage@Imf@@@Imf@@UAEXAAVIStream@2@HH@Z j        __unwindfunclet$?readValueFrom@?$TypedAttribute@VPreviewImage@Imf@@@Imf@@UAEXAAVIStream@2@HH@Z$0   �   <  [ G            #  )     �         Imf::TypedAttribute<Imf::PreviewImage>::readValueFrom                      @ 
 :����    �>   this     �  is     t   size     t   version  ����:>  p  98       �   9i       �   9�       �   9�       �   9�       �   9�       �   O�   �           #  �     �       O  �)   R  �\   S  �`   R  �b   S  ��   U  ��   S  ��   U  ��   W  ��   Z  ��   R  ��   \  ��   ]  ��   ^  ��   _  ��   Z  ��   b  ��   c  ��       �        allocator_arg �   "     l        piecewise_construct   �       �$    denorm_absent �   o    �$   denorm_present  �$    round_toward_zero  �$   round_to_nearest  �   memory_order_seq_cst  �   #3   "   _Uint32t  �%  __RTTIBaseClassArray - �(  $_s__CatchableTypeArray$_extraBytes_28  ''  _CatchableTypeArray - y%  $_s__RTTIBaseClassArray$_extraBytes_16 $ r%  _s__RTTICompleteObjectLocator  B  localeinfo_struct  #   Int64 ! �$  std::_Mutable_iterator_tag & �$  std::bidirectional_iterator_tag / �  std::unary_function<double,unsigned int> ? ?  std::basic_streambuf<wchar_t,std::char_traits<wchar_t> >    std::length_error  0  std::runtime_error 1 "  std::_Wrap_alloc<std::allocator<wchar_t> > D �(  std::_Wrap_alloc<std::allocator<wchar_t> >::rebind<wchar_t *> B �(  std::_Wrap_alloc<std::allocator<wchar_t> >::rebind<wchar_t>  �  std::ctype<char>  �  std::_Iterator_base12 ( %  std::numeric_limits<unsigned int>  �  std::allocator<void>    std::ctype_byname<char>   )%  std::char_traits<wchar_t>  ]  std::_Facet_base K �  std::basic_istream<unsigned short,std::char_traits<unsigned short> > S �  std::basic_istream<unsigned short,std::char_traits<unsigned short> >::sentry Y �  std::basic_istream<unsigned short,std::char_traits<unsigned short> >::_Sentry_base  �$  std::float_denorm_style  �$  std::float_round_style ! s  std::hash<std::error_code> M D$  std::_String_iterator<std::_String_val<std::_Simple_types<wchar_t> > >  �  std::allocator<char> . t  std::unary_function<float,unsigned int> z $  std::_Iterator012<std::random_access_iterator_tag,wchar_t,int,wchar_t const *,wchar_t const &,std::_Iterator_base0> T �  std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > 7 0  std::basic_ostream<char,std::char_traits<char> > ? Q#  std::basic_ostream<char,std::char_traits<char> >::sentry E C#  std::basic_ostream<char,std::char_traits<char> >::_Sentry_base  h  generic_errno  ?%  std::char_traits<char>     streamoff  x$  std::bad_cast , y  std::unary_function<int,unsigned int>    stringstream ! %  std::numeric_limits<float> C �!  std::ostreambuf_iterator<wchar_t,std::char_traits<wchar_t> >  �(  std::output_iterator_tag  o"  std::_Nil # %  std::numeric_limits<__int64> 6 T%  std::allocator_traits<std::allocator<wchar_t> >  �  std::system_error = �   std::istreambuf_iterator<char,std::char_traits<char> >  �  std::invalid_argument  "   _Atomic_counter_t    std::hash<float> > �!  std::basic_iostream<wchar_t,std::char_traits<wchar_t> >  �  std::allocator_arg_t    std::codecvt_base    nullptr_t 8 �  std::basic_iostream<char,std::char_traits<char> >  �  std::memory_order 3 )
@return Returns the gain of the 9-7 wavelet transform
*/
OPJ_UINT32 opj_dwt_getgain_real(OPJ_UINT32 orient);
/**
Get the norm of a wavelet function of a subband at a specified level for the irreversible 9-7 DWT
@param level Level of the wavelet function
@param orient Band of the wavelet function
@return Returns the norm of the 9-7 wavelet
*/
OPJ_FLOAT64 opj_dwt_getnorm_real(OPJ_UINT32 level, OPJ_UINT32 orient);
/**
Explicit calculation of the Quantization Stepsizes 
@param tccp Tile-component coding parameters
@param prec Precint analyzed
*/
void opj_dwt_calc_explicit_stepsizes(opj_tccp_t * tccp, OPJ_UINT32 prec);
/* ----------------------------------------------------------------------- */
/*@}*/

/*@}*/

#endif /* __DWT_H */
