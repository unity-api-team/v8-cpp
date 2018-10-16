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

// OBJECTS
// construct class via new() from JS
// construct class via factory methods from JS (constructor overloads)
// move/ref objects to JS
// move/ref objects back to C++

TEST(Test, construct_class_via_new)
{
    auto test_object = v8cpp::run_script<TestClass>(
    R"(
        var module = require("./test-objects-module");
        var test_object = new module.TestClass(1, 2);
        test_object;
    )");

    EXPECT_EQ(test_object.i(), 3);
}

TEST(Test, construct_class_via_factory)
{
    auto test_object = v8cpp::run_script<TestClass>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        test_object;
    )");

    EXPECT_EQ(test_object.i(), 3);
}

TEST(Test, object_to_js)
{
    auto test_object = v8cpp::run_script<std::shared_ptr<EmbeddedTestClass>>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        test_object.embedded_class_uptr();
    )");

    EXPECT_EQ(test_object->i(), -1);

    auto test_object2 = v8cpp::run_script<std::shared_ptr<EmbeddedTestClass>>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        test_object.embedded_class_sptr();
    )");

    EXPECT_EQ(test_object2->i(), -1);

    auto test_object3 = v8cpp::run_script<EmbeddedTestClass*>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        test_object.embedded_class_ptr();
    )");

    EXPECT_EQ(test_object3->i(), -1);

    auto test_object4 = v8cpp::run_script<EmbeddedTestClass>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        test_object.embedded_class_copy();
    )");

    EXPECT_EQ(test_object4.i(), -1);
}

TEST(Test, object_from_js)
{
    auto test_object = v8cpp::run_script<TestClass>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        var test_object2 = module.new_TestClass(1, 2);

        test_object.remove_ptr(test_object2.embedded_class_copy());
        test_object;
    )");

    EXPECT_EQ(test_object.i(), 4);

    test_object = v8cpp::run_script<TestClass>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        var test_object2 = module.new_TestClass(1, 2);

        test_object.remove_ptr(null);
        test_object;
    )");

    EXPECT_EQ(test_object.i(), 3);

    test_object = v8cpp::run_script<TestClass>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        var test_object2 = module.new_TestClass(1, 2);

        test_object.remove_sptr(null);
        test_object;
    )");

    EXPECT_EQ(test_object.i(), 3);

    auto result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        var test_object2 = module.new_TestClass(1, 2);

        test_object.add_ref(test_object2.embedded_class_ref());
    )");

    EXPECT_EQ(result, 2);

    result = v8cpp::run_script<int>(
    R"(
        var module = require("./test-objects-module");
        var test_object = module.new_TestClass(1, 2);
        var test_object2 = module.new_TestClass(1, 2);

        test_object.add_ref(test_object2.embedded_class_uptr());
    )");

    EXPECT_EQ(result, 2);
}

TEST(Test, construct_var_args_class_via_new)
{
    auto test_object = v8cpp::run_script<TestClassVariableConstructorArgs>(
    R"(
        var module = require("./test-objects-module");
        var test_object = new module.TestClassVariableConstructorArgs(1, 2, "me");
        test_object;
    )");

    EXPECT_EQ(test_object.zero(), 1);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
