#include "test_lexer.h"

using namespace ronin;

namespace unit
{
	struct date_literal : test_lexer { };

	TEST_P(date_literal, basic)
	{
		EXPECT_FALSE(tokens.empty());
		EXPECT_EQ(tokens.front().tag<token::type>(), token::type::date);
	}

	INSTANTIATE_TEST_SUITE_P(lexer, date_literal, ::testing::Values(
		"2022-04-12"
		, "1522-12-19"
		, "0-12-19"
		, "-10114-01-05"
		, "234534634723345-10-09"
	));
}