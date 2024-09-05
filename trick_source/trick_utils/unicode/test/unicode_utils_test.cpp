#include	<stddef.h>
#include	<stdio.h>
#include	<string.h>
#include	<wchar.h>
#include	<gtest/gtest.h>
#include	"trick/unicode_utils.h"

int	main(int	argc,	char	**argv)	{
		::testing::InitGoogleTest(&argc,	argv);
		return	RUN_ALL_TESTS();
}

const	char*	ISO_6429_Restore_Default			=	"\x1b[00m";
const	char*	ISO_6429_Bold														=	"\x1b[01m";
const	char*	ISO_6429_Underline									=	"\x1b[04m";
const	char*	ISO_6429_Black_Foreground		=	"\x1b[30m";
const	char*	ISO_6429_Red_Foreground				=	"\x1b[31m";
const	char*	ISO_6429_Green_Foreground		=	"\x1b[32m";
const	char*	ISO_6429_Yellow_Foreground	=	"\x1b[33m";
const	char*	ISO_6429_Blue_Foreground			=	"\x1b[34m";
const	char*	ISO_6429_Purple_Foreground	=	"\x1b[35m";
const	char*	ISO_6429_Cyan_Foreground			=	"\x1b[36m";
const	char*	ISO_6429_White_Foreground		=	"\x1b[37m";
const	char*	ISO_6429_Black_Background		=	"\x1b[40m";
const	char*	ISO_6429_Red_Background				=	"\x1b[41m";
const	char*	ISO_6429_Green_Background		=	"\x1b[42m";
const	char*	ISO_6429_Yellow_Background	=	"\x1b[43m";
const	char*	ISO_6429_Blue_Background			=	"\x1b[44m";
const	char*	ISO_6429_Purple_Background	=	"\x1b[45m";
const	char*	ISO_6429_Cyan_Background			=	"\x1b[46m";
const	char*	ISO_6429_White_Background		=	"\x1b[47m";

void	Error_Message_Expected()	{
				printf("%s%s%s",	ISO_6429_Blue_Background,	ISO_6429_White_Foreground,	ISO_6429_Underline);
				printf("An	error	message	is	expected	from	this	test.");
				printf("%s\n",	ISO_6429_Restore_Default	);
}

/*	The	following	are	the	utf-8	encodings	of	four	unicode	characters	used	in	the	following	tests.	*/
//	Greek	Phi	Symbol	=>	U+03d5	=>	0xcf	0x95																						//	see:	https://www.compart.com/en/unicode/U+03D5
//	Superscript	Latin	Small	Letter	I	=>	U+2071	=>	0xe2	0x81	0xb1	//	see:	https://www.compart.com/en/unicode/U+2071
//	Modifier	Letter	Small	Greek	Phi		=>	U+1D60	=>	0xe1	0xb5	0xa0	//	see:	https://www.compart.com/en/unicode/U+1D60
//	Aegean	Number	Ten	=>	U+10110	=>	0xf0	0x90	0x84	0x90										//	see:	https://www.compart.com/en/unicode/U+10110

//	-------------------------------------------------------
//	Test	suite	for	ucodepoint_to_utf32()
//	-------------------------------------------------------
TEST(ucodepoint_to_utf32,	valid	)	{
				/*	Aegean	Number	Ten,	U+10110	is	a	valid	code	point.	*/
				int32_t	out;
				size_t	size	=	ucodepoint_to_utf32(0x10110,	&out);
				EXPECT_EQ(1,	size);
}

TEST(ucodepoint_to_utf32,	invalid	)	{
				/*	0xdead	is	in	the	range	[d800	..	dfff],	and	reserved	for	UTF-16
							surrogates.	They	are	not	valid	unicode	codepoints.	So,	if	we
							attempt	to	convert	a	surrogate	as	a	codepoint,	we	should	get
							an	error	meassage.
					*/
				int32_t	out;
				Error_Message_Expected();
				size_t	size	=	ucodepoint_to_utf32(0xdead,	&out);
				EXPECT_EQ(0,	size);
}

