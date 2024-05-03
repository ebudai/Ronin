#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct block : test_parser {};

	TEST_P(block, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::block);
	}

	vector<token> block_tokens =
	{
		{ "{", open_brace },
		{ "var", var },
		{ "test name", word },
		{ "=", assign },
		{ "7", number },
		{ ";", terminal },
		{ "}", close_brace },
		{ "", nothing }
	};

	INSTANTIATE_TEST_SUITE_P(parser, block, ::testing::Values(span{ block_tokens }));
}