#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct loop : test_parser {};

	TEST_P(loop, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::loop);
	}

	vector<token> loop_tokens =
	{
		{ "iterate", iterate },
		{ "shoes", word },
		{ "=>", returns },
		{ "shoe", word },
		{ "=", assign },
		{ "{", open_brace },
		{ "return", word },
		{ "shoe", word },
		{ "size", word },
		{ ";", terminal },
		{ "}", close_brace },
		{ "", nothing }
	};

	INSTANTIATE_TEST_CASE_P(parser, loop, ::testing::Values(span{ loop_tokens }));
}