#include "grammar.h"
#include "error.h"
#include "lexicon.h"
#include "lib/magic_enum.hpp"

using namespace std;
using enum ronin::token::type;

namespace ronin
{
	template <typename T = void> struct parser;

	template <char open_t, typename element_t, char separator_t, char close_t>
	struct parser<aggregate<open_t, element_t, separator_t, close_t>>
	{
		using aggregate_t = aggregate<open_t, element_t, separator_t, close_t>;
		static aggregate_t* parse(span<token>& tokens)
		{
			if (tokens.front() != open_t) return nullptr;

			auto elements = new aggregate_t;
			span<token> parsed = tokens.subspan(1);

			while (parsed.front().is_not<nothing>())
			{
				if (auto const element = parser<element_t>::parse(parsed))
				{
					elements->push_back(element);
					if (parsed.front() == separator_t) parsed = parsed.subspan(1);
					continue;
				}
				if (parsed.front() == close_t) break;
				delete elements;
				return nullptr;
			}

			tokens = parsed.subspan(1);
			return elements;
		}
	};

	template <typename... Ts> struct parser<std::variant<Ts...>>
	{
		template <size_t N = 0> static variant<Ts...>* parse(span<token>& tokens)
		{
			if constexpr (N < variant_size_v<variant<Ts...>>)
			{
				using type = variant_alternative_t<N, variant<Ts...>>;
				unique_ptr<type> parsed{ parser<type>::parse(tokens) };
				return parsed ? new variant<Ts...>{ *std::move(parsed) } : parse<N + 1>(tokens);
			}
			return nullptr;
		}
	};

	template <> struct parser<name>
	{
		static name* parse(span<token>& tokens)
		{
			size_t length = 0;

			auto type = tokens.front().tag<token::type>();
			while (type == word || (type == symbol && !tokens[length].is_punctuation()))
			{
				if (++length >= tokens.size()) break;
				type = tokens[length].tag<token::type>();
			}

			if (length)
			{
				auto const name = new ronin::name{ tokens.begin(), length };
				tokens = tokens.subspan(length);
				return name;
			}

			return nullptr;
		}
	};

	template <> struct parser<exporter>
	{
		static exporter* parse(span<token>& tokens)
		{
			if (tokens.front().is<module>())
			{
				span<token> parsed = tokens.subspan(1);

				if (auto const name = parser<ronin::name>::parse(parsed))
				{
					auto const exporter = new ronin::exporter{ tokens.front(), *name };
					tokens = parsed;
					return exporter;
				}

				return new bad<exporter>::expected("name", tokens = parsed);
			}

			return nullptr;
		}
	};

	template <> struct parser<importer>
	{
		static importer* parse(span<token>& tokens)
		{
			if (tokens.front().is<import>())
			{
				span<token> parsed = tokens.subspan(1);

				if (auto const name = parser<ronin::name>::parse(parsed))
				{
					auto const importer = new ronin::importer{ tokens.front(), *name };
					tokens = parsed;
					return importer;
				}

				return new bad<importer>::expected("name", tokens = parsed);
			}

			return nullptr;
		}
	};

	template <> struct parser<literal>
	{
		static literal* parse(span<token>& tokens)
		{
			if (!tokens.front().is_literal()) return nullptr;
			auto const literal = new ronin::literal{ tokens.front() };
			tokens = tokens.subspan(1);
			return literal;
		}
	};

	template <> struct parser<reference>
	{
		static reference* parse(span<token>& tokens)
		{
			if (tokens.front().is_keyword()) return nullptr;

			vector<reference::component*> components;
			span<token> parsed = tokens;

			while (parsed.front().is_not<nothing>())
			{
				if (auto const component = parser<reference::component>::parse(parsed)) components.push_back(component);
				else break;
			}

			for (auto const component : components)
			{
				if (std::holds_alternative<name>(*component))
				{
					tokens = parsed;
					return new reference{ std::move(components) };
				}
			}

			return nullptr;
		}
	};

	template <> struct parser<delegate_declaration>
	{
		static delegate_declaration* parse(span<token>& tokens)
		{
			span<token> parsed = tokens;

			if (auto const parameter_block = parser<delegate_declaration::parameter_block>::parse(parsed))
			{
				if (parsed.front().is<returns>())
				{
					parsed = parsed.subspan(1);

					if (auto const expression = parser<ronin::expression>::parse(parsed))
					{
						tokens = parsed;
						return new delegate_declaration{ parameter_block, expression };
					}

					delete parameter_block;
					return new bad<delegate_declaration>::expected("single or compound statement", tokens = parsed);
				}

				delete parameter_block;
			}

			return nullptr;
		}
	};

