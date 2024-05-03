#include "parser.h"
#include "error.h"
#include "lexicon.h"
#include "lib/magic_enum.hpp"

using namespace std;
using enum ronin::token::type;

namespace ronin
{
	template <char open_t, typename element_t, char separator_t, char close_t>
	auto aggregate<open_t, element_t, separator_t, close_t>::parse(span<token>& tokens) -> aggregate*
	{
		if (tokens.front() != open_t) return nullptr;

		vector<element_t*> elements;
		span<token> parser = tokens.subspan(1);

		while (parser.front().is_not<nothing>())
		{
			if (auto const element = ronin::parse<element_t>(parser))
			{
				elements.push_back(element);
				if (parser.front() == separator_t) parser = parser.subspan(1);
				continue;
			}
			if (parser.front() == close_t) break;
			return nullptr;
		}

		tokens = parser.subspan(1);
		return new aggregate{ elements };
	}

	template <typename T> static T* parse(span<token>& tokens);

	template <typename T> struct variant_parser;
	template <typename... Ts> struct variant_parser<variant<Ts...>>
	{
		template <size_t N = 0> static variant<Ts...>* parse(span<token>& tokens)
		{
			if constexpr (N < variant_size_v<variant<Ts...>>)
			{
				using type = variant_alternative_t<N, variant<Ts...>>;
				unique_ptr<type> parsed{ ronin::parse<type>(tokens) };
				return parsed ? new variant<Ts...>{ *std::move(parsed) } : parse<N + 1>(tokens);
			}
			return nullptr;
		}
	};

