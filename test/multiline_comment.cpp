#include "test_lexer.h"

using namespace ronin;

namespace unit
{
	struct multiline_comment : test_lexer { };

	TEST_P(multiline_comment, basic)
	{
		EXPECT_FALSE(tokens.empty());
		EXPECT_EQ(tokens.front().tag<token::type>(), token::type::comment);
	}

	INSTANTIATE_TEST_SUITE_P(lexer, multiline_comment, ::testing::Values(
		"/* test */"
		, "/* multi\nline*/"
		, "/* nested /* balanced */ fun */"
	));
}