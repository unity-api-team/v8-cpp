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

#include <v8-cpp.h>

#include "myobject.h"

#include <iostream>

#include <node.h>

using namespace v8;

class MyObject2_OL : public MyObject2
{
public:
    using MyObject2::MyObject2;

    Local<Value> plus_two(FunctionCallbackInfo<Value> const& args)
    {
        if (args.Length() == 0)
        {
            return v8cpp::to_v8(Isolate::GetCurrent(), MyObject2::plus_two());
        }
        else if (args.Length() == 1 && args[0]->IsInt32())
        {
            int arg = v8cpp::from_v8<int>(Isolate::GetCurrent(), args[0]);
            return v8cpp::to_v8(Isolate::GetCurrent(), MyObject2::plus_two(arg));
        }
        return v8cpp::to_v8(Isolate::GetCurrent(), 0);
    }
};

class SearchHandler
{
public:
    explicit SearchHandler(Local<Function> const& cb)
        : cb_(Isolate::GetCurrent(), cb)
    {
    }

    void listen()
    {
        Local<Function> cb = v8cpp::to_local<Function>(Isolate::GetCurrent(), cb_);
        v8cpp::call_v8(Isolate::GetCurrent(), cb, "hello world");
    }

private:
    Persistent<Function> cb_;
};

MyObject* new_MyObject(FunctionCallbackInfo<Value> const& args)
{
    if (args.Length() == 1 && args[0]->IsNumber())
    {
        return new MyObject(v8cpp::from_v8<double>(Isolate::GetCurrent(), args[0]));
    }
    else if (args.Length() == 1 && args[0]->IsString())
    {
        return new MyObject(v8cpp::from_v8<std::string const&>(Isolate::GetCurrent(), args[0]));
    }
    return nullptr;
}

MyObject2_OL* new_MyObject2(double value, int value2)
{
    return new MyObject2_OL(value, value2);
}

SearchHandler* new_SearchHandler(Local<Function> const& cb)
{
    return new SearchHandler(cb);
}

void InitAll(Local<Object> exports)
{
    Isolate* isolate = Isolate::GetCurrent();

    // Prepare class binding
    v8cpp::Class<MyObject> myobject(isolate);
    myobject
            .add_method("plus_one", &MyObject::plus_one)
            .add_method("add", &MyObject::add)
            .add_method("value", &MyObject::value);

    // Prepare class binding
    v8cpp::Class<MyObject2_OL> myobject2(isolate);
    myobject2
            .add_inheritance<MyObject2>()
            .set_constructor<double, int>()
            .add_method("base_method", &MyObject2::base_method)
            .add_method("static_method", &MyObject2::static_method)
            .add_method("plus_two", &MyObject2_OL::plus_two);

    // Prepare class binding
    v8cpp::Class<SearchHandler> searchhandler(isolate);
    searchhandler
            .set_constructor<Local<Function>>()
            .add_method("listen", &SearchHandler::listen);

    // Prepare struct binding
    v8cpp::Class<MyStruct> mystruct(isolate);
    mystruct
            .set_constructor()
            .add_member("bool_value", &MyStruct::bool_value)
            .add_member("int_value", &MyStruct::int_value)
            .add_member("string_value", &MyStruct::string_value)
            .add_method("output", &MyStruct::output);

    v8cpp::Module module(isolate);

    module.add_class("MyObject", myobject);
    module.add_class("MyObject2", myobject2);
    module.add_class("SearchHandler", searchhandler);
    module.add_class("MyStruct", mystruct);

    module.add_function("new_MyObject", &new_MyObject);
    module.add_function("new_MyObject2", &new_MyObject2);
    module.add_function("new_SearchHandler", &new_SearchHandler);

    exports->SetPrototype(isolate->GetCurrentContext(), module.create_prototype());
}

NODE_MODULE(addon, InitAll)
