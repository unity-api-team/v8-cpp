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

#include <internal/require.h>

#include <v8.h>

#include <fstream>
#include <memory>

namespace v8cpp
{

template <typename T = v8::Local<v8::Value>>
T run_script(std::string const& source, std::string const& filename = "")
{
    // Create an isolate
    std::shared_ptr<v8::Isolate> isolate(v8::Isolate::New(v8::Isolate::CreateParams()), [](v8::Isolate* isolate)
    {
        // Force garbage collection before returning
        std::string const v8_flags = "--expose_gc";
        v8::V8::SetFlagsFromString(v8_flags.data(), (int)v8_flags.length());
        isolate->RequestGarbageCollectionForTesting(v8::Isolate::GarbageCollectionType::kFullGarbageCollection);

        // Clean up
        using ClassInstances = std::vector<v8cpp::internal::Class<void>*>;
        ClassInstances* instances = static_cast<ClassInstances*>(isolate->GetData(0));
        if (instances)
        {
            for (auto instance : *instances)
            {
                delete instance;
            }
        }
        delete instances;
        isolate->SetData(0, nullptr);
        isolate->Dispose();
    });

    // Create an isolate scope.
    v8::Isolate::Scope isolate_scope(isolate.get());

    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate.get());

    // Prepare console class
    v8cpp::Class<internal::Console> console(isolate.get());
    console
            .set_constructor()
            .add_method("log", &internal::Console::log);

    // Create a new context that supports "require()" and "console".
    v8cpp::Module module(isolate.get());
    {
        v8::Context::Scope context_scope(v8::Context::New(isolate.get()));

        // Store the script filename for use in require() later
        std::string script_path;
        std::size_t found = filename.find_last_of("/");
        if (found != std::string::npos)
        {
            script_path = filename.substr(0, found) + "/";
        }

        v8::Local<v8::Value> require = internal::export_value(isolate.get(), new internal::Require(script_path));
        module.object_template()->Set(isolate.get(), "require",
                                      v8::FunctionTemplate::New(isolate.get(), internal::Require::require, require));

        module.add_class("console", console);
    }
    v8::Local<v8::Context> context = v8::Context::New(isolate.get(), nullptr, module.object_template());

    // Enter the context for compiling and running the script.
    v8::Context::Scope context_scope(context);

    // Compile the script.
    v8::Local<v8::Script> script;
    v8::Script::Compile(context, v8cpp::to_v8(isolate.get(), source)).ToLocal(&script);

    // Run the script.
    if (script.IsEmpty())
    {
        if (!filename.empty())
        {
            throw std::runtime_error("run_script(): Failed to compile script file: \"" + filename + "\"");
        }
        else
        {
            throw std::runtime_error("run_script(): Failed to compile script.");
        }
    }

    v8::TryCatch try_catch(isolate.get());

    v8::Local<v8::Value> result;
    script->Run(isolate.get()->GetCurrentContext()).ToLocal(&result);

    if (try_catch.HasCaught())
    {
        throw std::runtime_error(v8cpp::from_v8<std::string>(isolate.get(), try_catch.Message()->Get()));
    }

    return v8cpp::from_v8<T>(isolate.get(), result);
}

template <typename T = v8::Local<v8::Value>>
T run_script_file(std::string const& filename)
{
    std::ifstream stream(filename.c_str());
    if (!stream)
    {
        throw std::runtime_error("run_script_file(): Failed to locate script file: \"" + filename + "\"");
    }

    std::istreambuf_iterator<char> begin(stream), end;
    return run_script<T>(std::string(begin, end), filename);
}
}
