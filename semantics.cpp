#include "semantics.h"
#include "context.h"

using namespace std;

namespace ronin
{
	static bool operator==(const words lhs, const words rhs)
	{
		if (lhs.size() != rhs.size()) return false;
		for (size_t i = 0; i != lhs.size(); ++i)
		{
			if (lhs[i] != rhs[i]) return false;
		}
		return true;
	}

	bool parameters::parameter::operator==(const parameters::parameter& other) const
	{
		return name == other.name && *type == *other.type;
	}

	template <statement::type T> void analyze(context*, statement);

	template <> void analyze<statement::type::importer>(context* context, statement statement)
	{
		auto import = static_cast<importer*>(statement.get());
		auto name = new identifier{import->name->begin(), import->name->end()};
		auto imported = new unresolved<ronin::context>(name);
		//context->imports.push_back(imported);
	}

	template <> void analyze<statement::type::exporter>(context* context, statement statement)
	{
				
	}

	context* analyze(span<statement> statements)
	{
		auto context = new ronin::context;

		using enum statement::type;

		for (auto& statement : statements)
		{
			switch (statement.tag<statement::type>())
			{
				case importer: analyze<importer>(context, statement); break;
				case exporter: analyze<exporter>(context, statement); break;
			}
		}

		return context;
	}

	
}