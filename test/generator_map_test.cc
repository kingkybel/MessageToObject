/*
 * Repository:  https://github.com/kingkybel/MessageToObject
 * File Name:   test/generator_map_test.cc
 * Description: Unit tests for generator_map and key generators.
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

#include "generator_map.h"
#include "msg_generated_object.h"
#include "string_to_hash.h"

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <utility>

// Tests for object_map
class GeneratorMapTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
        // Ensure map is clean before each test
        util::generator_map<4>::clear();
    }

    void TearDown() override
    {
        util::generator_map<4>::clear();
    }
};

// Define a dummy object for testing purposes
class TestObject : public util::MsgGeneratedObjectIfc
{
    public:
    std::string data;

    explicit TestObject(std::string d = "") : data(std::move(d))
    {
    }

    std::shared_ptr<util::MsgGeneratedObjectIfc> generate_from_msg(const std::string& message)
    {
        return std::make_shared<TestObject>(message);
    }
};

TEST_F(GeneratorMapTest, RegisterAndGet)
{
    // Register a generator for "TEST"
    util::generator_map<4>::registerGenerator("TEST",
                                              [](const std::string& msg) { return std::make_shared<TestObject>(msg); });

    // Get the object
    auto obj = util::generator_map<4>::get("TEST: some data");
    ASSERT_NE(obj, nullptr);

    // Check the object's data
    auto testObj = std::dynamic_pointer_cast<TestObject>(obj);
    ASSERT_NE(testObj, nullptr);
    EXPECT_EQ(testObj->data, "TEST: some data");
}

TEST_F(GeneratorMapTest, GetNonExistent)
{
    // No generator registered for "NONE"
    auto obj = util::generator_map<4>::get("NONE: some data");
    EXPECT_EQ(obj, nullptr);
}

TEST_F(GeneratorMapTest, ClearMap)
{
    // Register a generator
    util::generator_map<4>::registerGenerator("TEST",
                                              [](const std::string& msg) { return std::make_shared<TestObject>(msg); });

    // Clear the map
    util::generator_map<4>::clear();

    // Try to get the object again
    auto obj = util::generator_map<4>::get("TEST: some data");
    EXPECT_EQ(obj, nullptr);
}

TEST_F(GeneratorMapTest, RegisterMultiple)
{
    // Register multiple generators
    util::generator_map<4>::registerGenerator("ONE-",
                                              [](const std::string& msg) { return std::make_shared<TestObject>(msg); });
    util::generator_map<4>::registerGenerator("TWO-",
                                              [](const std::string& msg) { return std::make_shared<TestObject>(msg); });

    // Get both objects
    auto obj1 = util::generator_map<4>::get("ONE-data1");
    auto obj2 = util::generator_map<4>::get("TWO-data2");

    ASSERT_NE(obj1, nullptr);
    ASSERT_NE(obj2, nullptr);

    auto testObj1 = std::dynamic_pointer_cast<TestObject>(obj1);
    auto testObj2 = std::dynamic_pointer_cast<TestObject>(obj2);

    ASSERT_NE(testObj1, nullptr);
    ASSERT_NE(testObj2, nullptr);

    EXPECT_EQ(testObj1->data, "ONE-data1");
    EXPECT_EQ(testObj2->data, "TWO-data2");
}

TEST(GeneratorMapKeyGeneratorTest, PostfixKeyMatchesBySuffix)
{
    using Map = util::generator_map<3, util::postfix_key<3>>;
    Map::clear();

    Map::registerGenerator("ABC-END",
                           [](const std::string &msg) { return std::make_shared<TestObject>(msg); });

    auto obj = Map::get("SOME-PREFIX-END");
    ASSERT_NE(obj, nullptr);

    auto testObj = std::dynamic_pointer_cast<TestObject>(obj);
    ASSERT_NE(testObj, nullptr);
    EXPECT_EQ(testObj->data, "SOME-PREFIX-END");
}

TEST(GeneratorMapKeyGeneratorTest, EnumeratedKeyMatchesSpecifiedIndices)
{
    using Map = util::generator_map<3, util::enumerated_key<0, 2, 4>>;
    Map::clear();

    Map::registerGenerator("abcdef",
                           [](const std::string &msg) { return std::make_shared<TestObject>(msg); });

    auto obj = Map::get("aXcXeX");
    ASSERT_NE(obj, nullptr);

    auto testObj = std::dynamic_pointer_cast<TestObject>(obj);
    ASSERT_NE(testObj, nullptr);
    EXPECT_EQ(testObj->data, "aXcXeX");
}
