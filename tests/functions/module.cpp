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
    v8cpp::Class<TestCaller> testcaller(isolate);
    testcaller
            .set_constructor<Local<Function>>()
            .add_method("get_shared", &TestCaller::get_shared)
            .add_method("call_me_with_shared", &TestCaller::call_me_with_shared)
            .add_method("is_null", &TestCaller::is_null)
            .add_method("call_me", &TestCaller::call_me);

    // Prepare Shared binding
    v8cpp::Class<Shared> shared(isolate);
    shared.add_method("get_value", &Shared::get_value);
    
    // Prepare module
    v8cpp::Module module(isolate);

    module.add_class("TestCaller", testcaller);
    module.add_class("Shared", shared);
    module.add_function("simple_function", &simple_function);
    module.add_function("complex_function", &complex_function);

    exports->SetPrototype(module.create_prototype());
}

V8CPP_MODULE(addon, InitAll)
