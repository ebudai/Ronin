#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct association : test_parser {};

	TEST_P(association, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::association);
	}

	vector<token> association_tokens =
	{
		{ "a", word },
		{ "=", assign },
		{ "\"test\"", text },
		{ ";", terminal },
		{ "", nothing }
	};

	INSTANTIATE_TEST_SUITE_P(parser, association, ::testing::Values(span{ association_tokens }));
}