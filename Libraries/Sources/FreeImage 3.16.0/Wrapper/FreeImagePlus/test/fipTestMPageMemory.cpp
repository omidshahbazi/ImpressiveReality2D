// ==========================================================
// FreeImagePlus Test Script
//
// Design and implementation by
// - Herv� Drolon (drolon@infonie.fr)
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


#include "fipTest.h"

using namespace std;

// --------------------------------------------------------------------------

static BOOL  
loadBuffer(const char *lpszPathName, BYTE **buffer, DWORD *length) {
	struct stat file_info;
	int result;

	// get data associated with lpszPathName
	result = stat(lpszPathName, &file_info);
	if(result == 0) {
		// allocate a memory buffer and load temporary data
		*buffer = (BYTE*)malloc(file_info.st_size * sizeof(BYTE));
		if(*buffer) {
			FILE *stream = fopen(lpszPathName, "rb");
			if(stream) {
				*length = (DWORD)fread(*buffer, sizeof(BYTE), file_info.st_size, stream);
				fclose(stream);
				
				return TRUE;
			}
		}
	}

	return FALSE;
}

static BOOL 
extractPagesFromMemory(FREE_IMAGE_FORMAT fif, fipMemoryIO& memIO) {
	BOOL bMemoryCache = TRUE;

	char filename[256];
	fipImage image;

	// open the multipage bitmap stream as read-only
	fipMultiPage src(bMemoryCache);

	src.open(memIO);

	if(src.isValid()) {
		// get the page count
		int count = src.getPageCount();
		// extract all pages
		for(int page = 0; page < count; page++) {
			// load the bitmap at position 'page'
			image = src.lockPage(page);
			if(image.isValid()) {
				// save the page
				sprintf(filename, "page%d.%s", page, FreeImage_GetFormatFromFIF(fif));
				image.save(filename, 0);
				// Unload the bitmap (do not apply any change to src)
				src.unlockPage(image, FALSE);
			} else {
				// an error occured: free the multipage bitmap handle (fipMultiPage destructor) and return
				src.close(0);
				return FALSE;
			}
		}
	}
	// make sure to close the multipage bitmap handle on exit (fipMultiPage destructor or direct call to src.close(0))
	return src.close(0);
}

BOOL testLoadMultiBitmapFromMemory(const char *lpszPathName) {
	BOOL bSuccess = FALSE;

	BYTE *buffer = NULL;
	DWORD buffer_size = 0;

	// load source stream as a buffer, i.e. 
	// allocate a memory buffer and load temporary data
	bSuccess = loadBuffer(lpszPathName, &buffer, &buffer_size);
	assert(bSuccess);

	if(bSuccess) {
		// attach the binary data to a memory stream
		fipMemoryIO memIO(buffer, buffer_size);

		// get the file type
		FREE_IMAGE_FORMAT fif = memIO.getFileType();

		// extract pages
		bSuccess = extractPagesFromMemory(fif, memIO);
		assert(bSuccess);

		// close the memory stream (memIO destructor)
	}

	// user is responsible for freeing the data
	free(buffer);

	return bSuccess;
}

// --------------------------------------------------------------------------