	template <> struct parser<association>
	{
		static association* parse(span<token>& tokens)
		{
			span<token> parsed = tokens;

			if (auto const destination = parser<reference>::parse(parsed))
			{
				if (const token symbol = parsed.front(); symbol.is<assign>())
				{
					parsed = parsed.subspan(1);

					if (auto const expression = parser<ronin::expression>::parse(parsed))
					{
						tokens = parsed;
						return new association{ destination, symbol, expression };
					}

					delete destination;
					return new bad<association>::expected("expression", tokens = parsed);
				}

				delete destination;
			}

			return nullptr;
		}
	};

	template <> struct parser<loop>
	{
		static loop* parse(span<token>& tokens)
		{
			const token keyword = tokens.front();

			if (keyword.is_not<iterate>()) return nullptr;

			span<token> parsed = tokens.subspan(1);

			if (auto const iterator = parser<reference>::parse(parsed))
			{
				if (const token returning = parsed.front(); returning.is<returns>())
				{
					parsed = parsed.subspan(1);

					if (auto const name = parser<ronin::name>::parse(parsed))
					{
						token equals;
						if (parsed.front().is<assign>())
						{
							equals = parsed.front();
							parsed = parsed.subspan(1);
						}

						if (auto const body = parser<expression>::parse(parsed))
						{
							tokens = parsed;
							return new loop{ keyword, iterator, returning, name, equals, body };
						}

						delete name;
						delete iterator;
						return new bad<loop>::expected("expression", tokens = parsed);
					}

					delete iterator;
					return new bad<loop>::expected("name", tokens = parsed);
				}

				delete iterator;
				return new bad<loop>::expected(lexicon::returns, tokens = parsed);
			}

			return new bad<loop>::expected("reference", tokens = parsed);
		}
	};

	template <> struct parser<parameter>
	{
		static parameter* parse(span<token>& tokens)
		{
			span<token> parsed = tokens;

			if (auto const name = parser<ronin::name>::parse(parsed))
			{
				token returning;
				reference* type = nullptr;
				if (parsed.front().is<returns>())
				{
					returning = parsed.front();
					parsed = parsed.subspan(1);
					type = parser<reference>::parse(parsed);
					if (type == nullptr)
					{
						delete name;
						return new bad<parameter>::expected("type", tokens = parsed);
					}
				}

				token equals;
				expression* initalizer = nullptr;
				if (parsed.front().is<assign>())
				{
					equals = parsed.front();
					parsed = parsed.subspan(1);
					initalizer = parser<expression>::parse(parsed);
					if (initalizer == nullptr)
					{
						delete name;
						delete type;
						return new bad<parameter>::expected("initializer", tokens = parsed);
					}
				}

				tokens = parsed;
				return new parameter{ name, returning, type, equals, initalizer };
			}

			return nullptr;
		}
	};

	template <> struct parser<data_declaration>
	{
		static data_declaration* parse(span<token>& tokens)
		{
			if (const token keyword = tokens.front(); keyword.is<var>() || keyword.is<let>())
			{
				auto parsed = tokens.subspan(1);

				if (auto const declaration = parser<parameter>::parse(parsed))
				{
					tokens = parsed;
					return new data_declaration{ keyword, declaration };
				}

				// if keyword is 'let', give function declaration a chance
				return keyword.is<var>() ? new bad<data_declaration>::expected("declaration", tokens = parsed) : nullptr;
			}

			return nullptr;
		}
	};

	template <> struct parser<identifier>
	{
		static identifier* parse(span<token>& tokens)
		{
			if (tokens.front().is_keyword()) return nullptr;

			vector<identifier::component> components;
			span<token> parsed = tokens;

			while (parsed.front().is_not<nothing>())
			{
				if (auto const component = parser<identifier::component>::parse(parsed)) components.push_back(std::move(*component));
				else break;
			}

			if (components.empty()) return nullptr;

			tokens = parsed;
			return new identifier{ components };
		}
	};

