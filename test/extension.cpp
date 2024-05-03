#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct extension : test_parser {};

	TEST_P(extension, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::extension);
	}

	vector<token> extension_tokens =
	{
		{ "extend", token::type::extend },
		{ "ball", word },
		{ "kicker", word },
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
		{ "{", open_brace },
		{ "var", var },
		{ "big", word },
		{ "town", word },
		{ "=>", returns },
		{ "text", word },
		{ ";", terminal },
		{ "}", close_brace },
		{ ";", terminal },
		{ "", nothing }
	};

	INSTANTIATE_TEST_SUITE_P(parser, extension, ::testing::Values(span{ extension_tokens }));
}