//	-------------------------------------------------------
//	Test	suite	for	ucodepoint_to_utf16()
//	-------------------------------------------------------
TEST(ucodepoint_to_utf16,	two_16bit_element_sequence	)	{
				/*	Note	that	unicode	is	a	21-bit	encoding.
							Because	Aegean	Number	Ten	(U+10110)	is	larger	than	can	be	stored	in	16-bits,
							UTF-16	requires	two	16-bit	values,	called	surrogates	to	encode	it.
							The	high-surrogate	"carries"	the	most	significant	11	bits	of	the	codepoint.
							High-surrogate	=	0xd800	+	most	significant	11	bits	of	the	codepoint.
							The	low-surrogate	carries	the	least	significant	10	bits	of	the	codepoint.
							Low-surrogate	=	0xde00	+	least	significant	10	bits	of	the	codepoint.
				*/
				int16_t	out[2];
				size_t	size	=	ucodepoint_to_utf16(0x10110,	&out);
				EXPECT_EQ(2,	size);
				EXPECT_EQ((int16_t)0xd840,	out[0]);	
				EXPECT_EQ((int16_t)0xdd10,	out[1]);
}

TEST(ucodepoint_to_utf16,	one_16bit_element)	{
				int16_t	out[2];
				/*	A	valid	codepoint	that	can	be	stored	within	16-bits	should	be
							equal	to	its	UTF-16	character	value.	*/
				size_t	size	=	ucodepoint_to_utf16(0x03d5,	&out);
				EXPECT_EQ(1,	size);
				EXPECT_EQ((int16_t)0x03d5,	out[0]);	
}

TEST(ucodepoint_to_utf16,	invalid_surrogate	)	{
				/*	Input	codepoint	can	not	be	a	surrogate.	*/
				int16_t	out[2];
				Error_Message_Expected();
				size_t	size	=	ucodepoint_to_utf16(0xdead,	&out);
				EXPECT_EQ(0,	size);
}

TEST(ucodepoint_to_utf16,	codepoint_too_big	)	{
				/*	Input	codepoint	can	not	be	>	0x10ffff,	which	is	the	largest	valid	unicode	codepoint.	*/
				int16_t	out[2];
				Error_Message_Expected();
				size_t	size	=	ucodepoint_to_utf16(0x110000,	&out);
				EXPECT_EQ(0,	size);
}

//	-------------------------------------------------------
//	Test	suite	for	ucodepoint_to_utf8()
//	-------------------------------------------------------
TEST(ucodepoint_to_utf8,	four_8bit_element_sequence	)	{
				char	out[4];
				/*	Aegean	Number	Ten,	U+10110	is	a	valid	codepoint	that
							requires	four	bytes	to	encode	in	utf-8.	*/
				size_t	size	=	ucodepoint_to_utf8(0x10110,	&out);
				EXPECT_EQ(4,	size);
}

TEST(ucodepoint_to_utf8,	three_8bit_element_sequence	)	{
				char	out[4];
				/*	Superscript	Latin	Small	Letter	I,	U+2071	is	a	valid
							codepoint	that	requires	three	bytes	to	encode	in	utf-8.	*/
				size_t	size	=	ucodepoint_to_utf8(0x2071,	&out);
				EXPECT_EQ(3,	size);
}

TEST(ucodepoint_to_utf8,	two_8bit_element_sequence	)	{
				char	out[4];
				/*	Greek	Phi	Symbol,	U+03d5	is	a	valid	codepoint	that
							requires	two	bytes	to	encode	in	utf-8.	*/
				size_t	size	=	ucodepoint_to_utf8(0x03d5,	&out);
				EXPECT_EQ(2,	size);
}

