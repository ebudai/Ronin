#include "test_lexer.h"
#include "../lib/magic_enum.hpp"

using namespace ronin;
using enum token::type;

namespace unit
{
	using magic_enum::enum_name;

	struct punctuation : test_lexer {};

	TEST_P(punctuation, basic)
	{
		EXPECT_FALSE(tokens.empty());
		if (strcmp("=>", GetParam()) == 0)
		{
			EXPECT_EQ(tokens.front().tag<token::type>(), returns);
		}
		else
		{
			EXPECT_EQ(tokens.front().tag<token::type>(), (token::type)*GetParam());
		}		
	}

	INSTANTIATE_TEST_SUITE_P(lexer, punctuation, ::testing::Values(
		"=>",
		new char[] { (char)open_paren, '\0' },
		new char[] { (char)close_paren, '\0' },
		new char[] { (char)open_brace, '\0' },
		new char[] { (char)close_brace, '\0' },
		new char[] { (char)delegation, '\0' },
		new char[] { (char)separator, '\0' },
		new char[] { (char)terminal, '\0' },
		new char[] { (char)assign, '\0' }
	));
}