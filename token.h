#pragma once

#include "lib/tagged_ptr.hpp"

namespace ronin
{
	struct token : mz::tagged_ptr<const char>
	{
		enum class type : uint16_t
		{
			date,
			number,
			text,

			symbol,
			ellipsis,
			interval,
			shift_left,
			shift_right,

			var,
			let,
			module,
			import,
			type,
			extend,
			iterate,

			whitespace,
			comment,

			word,

			returns,
			open_paren = '(',
			close_paren = ')',
			open_brace = '{',
			close_brace = '}',
			delegation = '?',
			separator = ',',
			terminal = ';',
			assign = '=',
			quote = '\"',

			nothing,
		};

		token() : tagged_ptr("", type::nothing) { }
		token(std::nullptr_t, type) = delete;

		using tagged_ptr::tagged_ptr;

		bool operator==(const token other) const { return strcmp(get(), other.get()) == 0; }
		bool operator!=(const token other) const { return strcmp(get(), other.get()); }
		bool operator==(const char character) const { return *get() == character; }
		bool operator!=(const char character) const { return *get() != character; }

		template <type T> [[nodiscard]] bool is() const { return tag<type>() == T; }
		template <type T> [[nodiscard]] bool is_not() const { return tag<type>() != T; }

		[[nodiscard]] bool is_literal() const
		{
			const auto type = tag<token::type>();
			return type >= type::date && type <= type::text;
		}

		[[nodiscard]] bool is_punctuation() const
		{
			const auto type = tag<token::type>();
			return type >= type::returns && type <= type::quote;
		}

		[[nodiscard]] bool is_keyword() const
		{
			const auto type = tag<token::type>();
			return type >= type::var && type <= type::iterate;
		}
	};
}