TEST(ucodepoint_to_utf8,	ascii	)	{
				char	out[4];
				/*	Latin	Small	Letter	A,		U+0061	is	a	valid	codepoint	that
							requires	one	byte	to	encode	in	utf-8.	Below	0x7f,	Unicode
							and	ASCII	are	identical.	*/
				size_t	size	=	ucodepoint_to_utf8('a',	&out);
				EXPECT_EQ(1,	size);
}

//	-------------------------------------------------------
//	Test	suite	for	escape_to_ascii()
//	-------------------------------------------------------
TEST(escape_to_ascii,	null_input	)	{
				/*	Should	generate	error	message	if	input	character	pointer	is	NULL.	*/
				char	output[128];
				char*	null_ptr	=	(char*)0;
				Error_Message_Expected();
				size_t	size	=	escape_to_ascii(	null_ptr,	output,	sizeof(output));
				EXPECT_EQ(0,	size);
}

TEST(escape_to_ascii,	null_output	)	{
				/*	If	output	character	pointer	is	NULL,	still	determine	the	length.	*/
				char*	null_ptr	=	(char*)0;
				const	char*	input	=	"e\u2071\u1d60	=	cos(\u03d5)	+	i*sin(\u03d5)\n";
				size_t	expected_size	=	strlen	("e\\u2071\\u1d60	=	cos(\\u03d5)	+	i*sin(\\u03d5)\\n");
				size_t	size	=	escape_to_ascii(	input,	null_ptr,	size_t(5));
				EXPECT_EQ(expected_size,	size);
}

TEST(escape_to_ascii,	normal_1		)	{
				char	output[128];
				/*	escape_to_ascii()	should	escape	all	Unicode	and	non-printable	ASCII	characters.	*/
				const	char*	utf8_s	=	"e\u2071\u1d60	=	cos(\u03d5)	+	i*sin(\u03d5)\n";
				const	char*	expected_ascii_s	=	"e\\u2071\\u1d60	=	cos(\\u03d5)	+	i*sin(\\u03d5)\\n";
				size_t	size	=	escape_to_ascii(	utf8_s,	output,	sizeof(output));
				EXPECT_EQ(	strlen(expected_ascii_s),	size);
				EXPECT_STREQ(expected_ascii_s,	output);
}

TEST(escape_to_ascii,	normal_2		)	{
				char	output[256];
				/*	escape_to_ascii()	should	escape	all	Unicode	and	non-printable	ASCII	characters.	*/
				const	char	ascii[128]	=	{							'\x01','\x02','\x03','\x04','\x05','\x06','\x07','\x08','\x09','\x0a','\x0b','\x0c','\x0d','\x0e','\x0f',
																													'\x10','\x11','\x12','\x13','\x14','\x15','\x16','\x17','\x18','\x19','\x1a','\x1b','\x1c','\x1d','\x1e','\x1f',
																													'\x20','\x21','\x22','\x23','\x24','\x25','\x26','\x27','\x28','\x29','\x2a','\x2b','\x2c','\x2d','\x2e','\x2f',
																													'\x30','\x31','\x32','\x33','\x34','\x35','\x36','\x37','\x38','\x39','\x3a','\x3b','\x3c','\x3d','\x3e','\x3f',
																													'\x40','\x41','\x42','\x43','\x44','\x45','\x46','\x47','\x48','\x49','\x4a','\x4b','\x4c','\x4d','\x4e','\x4f',
																													'\x50','\x51','\x52','\x53','\x54','\x55','\x56','\x57','\x58','\x59','\x5a','\x5b','\x5c','\x5d','\x5e','\x5f',
																													'\x60','\x61','\x62','\x63','\x64','\x65','\x66','\x67','\x68','\x69','\x6a','\x6b','\x6c','\x6d','\x6e','\x6f',
																													'\x70','\x71','\x72','\x73','\x74','\x75','\x76','\x77','\x78','\x79','\x7a','\x7b','\x7c','\x7d','\x7e','\x7f',
																													'\x00'
																												};

				const	char*	expected_ascii_s	=	"\\x01\\x02\\x03\\x04\\x05\\x06\\a\\b\\t\\n\\v\\f"
																																			"\\r\\x0e\\x0f\\x10\\x11\\x12\\x13\\x14\\x15\\x16\\x17\\x18\\x19\\x1a\\x1b\\x1c\\x1d\\x1e\\x1f"
																																			"	!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\\x7f";

				size_t	size	=	escape_to_ascii(	ascii,	output,	sizeof(output));
				EXPECT_EQ(strlen(expected_ascii_s),	size);
				EXPECT_STREQ(expected_ascii_s,	output);
}

