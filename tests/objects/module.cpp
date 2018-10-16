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

using namespace v8;

void InitAll(Local<Object> exports)
{
    // Get current isolate
    Isolate* isolate = Isolate::GetCurrent();

    // Prepare TestClassVariableConstructorArgs binding
    v8cpp::Class<TestClassVariableConstructorArgs> testvarargsclass(isolate);
    testvarargsclass
      .set_constructor<v8::FunctionCallbackInfo<v8::Value>>()
      .add_method("zero", &TestClassVariableConstructorArgs::zero);

    // Prepare TestClass binding
    v8cpp::Class<TestClass> testclass(isolate);
    testclass
            .set_constructor<int, int>()
            .add_method("i", &TestClass::i)
            .add_method("embedded_class_sptr", &TestClass::embedded_class_sptr)
            .add_method("embedded_class_uptr", &TestClass::embedded_class_uptr)
            .add_method("embedded_class_ptr", &TestClass::embedded_class_ptr)
            .add_method("embedded_class_ref", &TestClass::embedded_class_ref)
            .add_method("embedded_class_copy", &TestClass::embedded_class_copy)
            .add_method("remove_ptr", &TestClass::remove_ptr)
            .add_method("remove_sptr", &TestClass::remove_sptr)
            .add_method("add_ref", &TestClass::add_ref);

    // Prepare EmbeddedTestClass binding
    v8cpp::Class<EmbeddedTestClass> embeddedtestclass(isolate);
    embeddedtestclass
            .set_constructor<int, int>()
            .add_method("i", &EmbeddedTestClass::i);

    // Prepare module
    v8cpp::Module module(isolate);

    module.add_class("TestClass", testclass);
    module.add_class("EmbeddedTestClass", embeddedtestclass);
    module.add_class("TestClassVariableConstructorArgs", testvarargsclass);

    module.add_function("new_TestClass", &new_TestClass);

    exports->SetPrototype(isolate->GetCurrentContext(), module.create_prototype());
}

V8CPP_MODULE(addon, InitAll)
