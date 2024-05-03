#include "test_lexer.h"

using namespace ronin;

namespace unit
{
	struct whitespace : test_lexer { };

	TEST_P(whitespace, basic)
	{
		EXPECT_FALSE(tokens.empty());
		EXPECT_EQ(tokens.front().tag<token::type>(), token::type::whitespace);
	}

	INSTANTIATE_TEST_SUITE_P(lexer, whitespace, ::testing::Values(
		" \t\r\n" //basic
		, "\x0B" // line tabulation
		, "\x0C" // form feed
		, "\xC2\x85" // next line
		, "\xC2\xA0" // no-break space
		, "\xE1\x9A\x80" // ogham space mark
		, "\xE2\x80\x80" // en quad
		, "\xE2\x80\x81" // em quad
		, "\xE2\x80\x82" // en space
		, "\xE2\x80\x83" // em space
		, "\xE2\x80\x84" // three-per-em space
		, "\xE2\x80\x85" // four-per-em space
		, "\xE2\x80\x86" // six-per-em space
		, "\xE2\x80\x87" // figure space
		, "\xE2\x80\x88" // punctuation space
		, "\xE2\x80\x89" // thin space
		, "\xE2\x80\x8A" // hair space
		, "\xE2\x80\xAF" // narrow no-break space
		, "\xE2\x81\x9F" // medium mathematical space
		, "\xE3\x80\x80" // ideographic space
		, " \t\r\n" // all
			"\x0B" 
			"\x0C" 
			"\xC2\x85" 
			"\xC2\xA0"
			"\xE1\x9A\x80" 
			"\xE2\x80\x80" 
			"\xE2\x80\x81" 
			"\xE2\x80\x82" 
			"\xE2\x80\x83" 
			"\xE2\x80\x84" 
			"\xE2\x80\x85" 
			"\xE2\x80\x86" 
			"\xE2\x80\x87" 
			"\xE2\x80\x88" 
			"\xE2\x80\x89" 
			"\xE2\x80\x8A" 
			"\xE2\x80\xAF" 
			"\xE2\x81\x9F" 
			"\xE3\x80\x80" 

	));
}