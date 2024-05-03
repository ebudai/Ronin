#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct importer : test_parser {};

	TEST_P(importer, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::importer);
	}

	vector<token> import_tokens =
	{
		{ "import", import },
		{ "test name", word },
		{ ";", terminal },
		{ "", nothing}
	};

	INSTANTIATE_TEST_SUITE_P(parser, importer, ::testing::Values(span{import_tokens}));
}