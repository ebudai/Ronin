#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct exporter : test_parser {};

	TEST_P(exporter, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::exporter);
	}

	vector<token> export_tokens =
	{
		{ "module", module },
		{ "test name", word },
		{ ";", terminal },
		{ "", nothing }
	};

	INSTANTIATE_TEST_SUITE_P(parser, exporter, ::testing::Values(span{export_tokens}));
}