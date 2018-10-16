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

// MEMBERS
// get/set class/struct members from JS

TEST(Test, get_set_members_from_js)
{
    auto test_struct = v8cpp::run_script<TestStruct>(
    R"(
        var module = require("./test-members-module");
        var test_struct = new module.TestStruct();

        if (test_struct.bool_value)
        {
            test_struct.bool_value = false;
        }
        if (test_struct.int_value == 9)
        {
            test_struct.int_value = -1;
        }
        if (test_struct.float_value - 0.1 < 0.001)
        {
            test_struct.float_value = test_struct.int_value + 0.12;
        }
        if (test_struct.string_value == "hello")
        {
            test_struct.string_value = test_struct.string_value + " there";
        }

        test_struct;
    )");

    EXPECT_EQ(test_struct.bool_value, false);
    EXPECT_EQ(test_struct.int_value, -1);
    EXPECT_FLOAT_EQ(test_struct.float_value, -0.88);
    EXPECT_EQ(test_struct.string_value, "hello there");
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
