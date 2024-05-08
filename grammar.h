#pragma once

#include "token.h"
#include "lib/magic_enum.hpp"
#include <utility>
#include <vector>
#include <span>
#include <variant>

namespace ronin
{
	struct statement : mz::tagged_ptr<void>
	{
		enum class type : uint16_t
		{
			importer,
			exporter,
			literal,
			delegate_declaration,
			list,
			input,
			lookup,
			loop,
			scope,
			association,
			reference,
			data_declaration,
			function_declaration,
			type_declaration,
			extension,
			unknown,
			error,
		};

		using tagged_ptr::tagged_ptr;

		template <type T> [[nodiscard]] bool is() const { return tag<type>() == T; }
	};

	template <char open_t, typename element_t, char separator_t, char close_t>
	struct aggregate : std::vector<element_t*> { };

	using name = std::span<token>;

	struct literal
	{
		token value;
	};

	struct data_declaration;
	struct delegate_declaration;
	struct function_declaration;
	struct type_declaration;
	struct extension;
	using declaration = std::variant<data_declaration, function_declaration, type_declaration, delegate_declaration, extension>;
	using definition = aggregate<'{', declaration, ';', '}'>;

	struct reference;
	using list = aggregate<'[', reference, ',', ']'>;
	using input = aggregate<'(', reference, ',', ')'>;

	struct association;
	using lookup = aggregate<'{', association, ',', '}'>;

	using temporary = std::variant<literal, list, input, lookup>;

	struct loop;
	struct scope;
	using expression = std::variant<reference, declaration, temporary, loop, scope>;

	//using block = aggregate<'{', expression, ';', '}'>;

	struct parameter
	{
		name* name;
		token returns;
		reference* type;
		token assign;
		expression* initializer;
	};
	using parameter_block = aggregate<'(', parameter, ',', ')'>;

	struct reference
	{
		using component = std::variant<name, literal, delegate_declaration, list, input, lookup>;

		std::vector<component*> components;
	};

	struct importer
	{
		token keyword;
		name name;
	};

	struct exporter
	{
		token keyword;
		name name;
	};

	struct identifier
	{
		using component = std::variant<name, parameter_block>;

		std::vector<component> components;
	};

	struct member
	{
		identifier* identifier;
	};

	struct association
	{
		reference* destination;
		token symbol;
		expression* assignment;
	};

	struct data_declaration
	{
		token keyword;
		parameter* declaration;
	};

	struct function_declaration
	{
		token keyword;
		identifier* identifier;
		token returns;
		reference* type;
		token equals;
		expression* body;
	};

	struct type_declaration
	{
		struct body
		{
			reference* algebra;
			definition* definition;
		};

		type_declaration() = default;
		type_declaration(const type_declaration&) = default;

		type_declaration(const token keyword, identifier* name, const token equals, std::vector<body> definitions)
			: keyword(keyword)
			, name(name)
			, equals(equals)
			, definitions(std::move(definitions)) { }

		token keyword;
		identifier* name;
		token equals;

		std::vector<body> definitions;
	};

	struct extension
	{
		token keyword;
		identifier* name;
		type_declaration::body* definition;
	};

	struct delegate_declaration
	{
		using parameter_block = std::variant<name, parameter_block>;

		parameter_block* parameters;
		expression* definition;
	};

	struct loop
	{
		token keyword;
		reference* iterator;
		token returns;
		name* item_name;
		token assign;
		expression* body;
	};

	struct scope
	{
		using block = aggregate<'{', expression, ';', '}'>;

		block statements;
	};
	/*struct expression : resolvable
	{
		expression() = default;
		explicit expression(resolvable* x) : resolvable(std::move(*x)) {}
	};*/

	struct unknown
	{
		std::span<token> tokens;
	};
}
