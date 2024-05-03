#include "test_lexer.h"

using namespace ronin;

namespace unit
{
	struct numeric_literal : test_lexer { };

	TEST_P(numeric_literal, basic)
	{
		EXPECT_FALSE(tokens.empty());
		EXPECT_EQ(tokens.front().tag<token::type>(), token::type::number);
	}

	INSTANTIATE_TEST_SUITE_P(lexer, numeric_literal, ::testing::Values(
		"12"
		, "1522"
		, "-10114"
		, "234534634723345"
		, "1,224,331.0028323"
		, "665,222,775,223,773,236"
	));
}