TEST(escape_to_ascii,	demotest	)	{
				char	output[128];

				/*	This	test	simply	demonstrates	that	the	following	UTF-8	string	(utf8_s),
							used	in	subsequent	tests,	is	a	well	formed	UTF-8	string.	*/

				const	char	utf8_s[11]	=	{'P','h','i','	','=','	','\xcf','\x95','\0'};
				const	char*	expected_ascii_s	=	"Phi	=	\\u03d5";

				size_t	size	=	escape_to_ascii(	utf8_s,	output,	sizeof(output));

				EXPECT_STREQ(expected_ascii_s,	output);
				EXPECT_EQ(strlen(expected_ascii_s),	size);
}

TEST(escape_to_ascii,	detect_corruption_1	)	{
				char	output[128];

				/*	The	input	string	is	deliberately	corrupted	with	a	spurious
							continuation	character.*/

				char	utf8_s[11]	=	{'P','h','i','	','=','	','\xcf','\x95','\0'};
				utf8_s[6]	=	'\x80';	/*	Deliberately	corrupt	the	UTF-8	string.	*/

				Error_Message_Expected();
				size_t	size	=	escape_to_ascii(	utf8_s,	output,	sizeof(output));

				EXPECT_EQ(0,	size);
}

TEST(escape_to_ascii,	detect_corruption_2	)	{
				char	output[128];

				/*	The	following	string	is	deliberately	corrupted:	0xcf	is	a	header
							for	a	two-byte	sequence,	it	should	be	followed	by	a	continuation
							byte	(most	significant	2	bits	are	10).	0x75	starts	with	01	*/

				char	utf8_s[11]	=	{'P','h','i','	','=','	','\xcf','\x95','\0'};
				utf8_s[7]	=	'\x75';	/*	Deliberately	corrupt	the	UTF-8	string.	*/

				Error_Message_Expected();
				size_t	size	=	escape_to_ascii(	utf8_s,	output,	sizeof(output));

				EXPECT_EQ(0,	size);
}

TEST(escape_to_ascii,	insufficient_result_array_size	)	{
				char	output[16];

				/*	If	the	output	array	pointer	is	not	NULL,	it	must	be	of	sufficient	size.	Here	it	is	not.	*/
				const	char*	utf8_s	=	"e\u2071\u1d60	=	cos(\u03d5)	+	i*sin(\u03d5)\n";
				Error_Message_Expected();
				size_t	size	=	escape_to_ascii(	utf8_s,	output,	sizeof(output));
				EXPECT_EQ(0,	size);
}

//	-------------------------------------------------------
//	Test	suite	for	unescape_to_utf8()
//	-------------------------------------------------------

TEST(unescape_to_utf8,	null_input	)	{
				/*	Should	generate	error	message	if	input	character	pointer	is	NULL.	*/
				char	output[128];
				char*	null_ptr	=	(char*)0;
				Error_Message_Expected();
				size_t	size	=	unescape_to_utf8(	null_ptr,	output,	sizeof(output));
				EXPECT_EQ(0,	size);
}

