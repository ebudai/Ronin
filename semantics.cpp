#include "model.h"
#include "context.h"

using namespace std;

namespace ronin
{
	static bool operator==(const name lhs, const name rhs)
	{
		if (lhs.size() != rhs.size()) return false;
		for (size_t i = 0; i != lhs.size(); ++i)
		{
			if (lhs[i] != rhs[i]) return false;
		}
		return true;
	}

	template <statement::type T> void analyze(context*, statement);

	template <> void analyze<statement::type::importer>(context* context, const statement statement)
	{
		const auto import = static_cast<importer*>(statement.get());
		const auto name = new identifier{{import->name}};
		const auto imported = new unresolved<module>(name);
		//context->import(imported);
	}

	template <> void analyze<statement::type::exporter>(context* context, const statement statement)
	{
		const auto exporter = static_cast<ronin::exporter*>(statement.get());
		const auto mod = main::module[exporter->name];
		mod->submodules.push_back(context);
	}

	template <> void analyze<statement::type::literal>(context* context, const statement statement)
	{
		const auto literal = static_cast<ronin::literal*>(statement.get());

	}

	context* analyze(const span<statement> statements)
	{
		const auto context = new ronin::context;

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