	template <> words* parse(span<token>& tokens)
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
			auto const name = new ronin::words{ tokens.begin(), length };
			tokens = tokens.subspan(length);
			return name;
		}

		return nullptr;	
	}

	template <> exporter* parse(span<token>& tokens)
	{
		if (tokens.front().is<module>())
		{
			span<token> parser = tokens.subspan(1);

			if (auto const name = parse<ronin::words>(parser))
			{
				auto const exporter = new ronin::exporter{ tokens.front(), name };
				tokens = parser;
				return exporter;
			}

			return new bad<exporter>::expected("name", tokens = parser);
		}
		
		return nullptr;
	}

	template <> importer* parse(span<token>& tokens)
	{
		if (tokens.front().is<import>()) 
		{
			span<token> parser = tokens.subspan(1);

			if (auto const name = parse<ronin::words>(parser))
			{
				auto const importer = new ronin::importer{ tokens.front(), name };
				tokens = parser;
				return importer;
			}

			return new bad<importer>::expected("name", tokens = parser);
		}

		return nullptr;
	}

	template <> literal* parse(span<token>& tokens)
	{
		if (!tokens.front().is_literal()) return nullptr;
		auto const literal = new ronin::literal{ tokens.front() };
		tokens = tokens.subspan(1);
		return literal;
	}

	template <> block* parse(span<token>& tokens)
	{
		return static_cast<block*>(block::parse(tokens));
	}

	template <> parameter_block* parse(span<token>& tokens)
	{
		return parameter_block::parse(tokens);
	}

	template <> reference::component* parse(span<token>& tokens)
	{
		return variant_parser<reference::component>::parse(tokens);
	}

	template <> reference* parse(span<token>& tokens)
	{
		if (tokens.front().is_keyword()) return nullptr;

		vector<reference::component*> components;
		span<token> parser = tokens;

		while (parser.front().is_not<nothing>())
		{
			if (auto const component = parse<reference::component>(parser)) components.push_back(component);
			else break;
		}

		for (auto const component : components)
		{
			if (std::holds_alternative<words>(*component))
			{
				tokens = parser;
				return new reference{ components };
			}
		}

		return nullptr;
	}

	template <> expression* parse(span<token>& tokens)
	{
		return variant_parser<expression>::parse(tokens);
	}

	template <> delegate* parse(span<token>& tokens)
	{
		span<token> parser = tokens;
		
		if (auto const parameter_block = variant_parser<delegate::parameter_block>::parse(parser))
		{
			if (parser.front().is<returns>())
			{
				parser = parser.subspan(1);

				if (auto const expression = parse<ronin::expression>(parser))
				{
					tokens = parser;
					return new delegate{ parameter_block, expression };
				}

				delete parameter_block;
				return new bad<delegate>::expected("single or compound statement", tokens = parser);
			}

			delete parameter_block;
		}

		return nullptr;
	}

	template <> list* parse(span<token>& tokens)
	{
		return list::parse(tokens);
	}

	template <> input* parse(span<token>& tokens)
	{
		return input::parse(tokens);
	}

	template <> lookup* parse(span<token>& tokens)
	{
		return lookup::parse(tokens);
	}

	template <> association* parse(span<token>& tokens)
	{
		span<token> parser = tokens;
		
		if (auto const destination = parse<reference>(parser))
		{
			token symbol = parser.front();
			if (symbol.is<assign>())
			{
				parser = parser.subspan(1);

				if (auto const expression = parse<ronin::expression>(parser))
				{
					tokens = parser;
					return new association{ destination, symbol, expression };
				}

				delete destination;
				return new bad<association>::expected("expression", tokens = parser);
			}

			delete destination;	
		}

		return nullptr;
	}

	template <> loop* parse(span<token>& tokens)
	{
		token keyword = tokens.front();

		if (keyword.is_not<iterate>()) return nullptr;
		
		span<token> parser = tokens.subspan(1);

		if (auto const iterator = parse<reference>(parser))
		{
			token returning = parser.front();
			if (returning.is<returns>())
			{
				parser = parser.subspan(1);

				if (auto const name = parse<ronin::words>(parser))
				{
					token equals;
					if (parser.front().is<assign>())
					{
						equals = parser.front();
						parser = parser.subspan(1);
					}

					if (auto const body = parse<expression>(parser))
					{
						tokens = parser;
						return new loop{ keyword, iterator, returning, name, equals, body };
					}

					delete name;
					delete iterator;
					return new bad<loop>::expected("expression", tokens = parser);
				}

				delete iterator;
				return new bad<loop>::expected("name", tokens = parser);
			}

			delete iterator;
			return new bad<loop>::expected(lexicon::returns, tokens = parser);
		}

		return new bad<loop>::expected("reference", tokens = parser);
	}

	template <> parameter* parse(span<token>& tokens)
	{
		span<token> parser = tokens;

		if (auto const name = parse<ronin::words>(parser))
		{
			token returning;
			reference* type = nullptr;
			if (parser.front().is<returns>())
			{
				returning = parser.front();
				parser = parser.subspan(1);
				type = parse<reference>(parser);
				if (type == nullptr)
				{
					delete name;
					return new bad<parameter>::expected("type", tokens = parser);
				}
			}

			token equals;
			expression* initalizer = nullptr;
			if (parser.front().is<assign>())
			{
				equals = parser.front();
				parser = parser.subspan(1);
				initalizer = parse<expression>(parser);
				if (initalizer == nullptr)
				{
					delete name;
					delete type;
					return new bad<parameter>::expected("initializer", tokens = parser);
				}
			}

			tokens = parser;
			return new parameter{ name, returning, type, equals, initalizer };
		}

		return nullptr;
	}

	template <> data_declaration* parse(span<token>& tokens)
	{
		token keyword = tokens.front();
		if (keyword.is<var>() || keyword.is<let>())
		{
			auto parser = tokens.subspan(1);

			if (auto const declaration = parse<parameter>(parser))
			{
				tokens = parser;
				return new data_declaration{ keyword, declaration };
			}
		}

		return nullptr;
	}

	template <> name::component* parse(span<token>& tokens)
	{
		return variant_parser<name::component>::parse(tokens);
	}

	template <> name* parse(span<token>& tokens)
	{
		if (tokens.front().is_keyword()) return nullptr;

		vector<name::component*> components;
		span<token> parser = tokens;

		while (parser.front().is_not<nothing>())
		{
			auto const component = parse<name::component>(parser);
			if (component) components.push_back(component);
			else break;
		}

		if (components.empty()) return nullptr;

		tokens = parser;
		return new name{ components };
	}

	template <> function_declaration* parse(span<token>& tokens)
	{
		token keyword = tokens.front();
		if (keyword.is_not<let>()) return nullptr;
		
		span<token> parser = tokens.subspan(1);

		if (auto const identifier = parse<ronin::name>(parser))
		{
			token returning;
			reference* type = nullptr;
			if (parser.front().is<returns>())
			{
				returning = parser.front();
				parser = parser.subspan(1);
				type = parse<reference>(parser);
				if (type == nullptr)
				{
					delete identifier;
					return new bad<function_declaration>::expected("type", tokens = parser);
				}
			}

			if (parser.front().is<assign>())
			{
				token equals = parser.front();
				parser = parser.subspan(1);

				if (auto const definition = parse<ronin::expression>(parser))
				{
					tokens = parser;
					return new function_declaration{ keyword, identifier, returning, type, equals, definition };
				}

				delete type;
				delete identifier;
				return new bad<function_declaration>::expected("function definition", tokens = parser);
			}

			delete type;
			delete identifier;
			return new bad<function_declaration>::expected({ (char)assign }, tokens = parser);
		}

		return new bad<function_declaration>::expected("identifier", tokens = parser);
	}

	template <> declaration* parse(span<token>& tokens)
	{
		return variant_parser<declaration>::parse(tokens);
	}

	template <> type_declaration::definition* parse<type_declaration::definition>(span<token>& tokens)
	{
		span<token> parser = tokens;
		auto const algebra = parse<reference>(parser);
		auto const body = definition::parse(parser);

		if (algebra || body)
		{
			tokens = parser;
			return new type_declaration::definition{ algebra, body };
		}

		return nullptr;
	}

	template <> type_declaration* parse(span<token>& tokens)
	{
		const token keyword = tokens.front();
		if (keyword.is_not<type>()) return nullptr;
		
		span<token> parser = tokens.subspan(1);

		if (auto const identifier = parse<ronin::name>(parser))
		{
			if (parser.front().is<assign>())
			{
				const token equals = parser.front();
				parser = parser.subspan(1);
				
				vector<type_declaration::definition> definitions;
				while (auto const definition = parse<type_declaration::definition>(parser))
				{
					definitions.push_back(*definition);
					delete definition;
				}				

				tokens = parser;
				return new type_declaration{ keyword, identifier, equals, definitions };
			}

			delete identifier;
			return new bad<type_declaration>::expected({ (char)assign }, tokens = parser);
		}

		return new bad<type_declaration>::expected("identifier", tokens = parser);
	}

	template <> extension* parse(span<token>& tokens)
	{
		token keyword = tokens.front();
		if (keyword.is_not<extend>()) return nullptr;

		span<token> parser = tokens.subspan(1);

		if (auto const identifier = parse<ronin::name>(parser))
		{
			if (auto const definition = parse<type_declaration::definition>(parser))
			{
				tokens = parser;
				return new extension{ keyword, identifier, definition };
			}
				
			delete identifier;
			return new bad<extension>::expected("definition", tokens = parser);
		}

		return new bad<extension>::expected("identifier", tokens = parser);
	}

	template <> unknown* parse(span<token>& tokens)
	{
		size_t length = 0;
		
		while (tokens[length].is_not<terminal>()
			&& tokens[length].is_not<close_brace>()
			&& tokens[length].is_not<nothing>()) ++length;
		
		if (length)
		{
			auto const unknown = new ronin::unknown{ .tokens = { tokens.begin(), length } };
			tokens = tokens.subspan(length);
			return unknown;
		}
		
		return nullptr;
	}

	vector<statement> parse(span<token> tokens)
	{
		vector<statement> statements;

		while (tokens.front().is_not<nothing>())
		{
			while (tokens.front().is<terminal>()) tokens = tokens.subspan(1);

#define PARSE(x) { auto x = parse<ronin::x>(tokens); if (x) { statements.emplace_back(x, statement::type::x); continue; } }

			PARSE(importer);
			PARSE(exporter);
			PARSE(association);
			PARSE(data_declaration);
			PARSE(function_declaration);
			PARSE(type_declaration);
			PARSE(extension);
			PARSE(literal);
			PARSE(reference);
			PARSE(delegate);
			PARSE(list);
			PARSE(lookup);
			PARSE(input);
			PARSE(block);
			PARSE(loop);
			PARSE(unknown);

#undef PARSE

		}

		return statements;
	}
}