TEST(unescape_to_utf8,	null_output	)	{
				/*	Should	return	the	length	of	the	string	that	would	have	been	produced.	*/
				char*	null_ptr	=	(char*)0;
				const	char*	input	=	"e\\u2071\\u1d60	=	cos(\\u03d5)	+	i*sin(\\u03d5)\\n";
				size_t	expected_size	=	strlen("e\u2071\u1d60	=	cos(\u03d5)	+	i*sin(\u03d5)\n");
				size_t	size	=	unescape_to_utf8(	input,	null_ptr,	size_t(5));
				EXPECT_EQ(expected_size,	size);
}

TEST(unescape_to_utf8,	normal_1)	{
				/*	unescape_to_utf8()	should	un-escape	all	escaped	ASCII	and	escaped	unicode,
							producing	a	utf8	character	string.	It	should	also	return	the	length	of
							that	string.	*/
				char	actual_output[128];
				const	char*	input	=	"e\\u2071\\u1d60	=	cos(\\u03d5)	+	i*sin(\\u03d5)\\n";
				const	char*	expected_output	=	"e\u2071\u1d60	=	cos(\u03d5)	+	i*sin(\u03d5)\n";

				size_t	size	=	unescape_to_utf8(input,	actual_output,	sizeof(actual_output));

				EXPECT_EQ(	strlen(expected_output),	size);
				EXPECT_STREQ(expected_output,	actual_output);
}

TEST(unescape_to_utf8,	non_ascii_chars)	{
				char	actual_output[128];
				/*	
???
					*/
				const	char*	input	=	"eⁱᵠ	=	cos(ϕ)	+	i*sin(\\u03d5)\\n";

				Error_Message_Expected();
				size_t	size	=	unescape_to_utf8(input,	actual_output,	sizeof(actual_output));

				EXPECT_EQ(30,	size);
}

TEST(unescape_to_utf8,	insufficient_hex_digits_1)	{
				/*	The	\U	escape	code	expects	exactly	8	hexidecimal	digits	to	follow.
							If	fewer	than	8	are	present,	then	an	error	message	should	result.
							Note:	"\U10110"	will	fail	in	a	C/C++	literal	at	compile	time	too,
							because	it	is	incomplete.	It	should	be	"\U00010110".
					*/
				char	actual_output[128];
				const	char*	input	=	"Aegean	Number	Ten	=	\\U10110\n";

				Error_Message_Expected();
				size_t	size	=	unescape_to_utf8(input,	actual_output,	sizeof(actual_output));

				EXPECT_EQ(0,	size);
}

TEST(unescape_to_utf8,	insufficient_hex_digits_2)	{
				/*	The	\u	escape	code	expects	exactly	4	hexidecimal	digits	to	follow.
							If	fewer	than	4	are	present,	then	an	error	message	should	result.
							Note:	"\u3d5"	will	fail	in	a	C/C++	literal	at	compile	time	too,
							because	it	is	incomplete.	It	should	be	"\u03d5".
					*/
				char	actual_output[128];
				const	char*	input	=	"Phi	=	\\u3d5\n";

				Error_Message_Expected();
				size_t	size	=	unescape_to_utf8(input,	actual_output,	sizeof(actual_output));

				EXPECT_EQ(0,	size);
}

TEST(unescape_to_utf8,	insufficient_result_array_size)	{
				/*	The	result	array	must	be	of	sufficient	size.	If	it	isn't,	then	an	error
							message	should	be	emitted.
					*/
				char	actual_output[16];
				const	char*	input	=	"e\\u2071\\u1d60	=	cos(\\u03d5)	+	i*sin(\\u03d5)\\n";

				Error_Message_Expected();
				size_t	size	=	unescape_to_utf8(input,	actual_output,	sizeof(actual_output));

				EXPECT_EQ(0,	size);
}