	template <> struct parser<function_declaration>
	{
		static function_declaration* parse(span<token>& tokens)
		{
			const token keyword = tokens.front();
			if (keyword.is_not<let>()) return nullptr;

			span<token> parsed = tokens.subspan(1);

			if (auto const identifier = parser<ronin::identifier>::parse(parsed))
			{
				token returning;
				reference* type = nullptr;
				if (parsed.front().is<returns>())
				{
					returning = parsed.front();
					parsed = parsed.subspan(1);
					type = parser<reference>::parse(parsed);
					if (type == nullptr)
					{
						delete identifier;
						return new bad<function_declaration>::expected("type", tokens = parsed);
					}
				}

				if (parsed.front().is<assign>())
				{
					const token equals = parsed.front();
					parsed = parsed.subspan(1);

					if (auto const definition = parser<expression>::parse(parsed))
					{
						tokens = parsed;
						return new function_declaration{ keyword, identifier, returning, type, equals, definition };
					}

					delete type;
					delete identifier;
					return new bad<function_declaration>::expected("function definition", tokens = parsed);
				}

				delete type;
				delete identifier;
				return new bad<function_declaration>::expected({ (char)assign }, tokens = parsed);
			}

			return new bad<function_declaration>::expected("identifier", tokens = parsed);
		}
	};

	template <> struct parser<type_declaration::body>
	{
		static type_declaration::body* parse(span<token>& tokens)
		{
			span<token> parsed = tokens;
			auto const algebra = parser<reference>::parse(parsed);
			auto const body = parser<definition>::parse(parsed);

			if (algebra || body)
			{
				tokens = parsed;
				return new type_declaration::body{ algebra, body };
			}

			return nullptr;
		}
	};

	template <> struct parser<type_declaration>
	{
		static type_declaration* parse(span<token>& tokens)
		{
			const token keyword = tokens.front();
			if (keyword.is_not<token::type::type>()) return nullptr;

			span<token> parsed = tokens.subspan(1);

			if (auto const identifier = parser<ronin::identifier>::parse(parsed))
			{
				if (parsed.front().is<assign>())
				{
					const token equals = parsed.front();
					parsed = parsed.subspan(1);

					vector<type_declaration::body> definitions;
					while (auto const definition = parser<type_declaration::body>::parse(parsed))
					{
						definitions.push_back(*definition);
						delete definition;
					}

					tokens = parsed;
					return new type_declaration{ keyword, identifier, equals, std::move(definitions) };
				}

				delete identifier;
				return new bad<type_declaration>::expected({ static_cast<char>(assign) }, tokens = parsed);
			}

			return new bad<type_declaration>::expected("identifier", tokens = parsed);
		}
	};

	template <> struct parser<extension>
	{
		static extension* parse(span<token>& tokens)
		{
			const token keyword = tokens.front();
			if (keyword.is_not<extend>()) return nullptr;

			span<token> parsed = tokens.subspan(1);

			if (auto const identifier = parser<ronin::identifier>::parse(parsed))
			{
				if (auto const definition = parser<type_declaration::body>::parse(parsed))
				{
					tokens = parsed;
					return new extension{ keyword, identifier, definition };
				}

				delete identifier;
				return new bad<extension>::expected("definition", tokens = parsed);
			}

			return new bad<extension>::expected("identifier", tokens = parsed);
		}
	};

	template <> struct parser<scope>
	{
		static scope* parse(span<token>& tokens)
		{
			if (auto const statements = parser<scope::block>::parse(tokens))
			{
				return new scope{std::move(*statements)};
			}
			return nullptr;
		}
	};

	template <> struct parser<unknown>
	{
		static unknown* parse(span<token>& tokens)
		{
			size_t length = 0;

			while (tokens[length].is_not<terminal>()
				&& tokens[length].is_not<separator>()
				&& tokens[length].is_not<close_brace>()
				&& tokens[length].is_not<close_bracket>()
				&& tokens[length].is_not<close_paren>()
				&& tokens[length].is_not<nothing>()) ++length;

			if (length)
			{
				if (tokens[length].is_not<nothing>()) ++length;
				auto const unknown = new ronin::unknown{ .tokens = { tokens.begin(), length } };
				tokens = tokens.subspan(length);
				return unknown;
			}

			return nullptr;
		}
	};

	vector<statement> parse(span<token> tokens)
	{
		vector<statement> statements;

		while (tokens.front().is_not<nothing>())
		{
			while (tokens.front().is<terminal>()) tokens = tokens.subspan(1);

#define PARSE(x) { auto x = parser<ronin::x>::parse(tokens); if (x) { statements.emplace_back(x, statement::type::x); continue; } }

			PARSE(importer);
			PARSE(exporter);
			PARSE(association);
			PARSE(data_declaration);
			PARSE(function_declaration);
			PARSE(type_declaration);
			PARSE(extension);
			PARSE(literal);
			PARSE(reference);
			PARSE(delegate_declaration);
			PARSE(list);
			PARSE(lookup);
			PARSE(input);
			PARSE(scope);
			PARSE(loop);
			PARSE(unknown);

#undef PARSE

		}

		return statements;
	}
}