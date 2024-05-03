#include "test_lexer.h"

using namespace ronin;

namespace unit
{
	struct single_line_comment : test_lexer { };

	TEST_P(single_line_comment, basic)
	{
		EXPECT_FALSE(tokens.empty());
		EXPECT_EQ(tokens.front().tag<token::type>(), token::type::comment);
	}

	INSTANTIATE_TEST_SUITE_P(lexer, single_line_comment, ::testing::Values(
		"// test test \n"
		, "// comment without newline"
		, "// comment with stuff after \nstuff"
	));
}