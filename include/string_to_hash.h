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

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>

namespace util
{
/**
 * @brief A union that efficiently converts the first up to `N` char's of a utf-8 string into an `unsigned long long`
 *        key, that can be used as hash
 * @tparam N number of char to use for the key. Since the hash is an `unsigned long long` the char array in this union
 *           is restricted to `sizeof(unsigned long long)`
 */
template<std::uint8_t N>
union prefix_key
{
    static_assert(N != 0 && N <= sizeof(unsigned long long), "too many or too few characters for prefix_key");
    private:
    unsigned long long hash_;
    char               chars_[N];
    public:
    explicit prefix_key(const std::string &str = "") : hash_(0)
    {
        if(str.size() >= N)
        {
            // copy first N chars into the chars array
            std::memcpy(chars_, str.data(), N);
        }
        else
        {
            // copy the entire string into the chars array - it will be shorter than N
            std::memcpy(chars_, str.data(), str.size());
        }
    }

    unsigned long long hash() const
    {
        return hash_;
    }
};

/**
 * @brief A union that efficiently converts the last up to `N` char's of a utf-8 string into an `unsigned long long`
 *        key, that can be used as hash
 * @tparam N number of char to use for the key. Since the hash is an `unsigned long long` the char array in this union
 *           is restricted to `sizeof(unsigned long long)`
 */
template<std::uint8_t N>
union postfix_key
{
    static_assert(N != 0 && N <= sizeof(unsigned long long), "too many or too few characters for postfix_key");
    private:
    unsigned long long hash_;
    char               chars_[N];
    public:
    explicit postfix_key(const std::string &str = "") : hash_(0)
    {
        if(str.size() >= N)
        {
            // copy last N chars into the chars array
            std::memcpy(chars_, str.data() + (str.size() - N), N);
        }
        else
        {
            // copy the entire string into the chars array - it will be shorter than N
            std::memcpy(chars_, str.data(), str.size());
        }
    }

    unsigned long long hash() const
    {
        return hash_;
    }
};

/**
 * @brief A union that converts selected character indices of a utf-8 string into an `unsigned long long` key.
 * @tparam Indices zero-based indices to use for the key in the order provided
 */
template<std::size_t... Indices>
union enumerated_key
{
    static constexpr std::size_t count = sizeof...(Indices);
    static_assert(count != 0 && count <= sizeof(unsigned long long),
                  "too many or too few characters for enumerated_key");
    private:
    unsigned long long hash_;
    char               chars_[count];
    public:
    explicit enumerated_key(const std::string &str = "") : hash_(0)
    {
        const std::size_t indices[] = {Indices...};
        for(std::size_t i = 0; i < count; ++i)
        {
            const std::size_t idx = indices[i];
            if(idx < str.size())
            {
                chars_[i] = str[idx];
            }
            else
            {
                chars_[i] = '\0';
            }
        }
    }

    unsigned long long hash() const
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
struct is_valid_key_generator<T, std::void_t<decltype(T(std::string{})), decltype(std::declval<const T &>().hash())>>
    : std::is_integral<decltype(std::declval<const T &>().hash())>
{
};

}; // namespace util

#endif  // MESSAGETOOBJECTS_STRING_TO_HASH_H_INCLUDED
