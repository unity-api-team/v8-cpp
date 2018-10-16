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

TEST(Test, empty_module_path)
{
    try
    {
        v8cpp::run_script("require()");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: require() call missing string argument");
    }
    try
    {
        v8cpp::run_script("require('')");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: require() call missing string argument");
    }
}

TEST(Test, non_existent_module)
{
    try
    {
        v8cpp::run_script("require('./noexist-module')");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: dlopen failed: ./noexist-module: cannot open shared object file: No such file or directory");
    }
}

TEST(Test, non_existent_script)
{
    try
    {
        v8cpp::run_script_file("./scripts/noexist.js");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "run_script_file(): Failed to locate script file: \"./scripts/noexist.js\"");
    }
}

TEST(Test, failing_module)
{
    try
    {
        v8cpp::run_script("require('./test-errors-fail-module')");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: dlsym failed: ./test-errors-fail-module.so: undefined symbol: init_module");
    }
}

TEST(Test, failing_script)
{
    try
    {
        v8cpp::run_script("#!/usr/bin/env sh");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "run_script(): Failed to compile script.");
    }
}

TEST(Test, failing_script_file)
{
    try
    {
        v8cpp::run_script_file("./scripts/fail.js");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "run_script(): Failed to compile script file: \"./scripts/fail.js\"");
    }
}

TEST(Test, incorrect_function_args)
{
    try
    {
        v8cpp::run_script(
        R"(
            var module = require("./test-errors-module");
            var test_object = new module.TestClass(1);
            test_object.i();
        )");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: argument count does not match the corresponding C++ function definition");
    }

    try
    {
        v8cpp::run_script(
        R"(
            var module = require("./test-errors-module");
            var test_object = new module.TestClass(1, 2);
            test_object.expect_sptr(function x(){});
        )");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: expected an exported object");
    }

    try
    {
        v8cpp::run_script(
        R"(
            var module = require("./test-errors-module");
            var test_object = new module.TestClass(1, 2);
            test_object.expect_obj(null);
        )");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: expected an exported object");
    }
}

TEST(Test, conversion_errors)
{
    try
    {
        v8cpp::run_script<std::string>("1");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected string value");
    }
    try
    {
        v8cpp::run_script<bool>("'hello'");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected bool value");
    }
    try
    {
        v8cpp::run_script<int>("'hello'");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected integer value");
    }
    try
    {
        v8cpp::run_script<float>("'hello'");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected float value");
    }
    try
    {
        v8cpp::run_script<std::vector<char>>("1");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected array value");
    }
    try
    {
        v8cpp::run_script<TestClass>("1");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected an object");
    }
    try
    {
        v8cpp::run_script<TestClass*>("1");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected an object");
    }
    try
    {
        v8cpp::run_script<TestClass>("function x(){}; x");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected an exported object");
    }
    try
    {
        v8cpp::run_script<std::list<char>>("1");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected list value");
    }
    try
    {
        v8cpp::run_script<std::set<char>>("1");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "expected set value");
    }
}

TEST(Test, throw_from_module)
{
    try
    {
        v8cpp::run_script(
        R"(
            var module = require("./test-errors-module");
            var test_object = new module.TestClass(1, 2);
            test_object.throw_ex();
        )");
    }
    catch (std::exception const& e)
    {
        EXPECT_STREQ(e.what(), "Uncaught Error: BOOM!");
    }
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
