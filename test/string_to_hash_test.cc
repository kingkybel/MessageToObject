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
