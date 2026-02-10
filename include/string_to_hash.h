/*
 * Repository:  https://github.com/kingkybel/MessageToObject
 * File Name:   include/string_to_hash.h
 * Description: Key generator helpers for hashing message strings.
 *
 * Copyright (C) 2026 Dieter J Kybelksties <github@kybelksties.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @date: 2026-02-10
 * @author: Dieter J Kybelksties
 */

#ifndef MESSAGETOOBJECTS_STRING_TO_HASH_H_INCLUDED
#define MESSAGETOOBJECTS_STRING_TO_HASH_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>

#if __has_include("traits.h")
    #include "traits.h"
    #define MTO_HAS_TRAITS 1
#elif __has_include(<dkyb/traits.h>)
    #include <dkyb/traits.h>
    #define MTO_HAS_TRAITS 1
#endif

namespace util
{
namespace detail
{
    inline std::string_view make_view(const std::string &value)
    {
        return std::string_view{value};
    }

    inline std::string_view make_view(std::string_view value)
    {
        return value;
    }

    inline std::string_view make_view(const char *value)
    {
        return value ? std::string_view{value} : std::string_view{};
    }

    inline std::string_view make_view(char *value)
    {
        return value ? std::string_view{value} : std::string_view{};
    }

    template<std::size_t N>
    inline std::string_view make_view(const char (&value)[N])
    {
        std::size_t len = N;
        if(N > 0 && value[N - 1] == '\0')
        {
            len = N - 1;
        }
        return std::string_view{value, len};
    }

    template<std::size_t N>
    inline std::string_view make_view(char (&value)[N])
    {
        std::size_t len = N;
        if(N > 0 && value[N - 1] == '\0')
        {
            len = N - 1;
        }
        return std::string_view{value, len};
    }

#if defined(MTO_HAS_TRAITS)
    template<typename T>
    struct is_string_like
    : std::bool_constant<util::is_string<std::decay_t<T>>::value
                         && std::is_same_v<typename util::is_string<std::decay_t<T>>::char_type, char>>
    {
    };
#else
    template<typename T>
    struct is_string_like : std::false_type
    {
    };

    template<>
    struct is_string_like<std::string> : std::true_type
    {
    };

    template<>
    struct is_string_like<std::string_view> : std::true_type
    {
    };

    template<>
    struct is_string_like<const char *> : std::true_type
    {
    };

    template<>
    struct is_string_like<char *> : std::true_type
    {
    };

    template<std::size_t N>
    struct is_string_like<const char[N]> : std::true_type
    {
    };

    template<std::size_t N>
    struct is_string_like<char[N]> : std::true_type
    {
    };
#endif

    template<typename T>
    using enable_string_like_t = std::enable_if_t<is_string_like<std::decay_t<T>>::value, int>;
}  // namespace detail

/**
 * @brief A union that efficiently converts the first up to `N` char's of a utf-8 string into an `size_t`
 *        key, that can be used as hash
 * @tparam N number of char to use for the key. Since the hash is an `size_t` the char array in this union
 *           is restricted to `sizeof(size_t)`
 */
template<std::uint8_t N>
union prefix_key
{
    static_assert(N != 0 && N <= sizeof(size_t), "N nust be at least one and less or equal to sizeof(size_t)");

    private:
    size_t hash_;
    char   chars_[N];

    public:
    prefix_key() : hash_(0)
    {
    }

    template<typename Str, detail::enable_string_like_t<Str> = 0>
    explicit prefix_key(const Str &str) : hash_(0)
    {
        const std::string_view view = detail::make_view(str);
        if(view.size() >= N)
        {
            // copy first N chars into the chars array
            std::memcpy(chars_, view.data(), N);
        }
        else
        {
            // copy the entire string into the chars array - it will be shorter than N
            std::memcpy(chars_, view.data(), view.size());
        }
    }

    size_t hash() const
    {
        return hash_;
    }
};

/**
 * @brief A union that efficiently converts the last up to `N` char's of a utf-8 string into an `size_t`
 *        key, that can be used as hash
 * @tparam N number of char to use for the key. Since the hash is an `size_t` the char array in this union
 *           is restricted to `sizeof(size_t)`
 */
template<std::uint8_t N>
union postfix_key
{
    static_assert(N != 0 && N <= sizeof(size_t), "too many or too few characters for postfix_key");

    private:
    size_t hash_;
    char   chars_[N];

