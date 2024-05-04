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

		void import(module* module) {} //TODO
		void add(const member& member) {} //TODO

	protected:

		std::vector<module*> imports;
		std::vector<member> members;

	private:

		std::mutex lock;
	};

	struct scope final : context
	{
		~scope() override = default;

		std::vector<member*> operator[](std::span<identifier::value_type> identifier) override { return {}; } //TODO

		context* parent;
		std::vector<resolution> lines;
	};

	struct module : context
	{
		~module() override = default;

		std::vector<member*> operator[](std::span<identifier::value_type> identifier) override { return {}; } //TODO
		module* operator[](words name) { return nullptr; } //TODO

		token name;
		std::vector<context*> submodules;
	};

	namespace main
	{
		static inline thread_local module module;
	}
}