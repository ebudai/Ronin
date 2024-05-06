#include "test_lexer.h"
#include "../lib/magic_enum.hpp"

using namespace ronin;

namespace unit
{
	struct keyword : test_lexer { };

	TEST_P(keyword, basic)
	{
		EXPECT_FALSE(tokens.empty());
		EXPECT_GE(tokens.front().tag<token::type>(), token::type::var);
		EXPECT_LE(tokens.front().tag<token::type>(), token::type::iterate);
	}

	INSTANTIATE_TEST_SUITE_P(lexer, keyword, ::testing::Values(
		"var " 
		, "let "
		, "module "
		, "import "
		, "type "
		, "extend "
		, "iterate "
	));
}