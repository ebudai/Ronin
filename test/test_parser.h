#pragma once

#include "../parser.h"
#include "gtest/gtest.h"
#include <span>

namespace unit
{
	struct test_parser : ::testing::TestWithParam<std::span<ronin::token>>
	{
		void SetUp() final { statements = parse(GetParam()); }

		std::vector<ronin::statement> statements;
	};
}