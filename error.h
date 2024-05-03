#pragma once

#include <string>
#include <span>
#include <mutex>
#include <utility>
#include <vector>

namespace ronin
{
	struct token;

	struct error
	{
		error(std::string reason, const std::span<token> tokens)
			: reason(std::move(reason)), tokens(tokens)
		{
			static std::mutex mutex;
			std::scoped_lock lock{ mutex };
			errors.push_back(this);
		}

		template <typename T> static error* bad(T* pointer)
		{
			return std::find(errors.begin(), errors.end(), pointer) != errors.end();
		}

		std::string reason;
		std::span<token> tokens;

		static inline std::vector<void*> errors;
	};

	template <typename T> struct bad
	{
		struct expected : T, error
		{
			expected(const std::string& reason, const std::span<token> tokens) : error("expected " + reason, tokens) {}
		};
	};
}