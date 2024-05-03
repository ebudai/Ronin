#include "test_lexer.h"

using namespace ronin;

namespace unit
{
	struct symbol : test_lexer { };

	TEST_P(symbol, basic)
	{
		EXPECT_FALSE(tokens.empty());
		if (strcmp("...", GetParam()) == 0)
		{
			EXPECT_EQ(tokens.front().tag<token::type>(), token::type::ellipsis);
		}
		else if (strcmp("..", GetParam()) == 0)
		{
			EXPECT_EQ(tokens.front().tag<token::type>(), token::type::interval);
		}
		else if (strcmp("<<", GetParam()) == 0)
		{
			EXPECT_EQ(tokens.front().tag<token::type>(), token::type::shift_left);
		}
		else if (strcmp(">>", GetParam()) == 0)
		{
			EXPECT_EQ(tokens.front().tag<token::type>(), token::type::shift_right);
		}
		else
		{
			EXPECT_EQ(tokens.front().tag<token::type>(), token::type::symbol);
		}
	}

	INSTANTIATE_TEST_SUITE_P(lexer, symbol, ::testing::Values(
		"..."
		, ".."
		, "<<"
		, ">>"
		, "+"
		, "-"
		, "*"
		, "/"
		, "~"
		, "!"
		, "@"
		, "#"
		, "$"
		, "%"
		, "^"
		, "&"
		, "|"
		, "+="
		, "&="
	));
}