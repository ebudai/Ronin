#pragma once

#include "grammar.h"

namespace ronin
{
	struct variable;
	using parameters = std::vector<variable>;
	//using identifier = std::vector<std::variant<token, parameters>>;



	struct resolution
	{
		member* member;
		std::vector<resolution*> parameters;
	};

	struct type;

	struct deletate;

	struct variable : member
	{
		type* type;
		resolution initializer;
	};

	struct relation : member
	{
		type* type;
		resolution initializer;
	};

	struct context;

	struct function : member
	{
		type* return_type;
		context* context;
	};

	struct type : member
	{
		bool operator==(const type& other) const { return this == &other; }

		context* context;
	};


	/*struct parameters
	{


		std::vector<parameter> parameters;
	};

	using name_part = std::variant<token, parameters>;

	[[nodiscard]] inline bool operator==(const name_part& lhs, const name_part& rhs)
	{
		if (const auto left_token = std::get_if<token>(&lhs))
		{
			if (const auto right_token = std::get_if<token>(&rhs))
			{
				return *left_token == *right_token;
			}

			return false;
		}

		auto& left_params = std::get<parameters>(lhs);
		if (const auto right_params = std::get_if<parameters>(&rhs))
		{
			return left_params == *right_params;
		}

		return false;
	}

	using identifier = std::vector<name_part>;*/



	template <typename T> struct unresolved final : T
	{
		identifier* identifier;

		explicit unresolved(ronin::identifier* identifier) : identifier(identifier) {}

		virtual bool operator==(const T& other) const { return false; }
		/*{
			auto&& unresolved_other = static_cast<const unresolved&>(other);
			return *identifier == *unresolved_other.identifier;
		}*/
	};
}