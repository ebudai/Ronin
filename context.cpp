#include "context.h"

using namespace std;

namespace ronin
{
	static bool operator==(const identifier& lhs, const std::span<identifier::component> rhs)
	{
		if (lhs.components.size() != rhs.size()) return false;

		for (size_t i = 0; i != rhs.size(); ++i)
		{
			if (auto const name = std::get_if<ronin::name>(&lhs.components[i]))
			{
				//if (auto const other = std::get_if<words>(&rhs[i]); *name == *other) continue;
				return false;
			}

			//auto const params = std::get<parameters>(lhs.components[i]);
			//if (auto const other = std::get_if<parameters>(&rhs[i]); params == *other) continue;
			return false;
		}

		return true;
	}

	/*std::vector<member*> context::operator[](const std::span<identifier::component> identifier)
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
	}*/
}