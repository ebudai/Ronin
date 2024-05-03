#include "test_lexer.h"

using namespace ronin;

namespace unit
{
	struct text_literal : test_lexer { };

	TEST_P(text_literal, basic)
	{
		EXPECT_FALSE(tokens.empty());
		EXPECT_EQ(tokens.front().tag<token::type>(), token::type::text);
	}

	INSTANTIATE_TEST_SUITE_P(lexer, text_literal, ::testing::Values(
		"\"testtest\""
		, "\"thing with stuff\""
		, "\"multi\nline\""
		, "\"has\\\"embedded\\\"quotes\""
	));
}