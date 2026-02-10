/*
 * Repository:  https://github.com/kingkybel/MessageToObject
 * File Name:   test/string_to_hash_test.cc
 * Description: Unit tests for string_to_hash key generators.
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

#include "string_to_hash.h"

#include <gtest/gtest.h>
#include <string>
#include <string_view>

TEST(StringToHashTest, PrefixKeyUsesFirstN)
{
    const std::string input = "abcdef";
    util::prefix_key<4> full(input);
    util::prefix_key<4> expected("abcd");
    EXPECT_EQ(full.hash(), expected.hash());
}

TEST(StringToHashTest, PrefixKeyShortStringZeroPads)
{
    const std::string input = "ab";
    util::prefix_key<4> key(input);
    util::prefix_key<4> expected("ab");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, PostfixKeyUsesLastN)
{
    const std::string input = "abcdef";
    util::postfix_key<3> key(input);
    util::prefix_key<3> expected("def");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, PostfixKeyShortStringZeroPads)
{
    const std::string input = "ab";
    util::postfix_key<5> key(input);
    util::prefix_key<5> expected("ab");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, EnumeratedKeyUsesSpecifiedIndices)
{
    const std::string input = "abcdef";
    util::enumerated_key<0, 2, 4> key(input);
    util::prefix_key<3> expected("ace");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, EnumeratedKeyOutOfRangeZeroPads)
{
    const std::string input = "abc";
    util::enumerated_key<0, 5, 10> key(input);

    std::string expected_str;
    expected_str.resize(3);
    expected_str[0] = 'a';
    expected_str[1] = '\0';
    expected_str[2] = '\0';

    util::prefix_key<3> expected(expected_str);
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, AcceptsStringViewWithoutConversion)
{
    const std::string      input = "abcdef";
    const std::string_view view  = input;
    util::prefix_key<4>    key(view);
    util::prefix_key<4>    expected("abcd");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, AcceptsCharPointer)
{
    const char *input = "abcdef";
    util::postfix_key<3> key(input);
    util::prefix_key<3> expected("def");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, AcceptsCharArray)
{
    const char input[] = "abcdef";
    util::enumerated_key<1, 3, 5> key(input);
    util::prefix_key<3> expected("bdf");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, ValidKeyGeneratorSupportsStringView)
{
    static_assert(util::is_valid_key_generator<util::prefix_key<4>>::value,
                  "prefix_key should be a valid key generator");
    static_assert(std::is_constructible_v<util::prefix_key<4>, std::string_view>,
                  "prefix_key should be constructible from string_view");
}

TEST(StringToHashTest, DelimitedNumberKeyParsesUntilEndSymbol)
{
    const char input[] = "9=1234|35=D|";
    util::delimited_number_key<'|', '\x01'> key(input, 2);
    EXPECT_EQ(key.hash(), static_cast<size_t>(1234));
}

TEST(StringToHashTest, DelimitedNumberKeyStopsOnNonDigit)
{
    const std::string input = "len=12A5|";
    util::delimited_number_key<'|'> key(input, 4);
    EXPECT_EQ(key.hash(), static_cast<size_t>(12));
}

TEST(StringToHashTest, RangeKeyCopiesSubstring)
{
    const std::string input = "abcdef";
    util::range_key<3> key(input, 1, 4);
    util::prefix_key<3> expected("bcd");
    EXPECT_EQ(key.hash(), expected.hash());
}

TEST(StringToHashTest, RangeKeyHandlesShortRange)
{
    const char input[] = "ab";
    util::range_key<4> key(input, 0, 2);
    util::prefix_key<4> expected("ab");
    EXPECT_EQ(key.hash(), expected.hash());
}
