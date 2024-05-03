#pragma once

#include "semantics.h"
#include <mutex>

namespace ronin
{
	struct module;

	struct context
	{
		virtual ~context() = default;

		virtual bool operator==(const context& other) const { return this == &other; }

		virtual std::vector<member*> operator[](std::span<identifier::value_type> identifier) const;

		void add(const member& member);

	protected:

		std::vector<module*> imports;
		std::vector<member> members;

	private:

		std::mutex lock;
	};

	struct scope final : context
	{
		~scope() override = default;

		std::vector<member*> operator[](std::span<identifier::value_type> identifier) const override;

		context* parent;
		std::vector<resolution> lines;
	};

	struct module final : context
	{
		~module() override = default;

		std::vector<member*> operator[](std::span<identifier::value_type> identifier) const override;

		token name;
		std::vector<scope> submodules;
	};
}