#include "context.h"

using namespace std;

namespace ronin
{
	static bool operator==(const identifier& lhs, const std::span<identifier::value_type> rhs)
	{
		if (lhs.size() != rhs.size()) return false;

		for (size_t i = 0; i != rhs.size(); ++i)
		{
			if (auto const name = std::get_if<token>(&lhs[i]))
			{
				if (auto const other = std::get_if<token>(&rhs[i]); *name == *other) continue;
				return false;
			}

			auto const params = std::get<parameters>(lhs[i]);
			//if (auto const other = std::get_if<parameters>(&rhs[i]); params == *other) continue;
			return false;
		}

		return true;
	}

	std::vector<member*> context::operator[](const std::span<identifier::value_type> identifier)
	{
		std::vector<member*> found;

		int sublength = 1;
		for (member& member : members)
		{
			if (*member.identifier == identifier.subspan(0, sublength))
			{

			}
		}

		return found;
	}
}