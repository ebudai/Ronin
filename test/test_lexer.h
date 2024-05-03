#pragma once

#include "../lexer.h"
#include "gtest/gtest.h"

namespace unit
{
	struct test_lexer : ::testing::TestWithParam<const char*>
	{
		void SetUp() final { tokens = ronin::lex(GetParam()); }

		std::vector<ronin::token> tokens;
	};
}