    public:
    postfix_key() : hash_(0)
    {
    }

    template<typename Str, detail::enable_string_like_t<Str> = 0>
    explicit postfix_key(const Str &str) : hash_(0)
    {
        const std::string_view view = detail::make_view(str);
        if(view.size() >= N)
        {
            // copy last N chars into the chars array
            std::memcpy(chars_, view.data() + (view.size() - N), N);
        }
        else
        {
            // copy the entire string into the chars array - it will be shorter than N
            std::memcpy(chars_, view.data(), view.size());
        }
    }

    size_t hash() const
    {
        return hash_;
    }
};

/**
 * @brief A union that converts selected character indices of a utf-8 string into an `size_t` key.
 * @tparam Indices zero-based indices to use for the key in the order provided
 */
template<std::size_t... Indices>
union enumerated_key
{
    static constexpr std::size_t count = sizeof...(Indices);
    static_assert(count != 0 && count <= sizeof(size_t), "too many or too few characters for enumerated_key");

    private:
    size_t hash_;
    char   chars_[count];

    public:
    enumerated_key() : hash_(0)
    {
    }

    template<typename Str, detail::enable_string_like_t<Str> = 0>
    explicit enumerated_key(const Str &str) : hash_(0)
    {
        const std::string_view view      = detail::make_view(str);
        const std::size_t      indices[] = {Indices...};
        for(std::size_t i = 0; i < count; ++i)
        {
            const std::size_t idx = indices[i];
            if(idx < view.size())
            {
                chars_[i] = view[idx];
            }
            else
            {
                chars_[i] = '\0';
            }
        }
    }

    size_t hash() const
    {
        return hash_;
    }
};

/**
 * @brief A key that parses an unsigned integer from a string starting at a given position
 *        until one of the end symbols is encountered.
 * @tparam EndSymbols sentinel characters that terminate the number
 */
template<char... EndSymbols>
struct delimited_number_key
{
    delimited_number_key() : hash_(0)
    {
    }

    template<typename Str, detail::enable_string_like_t<Str> = 0>
    explicit delimited_number_key(const Str &str, std::size_t start = 0) : hash_(0)
    {
        const std::string_view view = detail::make_view(str);
        if(start >= view.size())
        {
            return;
        }

        std::size_t value = 0;
        for(std::size_t i = start; i < view.size(); ++i)
        {
            const char c = view[i];
            if(isEndSymbol(c))
            {
                break;
            }
            if(c < '0' || c > '9')
            {
                break;
            }
            value = (value * 10u) + static_cast<std::size_t>(c - '0');
        }
        hash_ = value;
    }

    std::size_t hash() const
    {
        return hash_;
    }

private:
    std::size_t hash_;

    static constexpr bool isEndSymbol(char c)
    {
        return ((c == EndSymbols) || ...);
    }
};

/**
 * @brief A key that copies a substring from [start, end) into a fixed-size buffer like prefix_key.
 * @tparam N number of chars to copy into the key
 */
template<std::uint8_t N>
union range_key
{
    static_assert(N != 0 && N <= sizeof(size_t), "too many or too few characters for range_key");

    private:
    size_t hash_;
    char   chars_[N];

    public:
    range_key() : hash_(0)
    {
    }

    template<typename Str, detail::enable_string_like_t<Str> = 0>
    explicit range_key(const Str &str, std::size_t start = 0, std::size_t end = N) : hash_(0)
    {
        const std::string_view view = detail::make_view(str);
        if(start >= view.size() || start >= end)
        {
            return;
        }

        const std::size_t capped_end = (end > view.size()) ? view.size() : end;
        const std::size_t available  = capped_end - start;
        const std::size_t count      = (available >= N) ? N : available;
        if(count > 0)
        {
            std::memcpy(chars_, view.data() + start, count);
        }
    }

    size_t hash() const
    {
        return hash_;
    }
};

// Trait to check if a type is a valid KeyGenerator
template<typename T, typename = void>
struct is_valid_key_generator : std::false_type
{
};

template<typename T>
struct is_valid_key_generator<T, std::void_t<decltype(std::declval<const T &>().hash())>>
: std::bool_constant<std::is_integral_v<decltype(std::declval<const T &>().hash())>
                     && (std::is_constructible_v<T, std::string_view> || std::is_constructible_v<T, std::string>)>
{
};

};  // namespace util

#endif  // MESSAGETOOBJECTS_STRING_TO_HASH_H_INCLUDED
