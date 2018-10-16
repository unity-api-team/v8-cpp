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

    // Prepare TestClass binding
    v8cpp::Class<TestClass> testclass(isolate);
    testclass
            .set_constructor<int, int>()
            .add_method("i", &TestClass::i)
            .add_method("expect_sptr", &TestClass::expect_sptr)
            .add_method("expect_obj", &TestClass::expect_obj)
            .add_method("throw_ex", &TestClass::throw_ex);

    // Prepare module
    v8cpp::Module module(isolate);

    module.add_class("TestClass", testclass);

    exports->SetPrototype(isolate->GetCurrentContext(), module.create_prototype());
}

V8CPP_MODULE(addon, InitAll)