//	-------------------------------------------------------
//	Test	suite	for	utf8_to_wchar()
//	-------------------------------------------------------
TEST(utf8_to_wchar,	null_input	)	{
				/*	Should	generate	error	message	if	input	character	pointer	is	NULL.	*/
				wchar_t	output[128];
				char*	null_ptr	=	(char*)0;
				Error_Message_Expected();
				size_t	size	=	utf8_to_wchar(	null_ptr,	output,	sizeof(output)/sizeof(wchar_t));	
				EXPECT_EQ(0,	size);
}

TEST(utf8_to_wchar,	null_output	)	{
				/*	Should	return	the	length	of	the	string	that	would	have	been	produced.	*/
				wchar_t*	null_ptr	=	(wchar_t*)0;
				const	char*	input	=	"e\u2071\u1d60	=	cos(\u03d5)	+	i*sin(\u03d5)";
				size_t	expected_size	=	wcslen(L"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)");
				size_t	size	=	utf8_to_wchar(	input,	null_ptr,	size_t(0));
				EXPECT_EQ(expected_size,	size);
}

/*	The	following	three	tests	demonstrate	three	different	ways	to
			create	the	same	input	string.	*/

TEST(utf8_to_wchar,	test1)	{
				wchar_t	resultant_wchar_s[128];
				const	char*	input	=	"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";
				const	wchar_t*	expected_wide_s	=	L"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";

				(void)	utf8_to_wchar(input,	resultant_wchar_s,	sizeof(resultant_wchar_s)/sizeof(wchar_t));	
				bool	test_result	=	(wcscmp(expected_wide_s,	expected_wide_s)	==	0);
				EXPECT_EQ(true,	test_result);
}

TEST(utf8_to_wchar,	test2)	{
				wchar_t	resultant_wchar_s[128];
				const	char*	input	=	"e\u2071\u1d60	=	cos(\u03d5)	+	i*sin(\u03d5)";
				const	wchar_t*	expected_wide_s	=	L"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";

				(void)	utf8_to_wchar(input,	resultant_wchar_s,	sizeof(resultant_wchar_s)/sizeof(wchar_t));	
				bool	test_result	=	(wcscmp(expected_wide_s,	expected_wide_s)	==	0);
				EXPECT_EQ(true,	test_result);
}

TEST(utf8_to_wchar,	test3)	{
				wchar_t	resultant_wchar_s[128];
				const	char	input[30]	=	{'e','\xe2','\x81','\xb1','\xe1','\xb5','\xa0','	','=','	',
																												'c','o','s','(','\xcf','\x95',')','	','+','	','i','*','s',
																												'i','n','(','\xcf','\x95',')','\0'};
				const	wchar_t*	expected_wide_s	=	L"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";

				(void)	utf8_to_wchar(input,	resultant_wchar_s,	sizeof(resultant_wchar_s)/sizeof(wchar_t));	
				bool	test_result	=	(wcscmp(expected_wide_s,	expected_wide_s)	==	0);
				EXPECT_EQ(true,	test_result);
}

TEST(utf8_to_wchar,	insufficient_result_array_size)	{
				wchar_t	resultant_wchar_s[16];
				const	char*	input	=	"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";

				Error_Message_Expected();
				size_t	size	=	utf8_to_wchar(input,	resultant_wchar_s,	sizeof(resultant_wchar_s)/sizeof(wchar_t));	
				EXPECT_EQ(0,	size);
}

TEST(utf8_to_wchar,	corrupted_input)	{
				wchar_t	resultant_wchar_s[128];
				char	input[30]	=	{'e','\xe2','\x81','\xb1','\xe1','\xb5','\xa0','	','=','	',
																						'c','o','s','(','\xcf','\x95',')','	','+','	','i','*','s',
																						'i','n','(','\xcf','\x95',')','\0'};

				/*	Deliberately	corrupt	input	by	changing	input[2]	to	not	being	a	continuation	byte.	*/
				input[2]	=	0x70;

				Error_Message_Expected();
				size_t	size	=	utf8_to_wchar(input,	resultant_wchar_s,	sizeof(resultant_wchar_s)/sizeof(wchar_t));	
				EXPECT_EQ(0,	size);
}

