#include "lexer.h"
#include "lexicon.h"
#include "lib/magic_enum.hpp"
#include "lib/ctre.hpp"
#include <icu.h>

using namespace std;

namespace ronin
{
	using enum token::type;

	template <token::type T> static size_t lex(string_view sourcecode);

	template <> size_t lex<date>(const string_view sourcecode)
	{
		return ctre::multiline_starts_with<R"(-?\d+-\d{2}-\d{2})">(sourcecode).size();
	}

	template <> size_t lex<number>(const string_view sourcecode)
	{
		if (auto with_separators = ctre::multiline_starts_with<R"(-?\d{1,3}(,\d{3})*([.]\d+)?)">(sourcecode)) return with_separators.size();

		return ctre::multiline_starts_with<"-?\\d+([.]\\d+)?">(sourcecode).size();
	}

	template <> size_t lex<text>(const string_view sourcecode)
	{
		return ctre::multiline_starts_with<R"("([^"]|\")*")">(sourcecode).size();
	}

	template <> size_t lex<comment>(const string_view sourcecode)
	{
		if (sourcecode.starts_with("//"))
		{
			const auto length = sourcecode.find('\n', 0);
			return length == string_view::npos ? sourcecode.size() : length + 1;
		}

		if (sourcecode.starts_with("/*") == false) return 0;

		int depth = 1;
		size_t length = 2;

		for (; length != sourcecode.size(); ++length)
		{
			if (auto innerspan = sourcecode.substr(length); innerspan.starts_with("/*")) ++depth;
			else if (innerspan.starts_with("*/")) --depth;
			if (depth == 0) break;
		}

		length += 2;

		return depth == 0 ? length : 0;
	}

	template <> size_t lex<symbol>(const string_view sourcecode)
	{
		size_t length = 0;
		UChar32 character;

		U8_NEXT(sourcecode, length, sourcecode.size(), character);

		if (const auto type = u_charType(character);type != U_DASH_PUNCTUATION
			&& type != U_START_PUNCTUATION
			&& type != U_END_PUNCTUATION
			&& type != U_CONNECTOR_PUNCTUATION
			&& type != U_OTHER_PUNCTUATION
			&& type != U_MATH_SYMBOL
			&& type != U_CURRENCY_SYMBOL
			&& type != U_MODIFIER_SYMBOL
			&& type != U_OTHER_SYMBOL
			&& type != U_INITIAL_PUNCTUATION
			&& type != U_FINAL_PUNCTUATION) return 0;

		const auto source = sourcecode.substr(length);
		if (source.empty() == false) return length;

		size_t compound_assignment = 0;
		U8_NEXT(source, compound_assignment, source.size(), character);
		if (character == static_cast<char>(assign)) length += compound_assignment;

		return length;
	}

	template <> size_t lex<ellipsis>(const string_view sourcecode) { return sourcecode.starts_with(lexicon::ellipsis) ? 3 : 0; }
	template <> size_t lex<interval>(const string_view sourcecode) { return sourcecode.starts_with(lexicon::interval) ? 2 : 0; }
	template <> size_t lex<shift_left>(const string_view sourcecode) { return sourcecode.starts_with(lexicon::shift_left) ? 2 : 0; }
	template <> size_t lex<shift_right>(const string_view sourcecode) { return sourcecode.starts_with(lexicon::shift_right) ? 2 : 0; }

	template <> size_t lex<returns>(const string_view sourcecode)
	{
		return sourcecode.starts_with(lexicon::returns) ? 2 : 0;
	}

	template <> size_t lex<open_paren>(const string_view sourcecode) { return sourcecode[0] == (char)open_paren; }
	template <> size_t lex<close_paren>(const string_view sourcecode) { return sourcecode[0] == (char)close_paren; }
	template <> size_t lex<open_brace>(const string_view sourcecode) { return sourcecode[0] == (char)open_brace; }
	template <> size_t lex<close_brace>(const string_view sourcecode) { return sourcecode[0] == (char)close_brace; }
	template <> size_t lex<delegation>(const string_view sourcecode) { return sourcecode[0] == (char)delegation; }
	template <> size_t lex<separator>(const string_view sourcecode) { return sourcecode[0] == (char)separator; }
	template <> size_t lex<terminal>(const string_view sourcecode) { return sourcecode[0] == (char)terminal; }
	template <> size_t lex<assign>(const string_view sourcecode) { return sourcecode[0] == (char)assign; }

	template <token::type T> static size_t lex_keyword(string_view sourcecode)
	{
		constexpr auto keyword = magic_enum::enum_name<T>();
		return sourcecode.starts_with(keyword) && lex<whitespace>(sourcecode.substr(keyword.size())) ? keyword.size() : 0;
	}

	template <> size_t lex<var>(const string_view sourcecode) { return lex_keyword<var>(sourcecode); }
	template <> size_t lex<let>(const string_view sourcecode) { return lex_keyword<let>(sourcecode); }
	template <> size_t lex<module>(const string_view sourcecode) { return lex_keyword<module>(sourcecode); }
	template <> size_t lex<import>(const string_view sourcecode) { return lex_keyword<import>(sourcecode); }
	template <> size_t lex<type>(const string_view sourcecode) { return lex_keyword<type>(sourcecode); }
	template <> size_t lex<extend>(const string_view sourcecode) { return lex_keyword<extend>(sourcecode); }
	template <> size_t lex<iterate>(const string_view sourcecode) { return lex_keyword<iterate>(sourcecode); }

	template <> size_t lex<whitespace>(const string_view sourcecode)
	{
		constexpr static auto whitespace = {
			" " // space
			,"\t" // tab
			, "\r" // line feed
			, "\n" // carriage return
			, "\x0B" // line tabulation
			, "\x0C" // form feed
			, "\xC2\x85" // next line
			, "\xC2\xA0" // no-break space
			, "\xE1\x9A\x80" // ogham space mark
			, "\xE2\x80\x80" // en quad
			, "\xE2\x80\x81" // em quad
			, "\xE2\x80\x82" // en space
			, "\xE2\x80\x83" // em space
			, "\xE2\x80\x84" // three-per-em space
			, "\xE2\x80\x85" // four-per-em space
			, "\xE2\x80\x86" // six-per-em space
			, "\xE2\x80\x87" // figure space
			, "\xE2\x80\x88" // punctuation space
			, "\xE2\x80\x89" // thin space
			, "\xE2\x80\x8A" // hair space
			, "\xE2\x80\xAF" // narrow no-break space
			, "\xE2\x81\x9F" // medium mathematical space
			, "\xE3\x80\x80" // ideographic space
		};

		size_t length = 0;
	advance:
		auto substr = sourcecode.substr(length);
		for (const auto character : whitespace)
		{
			if (substr.starts_with(character))
			{
				length += strnlen_s(character, string::npos);
				goto advance;
			}
		}
		return length;
	}

	template <> size_t lex<word>(const string_view sourcecode)
	{
		size_t length = 0;
		UChar32 character;		
		U8_NEXT(sourcecode, length, sourcecode.size(), character);
		
		if (u_isdigit(character)) return 0;

		auto source  = sourcecode.substr(length);

		while (source.empty() == false
			&& lex<whitespace>(source) == 0
			&& lex<symbol>(source) == 0)
		{
			source = source.substr(1);
			++length;
		}

		return length;
	}

	template <token::type T> static bool lex(string_view& sourcecode, vector<token>& tokens)
	{
		const auto length = lex<T>(sourcecode);
		if (length)
		{
			tokens.emplace_back(sourcecode.data(), T);
			sourcecode = sourcecode.substr(length);
		}
		return length;
	}

	template <token::type T, token::type U, token::type... Ts> static vector<token> lex(string_view sourcecode)
	{
		vector<token> tokens;

		while (sourcecode.empty() == false)
		{
			[[maybe_unused]] bool success = lex<T>(sourcecode, tokens) || lex<U>(sourcecode, tokens) || (lex<Ts>(sourcecode, tokens) || ...);
		}

		tokens.emplace_back("", nothing);

		return tokens;
	}

	vector<token> lex(const string_view sourcecode)
	{
		return lex<
			date, number, text,
			comment,
			returns, open_paren, close_paren, open_brace, close_brace, delegation, separator, terminal, assign,
			ellipsis, interval, shift_left, shift_right,
			symbol,
			var, let, module, import, type, extend, iterate,
			whitespace,
			word>(sourcecode);
	}
}