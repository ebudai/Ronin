#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct function : test_parser {};

	TEST_P(function, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::function_declaration);
	}

	vector<token> function_tokens =
	{
		{ "let", let },
		{ "(", open_paren },
		{ "a", word },
		{ "=>", returns },
		{ "number", word },
		{ ")", close_paren },
		{ "plus", word },
		{ "(", open_paren },
		{ "b", word },
		{ "=>", returns },
		{ "number", word },
		{ ")", close_paren },
		{ "=>", returns },
		{ "number", word },
		{ "=", assign },
		{ "a", word },
		{ "+", symbol },
		{ "b", word },
		{ ";", terminal },
		{ "", nothing }
	};

	INSTANTIATE_TEST_SUITE_P(parser, function, ::testing::Values(span{ function_tokens }));
}