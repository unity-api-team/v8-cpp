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

#pragma once

#include <gtest/gtest.h>

#include <memory>

#include <v8-cpp.h>

class EmbeddedTestClass
{
public:
    EmbeddedTestClass(int a, int b)
    {
        EXPECT_EQ(a, 1);
        EXPECT_EQ(b, 2);
        i_ = a - b;
    }

    int i() const
    {
        return i_;
    }

private:
    int i_;
};

class TestClassVariableConstructorArgs
{
public:
    TestClassVariableConstructorArgs(v8::FunctionCallbackInfo<v8::Value> const& args) {
    if (args.Length() > 0 && args[0]->IsNumber()) {
        v_ = args[0]->ToNumber(v8::Isolate::GetCurrent())->Value();
      }
    }

    int zero() const {
      return v_;
    }

 private:
    int v_;
};

class TestClass
{
public:
    TestClass(int a, int b)
        : embedded_class_(std::make_shared<EmbeddedTestClass>(a, b))
    {
        EXPECT_EQ(a, 1);
        EXPECT_EQ(b, 2);
        i_ = a + b;
    }

    int i() const
    {
        return i_;
    }

    std::shared_ptr<EmbeddedTestClass> embedded_class_sptr()
    {
        return embedded_class_;
    }

    std::unique_ptr<EmbeddedTestClass> embedded_class_uptr()
    {
        return std::unique_ptr<EmbeddedTestClass>(new EmbeddedTestClass(1, 2));
    }

    EmbeddedTestClass* embedded_class_ptr()
    {
        return embedded_class_.get();
    }

    EmbeddedTestClass& embedded_class_ref()
    {
        return *embedded_class_;
    }

    EmbeddedTestClass embedded_class_copy()
    {
        return *embedded_class_;
    }

    void remove_ptr(EmbeddedTestClass* other)
    {
        if (other)
        {
            i_ -= other->i();
        }
    }

    void remove_sptr(std::shared_ptr<EmbeddedTestClass> const& other)
    {
        if (other)
        {
            i_ -= other->i();
        }
    }

    int add_ref(EmbeddedTestClass const& other)
    {
        return i_ += other.i();
    }

private:
    std::shared_ptr<EmbeddedTestClass> embedded_class_;
    int i_;
};

TestClass* new_TestClass(int a, int b)
{
    return new TestClass(a, b);
}
