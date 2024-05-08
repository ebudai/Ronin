#pragma once

#include <vector>
#include <span>

namespace ronin
{
    struct statement;
    struct token;

    std::vector<statement> parse(std::span<token> tokens);
}