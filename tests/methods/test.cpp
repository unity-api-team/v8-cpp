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

// METHODS
// call regular class methods from JS
// call static class methods from JS
// call base class methods from JS (inheritance)
// call overridden class methods from JS (inheritance)
// call overloaded class methods from JS

TEST(Test, call_regular_method_from_js)
{
    auto result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-methods-module");
        var test_object = new module.TestClass();
        test_object.regular_method();
    )");

    EXPECT_EQ(result, 2);
}

TEST(Test, call_static_method_from_js)
{
    auto result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-methods-module");
        module.TestClass.static_method();
    )");

    EXPECT_EQ(result, 3);
}

TEST(Test, call_base_method_from_js)
{
    auto result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-methods-module");
        var test_object = new module.TestClass();
        test_object.base_method();
    )");

    EXPECT_EQ(result, 1);

    result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-methods-module");
        var test_object = new module.TestClass();
        test_object.base_base_method();
    )");

    EXPECT_EQ(result, 0);
}

TEST(Test, call_override_method_from_js)
{
    auto result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-methods-module");
        var test_object = new module.TestClass();
        test_object.virtual_method();
    )");

    EXPECT_EQ(result, 4);
}

TEST(Test, call_overload_method_from_js)
{
    auto result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-methods-module");
        var test_object = new module.TestClass();
        test_object.overload_method(5);
    )");

    EXPECT_EQ(result, 5);

    auto result2 = v8cpp::run_script<std::string>(
    R"(
        var module = require("./test-methods-module");
        var test_object = new module.TestClass();
        test_object.overload_method(1, 5);
    )");

    EXPECT_EQ(result2, "6");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
