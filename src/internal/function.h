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

#include <internal/call.h>
#include <internal/utility/function_traits.h>

namespace v8cpp
{
namespace internal
{

template <typename T>
using IsFunctionPointer = std::is_function<typename std::remove_pointer<T>::type>;

template <typename F>
using IsVoidReturn = std::is_same<void, typename FunctionTraits<F>::ReturnType>;

template <typename T>
v8::Local<v8::Value> export_value(v8::Isolate* isolate, T const& value)
{
    T* data = new T(value);

    v8::Local<v8::External> ext = v8::External::New(isolate, data);

    v8::Persistent<v8::External> pext(isolate, ext);
    pext.SetWeak(data, [](v8::WeakCallbackInfo<T> const& data)
                 {
                     delete data.GetParameter();
                 }, v8::WeakCallbackType::kParameter);

    return ext;
}

template <typename T>
T import_value(v8::Local<v8::Value> value)
{
    T* data = static_cast<T*>(value.As<v8::External>()->Value());
    return *data;
}

template <typename F>
typename std::enable_if<IsFunctionPointer<F>::value, typename FunctionTraits<F>::ReturnType>::type invoke(
    v8::FunctionCallbackInfo<v8::Value> const& args)
{
    F f = import_value<F>(args.Data());
    return call_from_v8(std::forward<F>(f), args);
}

template <typename F>
typename std::enable_if<std::is_member_function_pointer<F>::value, typename FunctionTraits<F>::ReturnType>::type invoke(
    v8::FunctionCallbackInfo<v8::Value> const& args)
{
    using Arguments = typename FunctionTraits<F>::Arguments;
    static_assert(std::tuple_size<Arguments>::value > 0, "");
    using ClassType = typename std::tuple_element<0, Arguments>::type;

    F f = import_value<F>(args.Data());
    ClassType& object = from_v8<ClassType&>(args.GetIsolate(), args.This());

    return call_from_v8(object, std::forward<F>(f), args);
}

template <typename F>
typename std::enable_if<IsVoidReturn<F>::value>::type forward_invoke(v8::FunctionCallbackInfo<v8::Value> const& args)
{
    invoke<F>(args);
}

template <typename F>
typename std::enable_if<!IsVoidReturn<F>::value>::type forward_invoke(v8::FunctionCallbackInfo<v8::Value> const& args)
{
    args.GetReturnValue().Set(to_v8(args.GetIsolate(), invoke<F>(args)));
}

template <typename F>
void forward_function(v8::FunctionCallbackInfo<v8::Value> const& args)
{
    static_assert(IsFunctionPointer<F>::value || std::is_member_function_pointer<F>::value,
                  "function pointer required");

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    try
    {
        forward_invoke<F>(args);
    }
    catch (std::exception const& e)
    {
        auto exception = isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, e.what())));
        args.GetReturnValue().Set(exception);
    }
}

// Export a C++ function to V8
template <typename F>
v8::Local<v8::FunctionTemplate> export_function(v8::Isolate* isolate, F func)
{
    return v8::FunctionTemplate::New(isolate, &forward_function<F>, export_value(isolate, func));
}

}  // namespace internal
}  // namespace v8cpp
