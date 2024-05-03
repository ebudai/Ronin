#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct literal : test_parser {};

	TEST_P(literal, basic)
	{
		ASSERT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::literal);
	}

	vector<token> number_tokens =
	{
		{ "125", token::type::number },
		{ ";", symbol },
		{ "", nothing}
	};

	vector<token> text_tokens =
	{
		{ "\"test test\"", token::type::text },
		{ ";", symbol },
		{ "", nothing}
	};

	vector<token> date_tokens =
	{
		{ "2654-05-11", token::type::date },
		{ ";", symbol },
		{ "", nothing}
	};

	INSTANTIATE_TEST_SUITE_P(parser, literal, ::testing::Values(span{ number_tokens }, span{ text_tokens }, span{ date_tokens }));
}