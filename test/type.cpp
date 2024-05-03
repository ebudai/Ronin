#include "test_parser.h"

using namespace ronin;
using namespace std;
using enum token::type;

namespace unit
{
	struct type : test_parser {};

	TEST_P(type, basic)
	{
		EXPECT_FALSE(statements.empty());
		EXPECT_EQ(statements.front().tag<statement::type>(), statement::type::type_declaration);
	}

	vector<token> type_tokens =
	{
		{ "type", token::type::type },
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
		{ "=", assign },
		{ "number", word },
		{ "or", word },
		{ "text", word },
		{ "and", word },
		{ "{", open_brace },
		{ "var", var },
		{ "x", word },
		{ "=>", returns },
		{ "number", word },
		{ ";", terminal },
		{ "}", close_brace },
		{ ";", terminal },
		{ "", nothing }
	};

	INSTANTIATE_TEST_SUITE_P(parser, type, ::testing::Values(span{ type_tokens }));
}