BOOL testSaveMultiBitmapToMemory(const char *input, const char *output, int output_flag) {
	BOOL bSuccess;

	BOOL bCreateNew = FALSE;
	BOOL bReadOnly = TRUE;
	BOOL bMemoryCache = TRUE;

	// Open src file (read-only, use memory cache)
	fipMultiPage src(bMemoryCache);
	src.open(input, bCreateNew, bReadOnly, 0);

	if(src.isValid()) {
		// open and allocate a memory stream
		fipMemoryIO memIO;
		
		// save the file to memory
		FREE_IMAGE_FORMAT fif = fipImage�I��8` W�*          p���|    P�E�     |T�	X�	\�	`�	d�E�     |h�	l�	@�	@�	@�	@�	@�	@�	@�	@�	#@�	'@�	+@�	/@�	3@�	7@�	;@�	?@�	C@�	G@�	K@�	O@�	S@�	W@�	[@�	_@�	c@�	g@�	k@�	`�	`�	
`�E�     |`�	`�	`�	`�	`�	"`�	&`�	*`�	.`�	2`�	6`�	:`�	>`�	B`�	F`�	J`�	N`�	R`�	V`�	Z`�	^`�	b`�	f`�	j`�	��	��		��	��	��	��	��	��E�     |!��	%��	)��	-��	1��	5��	9��	=��	A��	E��	I��	M��	Q��	U��	Y��	]��	a��	e��	i��	 ��	��	��	��	��	��	��	��	 ��	$��	(��	,��	0��E�     |4��	8��	<��	@��	D��	H��	L��	P��	T��	X��	\��	`��	d��	h��	l��	��	��	��	��E�     |��E�     |��E�     |��E�     |��E�     |#��E�     |'��E�     |+��E�     |/��E�     |3��E�     |7��E�     |;��E�     |?��E�     |C��E�     |G��E�     |K��E�     |O��E�     |S��E�     |W��E�     |[��E�     |_��E�     |c��E�     |g��E�     |k��E�     | �E�     | �E�     |
 �E�     | �E�     | �E�     | �E�     | �E�     | �E�     |! �E�     |$ �E�     |' �E�     |* �E�     |- �E�     |0 �E�     |3 �E�     |6 �E�     |9 �E�     |< �E�     |J �E�     |N �E�     |R �E�     |V �E�     |Z �E�     |^ �E�     |b �E�     |f �E�     |j �E�     |�E�     |�E�     |�E�     |#�E�     |'�E�     |+�E�     |/�E�     |3�E�     |7�E�     |;�E�     |?�E�     |C�E�     |G�E�     |K�E�     |O�E�     |S�E�     |W�E�     |[�E�     |_�E�     |c�E�     |g�E�     |k�E�     |P�E�     |P�E�     |00�E�     |30�E�     |70�E�     |;0�E�     |>0�E�     |A0�E�     |D0�E�     |G0�E�     |K0�E�     |N0�E�     |R0�E�     |V0�E�     |Z0�E�     |]0�E�     |a0�E�     |e0�E�     |i0�E�     | p�E�     |p�E�     |p�E�     |p�E�     |p�E�     |p�E�     |p�E�     |p�E�     | p�E�     |$p�E�     |(p�E�     |,p�E�     |2p�E�     |6p�E�     |�L}�L	
�L	�L	�L	�L	�L	�L	�L	�L	"�L	��	_��	b��	e��	h��	k��	 �	 �	 �	
 �	 �	 �	P�	d {	g {	j {	 0{	0{	0{		0{	0{	0{	0{	0{	e�~	RP	UP	XP	[P	^P	aP	dP	gP	jP	 �	�	0�E�     }%�L~H�L	k�L	!�M	D�M	g�M	 N	@ N	c N	@N	<@N	0O	0O	$0O	+0O	20O	90O	@0O	G0O	N0O	U0O	\0O	��	L��	��	��	��	��	!��	$��	'��	*��	-��	0��	3��	6��	:��	>��	B��	F��	J��	N��	R��	V��	Z��	^��	b��	f��	j��	��	��		��	��	��	��	��	��	!��	$��	'��	*��	-��	0��	3��	6��	9��	<��	?��	B��	E��	H��	K��	N��	Q��	T��	W��	Z��	]��	`��	c��	f��	i��	l��	 �	 �	 �	 �	 �	 �	 �	 �	 �	 �	  �	# �	& �	) �	, �	/ �	2 �	5 �	8 �	< �	@ �	D �	H �	L �	P �	T �	X �	\ �	` �	d �	g �	j �	 0�	0�	0�	                                                                                                                                                                                                                                                                                                                                                                                                                                    ��Q����a����q�!�ѯ��1�ᮑ�A�񭡭Q����a����q�!�Ѫ��1�ᩑ�A�񨡨Q����a����q�!�ѥ��1�ᤑ�A�񣡣Q����a����q�!�Ѡ��1�៑�A�񞡞Q����a����q�!�ћ��1�ᚑ�A�񙡙Q����a����q�!�і��1�ᕑ�A�񔡔Q����a����q�!�ё��1�ᐑ�A�񏡏Q����a��������A�񋡋Q����a����q�!�ш��1�ᇑ�A�񆡆Q����a����q�!�у��1�႑�A�񁡁Q����a���q!�~�~!~P}�|�{�zz@ypx�w�v v0u`t�s�r�q qPp�o�n�mm@lpk�j�i i0h`g�f�e�d dPc�b�a�``@_p^�]�\ \0[`Z�Y�X�W WPV�U�T�S