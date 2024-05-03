#pragma once

#include "token.h"
#include <vector>
#include <string>

namespace ronin
{
	std::vector<token> lex(std::string_view sourcecode);
}