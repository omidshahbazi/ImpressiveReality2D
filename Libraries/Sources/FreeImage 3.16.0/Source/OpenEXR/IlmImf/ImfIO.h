8P±.>` &           új0ó    À¯ Eq
|~DQ6!D~2¦!H 	Ð³ +àÔ 2%, àÔ |~  ë 0s ¤2, !ë 0s D! 0s ¦|##! 0s D! 	0s §|##! 
0s D! ² 
	|¦ ¶  	 0 ÐÐîD
!D2D~|2pD!D2p!¦DQ6 ÐîC,2# Ðî,D2p#| 	`í D
|H|Dp#|Q 
kEq
|D!¢2
!DH2p6 à 	¤ À 	¤   Ò ¢ 	%3, !PcmEq
	%3, %1!#3, @7P:à: <P=p=ð= ?àB0C
ÐFHH HÐH^
°^`_`bf	0rrr@|" ÀP Eq
!#3, À.Eq
!#3, `/Eq
!#3,  2Eq
!#3, `IEq
!#3,  nEq
!#3,  µ	p~!p @µ$0º$00Eq
§~! 00Eq
§~! 	00Eq
§~! 00Eq
§~! P0Eq
§~! ð=	¤p3, p^°/Eq
¤p3, p=p2Q!3, H°^r` Eq
p2Q!3, 
p¯ ¤D2p2D	H!  ± %0é§~D!D6!§D! PüEq
§~D

QH2¢!D2Q!| 	úEq
§~D
2!DHH|¤Dp
§~D6!§D#H% ú
2!D

,!D#H% ú!|Q~!D6!§D#H% úp
§~D6!§D#H%DQ6 úp|D,!H2p% úEq
§~D
2!D|pH§Dp
§~D6!§D#H% 
úEq
§~D
2!D~!#!Dp
§~D6!§D#H% ú2H!pD#H% ú!D~|¢D2p#|D#H% ú¤2p|¤D,pH!D#H% @é!D(D6!§D! @é"$D6!§D! PüEq
§~D2D!2!¢H êH%|2,QD2!p2#2!  ¿Eq
§~DHHD#p
2|p  ¿Eq
§~DHHD~|¢2! 	 ¿Eq
§~Dp§ àç§~D
,2¢	H! 0è! `ìpD!p
|2p% PüEq
§~D§p
D!2!¢H üEq
§~D§p
D!2!¢HD
|Q~H!2|p PéD	H|	 ÐÁEq
§~DD	H|	 H Ðù'!¦D|D!H!!D#H% Ðù/
!D2%pD|2 Ðù/QH2D
!D2%pD|2 Ðù;|2D~ Ðù/~|
!D#H% °ñ|2  èH!!D!#H àçH!!6!§! PéD	H|	 D	H|	 ãAáß±ÝÜ!Ú!Ø¡ÕQÓÁÏÌÊ!Ç±Å1ÄÁÀðëÁéÑæãáQÝÙpÇðÃqÁÀº ¶³±A®à« §`£ÑñÁñ!¡0~ wÑsqn!jÁe^ðXáTTaRpP!PÑOOñM@LñKÁJÀHÀFÀDÀBÀ@q@!@!>P<:°8à65@3ñ2¡2Q22±1a11Á0q0!0Ñ//1/á..A.ñ-¡-Q--±,a,,Á+q+!+ñ)@(ñ'¡&Q&a%$À!ÑÁA1áÁqqQá
Á	Q1á±aqAñ   to read() will
    // read the first byte in the file, tellg() returns 0.
    //--------------------------------------------------------

    virtual Int64	tellg () = 0;


    //-------------------------------------------
    // Set the current reading position.
    // After calling seekg(i), tellg() returns i.
    //-------------------------------------------

    virtual void	seekg (Int64 pos) = 0;


    //------------------------------------------------------
    // Clear error conditions after an operation has failed.
    //------------------------------------------------------

    virtual void	clear ();


    //------------------------------------------------------
    // Get the name of the file associated with this stream.
    //------------------------------------------------------

    const char *	fileName () const;

  protected:

    IStream (const char fileName[]);

  private:

    IStream (const IStream &);			// not implemented
    IStream & operator = (const IStream &);	// not implemented

    std::string		_fileName;
};


//-----------------------------------------------------------
// class OStream -- an abstract base class for output streams
//-----------------------------------------------------------

class OStream
{
  public:

    //-----------
    // Destructor
    //-----------

    virtual ~OStream ();
  

    //----------------------------------------------------------
    // Write to the stream:
    //
    // write(c,n) takes n bytes from array c, and stores them
    // in the stream.  If an I/O error occurs, write(c,n) throws
    // an exception.
    //----------------------------------------------------------

    virtual void	write (const char c[/*n*/], int n) = 0;


    //---------------------------------------------------------
    // Get the current writing position, in bytes from the
    // beginning of the file.  If the next call to write() will
    // start writing at the beginning of the file, tellp()
    // returns 0.
    //---------------------------------------------------------

    virtual Int64	tellp () = 0;


    //-------------------------------------------
    // Set the current writing position.
    // After calling seekp(i), tellp() returns i.
    //-------------------------------------------

    virtual void	seekp (Int64 pos) = 0;


    //------------------------------------------------------
    // Get the name of the file associated with this stream.
    //------------------------------------------------------

    const char *	fileName () const;

  protected:

    OStream (const char fileName[]);

  private:

    OStream (const OStream &);			// not implemented
    OStream & operator = (const OStream &);	// not implemented

    std::string		_fileName;
};


//-----------------------
// Helper classes for Xdr
//-----------------------

struct StreamIO
{
    static void
    writeChars (OStream &os, const char c[/*n*/], int n)
    {
	os.write (c, n);
    }

    static bool
    readChars (IStream &is, char c[/*n*/], int n)
    {
	return is.read (c, n);
    }
};


struct CharPtrIO
{
    static void
    writeChars (char *&op, const char c[/*n*/], int n)
    {
	while (n--)
	    *op++ = *c++;
    }

    static bool
    readChars (const char *&ip, char c[/*n*/], int n)
    {
	while (n--)
	    *c++ = *ip++;

	return true;
    }
};


} // namespace Imf

#endif