//	-------------------------------------------------------
//	Test	suite	for	wchar_to_utf8()
//	-------------------------------------------------------
TEST(wchar_to_utf8,	null_input	)	{
				/*	Should	generate	error	message	if	input	character	pointer	is	NULL.	*/
				wchar_t*	null_ptr	=	(wchar_t*)0;
				char	output[128];
				Error_Message_Expected();
				size_t	size	=	wchar_to_utf8(	null_ptr,	output,	sizeof(output)/sizeof(wchar_t));	
				EXPECT_EQ(0,	size);
}

TEST(wchar_to_utf8,	null_output	)	{
				/*	If	output	is	NULL,	still	generate	the	length	of	the	array	that	would	have	been	produced.	*/
				const	wchar_t*	input	=	L"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";
				char*	null_ptr	=	(char*)0;
				size_t	expected_size	=	strlen("eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)");
				size_t	size	=	wchar_to_utf8(	input,	null_ptr,	(size_t)0);	
				EXPECT_EQ(expected_size,	size);
}

TEST(wchar_to_utf8,	test1)	{
				/*	Should	convert	wchar_t	array	to	a	UTF-8	array.	*/
				char	resultant_utf8_s[128];
				const	wchar_t*	wide_s	=	L"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";
				const	char*	expected_utf8_s	=	"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";

				wchar_to_utf8(wide_s,	resultant_utf8_s,	sizeof(resultant_utf8_s)/sizeof(char));	
				bool	test_result	=	(strcmp(expected_utf8_s,	resultant_utf8_s)	==	0);
				EXPECT_EQ(true,	test_result);
}

TEST(wchar_to_utf8,	test2)	{
				/*	Same	test	as	above,	but	input	is	a	constrained	array.	A	variant	fo	this	is	used	below.*/
				char	resultant_utf8_s[128];
				wchar_t	wide_s[32]	=	{	L'e',	L'ⁱ',	L'ᵠ',	L'	',	L'=',	L'	',	L'c',	L'o',	L's',	L'(',	L'ϕ',	L')',
																											L'	',	L'+',	L'	',	L'i',	L'*',	L's',	L'i',	L'n',	L'(',	L'ϕ',	L')'	};
				const	char*	expected_utf8_s	=	"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";

				wchar_to_utf8(wide_s,	resultant_utf8_s,	sizeof(resultant_utf8_s)/sizeof(char));	
				bool	test_result	=	(strcmp(expected_utf8_s,	resultant_utf8_s)	==	0);
				EXPECT_EQ(true,	test_result);
}

TEST(wchar_to_utf8,	invalid_unicode)	{
				/*	Should	detect	an	invalid	unicode	codepoint.	*/
				char	resultant_utf8_s[128];
				wchar_t	wide_s[32]	=	{	L'e',	L'ⁱ',	L'ᵠ',	L'	',	L'=',	L'	',	L'c',	L'o',	L's',	L'(',	L'ϕ',	L')',
																											L'	',	L'+',	L'	',	L'i',	L'*',	L's',	L'i',	L'n',	L'(',	L'ϕ',	L')'	};
				wide_s[14]	=	(wchar_t)0x110000;
				Error_Message_Expected();
				size_t	size	=	wchar_to_utf8(wide_s,	resultant_utf8_s,	sizeof(resultant_utf8_s)/sizeof(char));	
				EXPECT_EQ(0,	size);
}

TEST(wchar_to_utf8,	insufficient_result_array_size)	{

				char	resultant_utf8_s[16];
				const	wchar_t*	wide_s	=	L"eⁱᵠ	=	cos(ϕ)	+	i*sin(ϕ)";

				Error_Message_Expected();
				size_t	size	=	wchar_to_utf8(wide_s,	resultant_utf8_s,	sizeof(resultant_utf8_s)/sizeof(char));	
				EXPECT_EQ(0,	size);
}
