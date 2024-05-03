#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum ronin::token::type;

namespace unit
{
	struct delegate : test_parser {};

	TEST_P(delegate, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::delegate);
	}

	vector<token> delegate_tokens =
	{
		{ "(", open_paren },
		{ "a", word },
		{ "=>", returns },
		{ "number", word },
		{ ",", separator },
		{ "b", word },
		{ "=>", returns },
		{ "number", word },
		{ ")", close_paren },
		{ "=>", returns },
		{ "{", open_brace },
		{ "return", word },
		{ "a", word },
		{ "+", symbol },
		{ "b", word },
		{ ";", terminal },
		{ "}", close_brace },
		{ "", nothing }
	};

	INSTANTIATE_TEST_SUITE_P(parser, delegate, ::testing::Values(span{ delegate_tokens }));
}