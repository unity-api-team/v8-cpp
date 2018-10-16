/*
 * Copyright (C) 2015 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Marcus Tomlinson <marcus.tomlinson@canonical.com>
 */

#include "test.h"

#include <v8-cpp.h>

#include <gtest/gtest.h>

// FUNCTIONS
// call JS function from C++ (callback)
// call C++ functions from JS

TEST(Test, call_to_js)
{
    auto callback_result = v8cpp::run_script<std::string>(
    R"(
        var module = require("./test-functions-module");

        var caller = new module.TestCaller(function(message)
        {
            return message + " world" // "hello world"
        });

        caller.call_me();
    )");

    EXPECT_EQ(callback_result, "hello world");
}

TEST(Test, call_to_js_with_self)
{
    auto callback_result = v8cpp::run_script<std::string>(
    R"(
        var module = require("./test-functions-module");

        var caller = new module.TestCaller(function() {})

        var s = caller.get_shared()
        caller.call_me_with_shared(s, function() {
            return this.get_value()
        });
    )");

    EXPECT_EQ(callback_result, "hello");
}

TEST(Test, call_from_js)
{
    auto result = v8cpp::run_script<std::string>(
    R"(
        var module = require("./test-functions-module");
        module.simple_function();
    )");

    EXPECT_EQ(result, "hello there");

    auto result2 = v8cpp::run_script<std::vector<float>>(
    R"(
        var module = require("./test-functions-module");
        module.complex_function(4, "3", 2.1, false);
    )");

    ASSERT_EQ(result2.size(), 4);
    EXPECT_FLOAT_EQ(result2[0], 4);
    EXPECT_FLOAT_EQ(result2[1], 3);
    EXPECT_FLOAT_EQ(result2[2], 2.1);
    EXPECT_FLOAT_EQ(result2[3], 0);

    auto result3 = v8cpp::run_script<std::list<float>>(
    R"(
        var module = require("./test-functions-module");
        module.complex_list_function(4, "3", 2.1, false);
    )");

    ASSERT_EQ(result3.size(), 4);
    EXPECT_FLOAT_EQ(result3.front(), 4);
    result3.pop_front();
    EXPECT_FLOAT_EQ(result3.front(), 3);
    result3.pop_front();
    EXPECT_FLOAT_EQ(result3.front(), 2.1);
    result3.pop_front();
    EXPECT_FLOAT_EQ(result3.front(), 0);
    result3.pop_front();

    auto result4 = v8cpp::run_script<std::set<std::string>>(
    R"(
        var module = require("./test-functions-module");
        module.complex_set_function(4, "3", false);
    )");

    ASSERT_EQ(result4.size(), 3);
    EXPECT_FLOAT_EQ(result4.count("4"), 1);
    EXPECT_FLOAT_EQ(result4.count("3"), 1);
    EXPECT_FLOAT_EQ(result4.count("0"), 1);
}

TEST(Test, call_return_enum)
{
    auto result = v8cpp::run_script<TestCaller::TestEnum>(
    R"(
        var module = require("./test-functions-module");
        var tc = new module.TestCaller(function() {});
        tc.return_enum_function();
    )");

    EXPECT_EQ(result, TestCaller::TestEnum::ONE);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
