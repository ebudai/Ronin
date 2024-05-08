#pragma once

#include "model.h"
#include <mutex>

namespace ronin
{
	struct module;

	struct context
	{
		std::vector<module*> imports;
		std::vector<member> members;
	};

	/*struct scope final : context
	{
		~scope() override = default;

		std::vector<member*> operator[](std::span<identifier::component> identifier) override { return {}; } //TODO

		context* parent = nullptr;
		std::vector<resolution> lines;
	};*/

	struct module //: context
	{
		//~module() override = default;

		std::vector<member*> operator[](std::span<identifier::component> identifier) /*override*/ { return {}; } //TODO
		module* operator[](name name) const { return nullptr; } //TODO

		token name;
		std::vector<context*> submodules;
	};

	namespace main
	{
		static inline thread_local module module;
	}
}