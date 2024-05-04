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

		virtual std::vector<member*> operator[](std::span<identifier::value_type> identifier);

		void import(module* module);
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

		std::vector<member*> operator[](std::span<identifier::value_type> identifier) override;

		context* parent;
		std::vector<resolution> lines;
	};

	struct module final : context
	{
		~module() override = default;

		std::vector<member*> operator[](std::span<identifier::value_type> identifier) override;
		module* operator[](words name);

		token name;
		std::vector<context*> submodules;
	};

	namespace main
	{
		static inline thread_local module module;
	}
}