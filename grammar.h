#pragma once

#include "token.h"
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
			block,
			association,
			reference,
			data_declaration,
			function_declaration,
			type_declaration,
			extension,
			unknown,
		};

		using tagged_ptr::tagged_ptr;
	};

	template <char open_t, typename element_t, char separator_t, char close_t>
	struct aggregate
	{
		static aggregate* parse(std::span<token>& tokens);

		std::vector<element_t*> elements;
	};

	using words = std::span<token>;

	struct literal
	{
		token value;
	};

	struct association;
	struct block;
	struct data_declaration;
	struct delegate_declaration;
	struct function_declaration;
	struct reference;
	struct type_declaration;
	struct extension;

	using declaration = std::variant<data_declaration, function_declaration, type_declaration, extension>;
	using list = aggregate<'[', reference, ',', ']'>;
	using input = aggregate<'(', reference, ',', ')'>;
	using lookup = aggregate<'{', association, ',', '}'>;	
	using expression = std::variant<reference, literal, delegate_declaration, list, input, lookup, data_declaration, function_declaration, type_declaration, extension, block>;

	struct reference
	{
		using component = std::variant<words, literal, delegate_declaration, list, input, lookup>;

		std::vector<component*> components;
	};

	struct importer
	{
		token keyword;
		words* name;
	};

	struct exporter
	{
		token keyword;
		words* name;
	};

	struct association
	{
		reference* destination;
		token symbol;
		expression* assignment;
	};

	struct parameter
	{
		words* name;
		token returns;
		reference* type;
		token assign;
		expression* initalizer;
	};

	struct data_declaration
	{
		token keyword;
		parameter* declaration;
	};

	using parameter_block = aggregate<'(', parameter, ',', ')'>;

	struct name
	{
		using component = std::variant<words, parameter_block>;

		std::vector<component*> components;
	};

	struct function_declaration
	{
		token keyword;
		name* identifier;
		token returns;
		reference* type;
		token equals;
		expression* body;
	};
	
	using definition = aggregate<'{', declaration, ';', '}'>;

	struct type_declaration
	{
		struct body
		{
			reference* algebra;
			definition* definition;
		};

		type_declaration() = default;
		type_declaration(const type_declaration&) = default;

		type_declaration(const token keyword, name* name, const token equals, std::vector<body> definitions)
			: keyword(keyword)
			, name(name)
			, equals(equals)
			, definitions(std::move(definitions)) { }

		token keyword;
		name* name;
		token equals;

		std::vector<body> definitions;
	};

	struct extension
	{
		token keyword;
		name* name;
		type_declaration::body* definition;
	};

	struct delegate_declaration
	{
		using parameter_block = std::variant<words, parameter_block>;

		parameter_block* parameters;
		expression* definition;
	};

	struct block : aggregate<'{', expression, ';', '}'> {};

	struct loop
	{
		token keyword;
		reference* iterator;
		token returns;
		words* item_name;
		token assign;
		expression* body;
	};

	struct unknown
	{
		std::span<token> tokens;
	};

	std::vector<statement> parse(std::span<token> tokens);
}