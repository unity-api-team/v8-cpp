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

#include <climits>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include <v8.h>

namespace v8cpp
{
namespace internal
{

template <typename T>
class Class;

// Generic convertor
template <typename T, typename Enable = void>
struct Convert;

// String converter
template <typename Char, typename Traits, typename Alloc>
struct Convert<std::basic_string<Char, Traits, Alloc>>
{
    static_assert(sizeof(Char) <= sizeof(uint16_t), "only UTF8 and UTF16 strings supported");

    using FromType = std::basic_string<Char, Traits, Alloc>;
    using ToType = v8::Local<v8::String>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsString();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected string value");
        }

        if (sizeof(Char) == 1)
        {
            v8::String::Utf8Value str(isolate, value);
            return FromType(reinterpret_cast<Char const*>(*str), str.length());
        }
        else
        {
            v8::String::Value str(isolate, value);
            return FromType(reinterpret_cast<Char const*>(*str), str.length());
        }
    }

    static ToType to_v8(v8::Isolate* isolate, FromType const& value)
    {
        if (sizeof(Char) == 1)
        {
            return v8::String::NewFromUtf8(isolate, reinterpret_cast<char const*>(value.data()),
                                           v8::String::kNormalString, static_cast<int>(value.length()));
        }
        else
        {
            return v8::String::NewFromTwoByte(isolate, reinterpret_cast<uint16_t const*>(value.data()),
                                              v8::String::kNormalString, static_cast<int>(value.length()));
        }
    }
};

// Bool converter
template <>
struct Convert<bool>
{
    using FromType = bool;
    using ToType = v8::Local<v8::Boolean>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsBoolean();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected bool value");
        }
        return value->ToBoolean(isolate)->Value();
    }

    static ToType to_v8(v8::Isolate* isolate, bool value)
    {
        return v8::Boolean::New(isolate, value);
    }
};

// Int converter
template <typename T>
struct Convert<T, typename std::enable_if<std::is_integral<T>::value>::type>
{
    using FromType = T;
    using ToType = v8::Local<v8::Number>;

    enum
    {
        bits = sizeof(T) * CHAR_BIT,
        is_signed = std::is_signed<T>::value
    };

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsNumber();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected integer value");
        }

        if (bits <= 32)
        {
            if (is_signed)
            {
                return static_cast<T>(value->Int32Value(isolate->GetCurrentContext()).FromJust());
            }
            else
            {
                return static_cast<T>(value->Uint32Value(isolate->GetCurrentContext()).FromJust());
            }
        }
        else
        {
            return static_cast<T>(value->IntegerValue(isolate->GetCurrentContext()).FromJust());
        }
    }

    template <typename O>
    static ToType to_v8(v8::Isolate* isolate, O value)
    {
        if (bits <= 32)
        {
            if (is_signed)
            {
                return v8::Integer::New(isolate, static_cast<int32_t>(value));
            }
            else
            {
                return v8::Integer::NewFromUnsigned(isolate, static_cast<uint32_t>(value));
            }
        }
        else
        {
            return v8::Number::New(isolate, static_cast<double>(value));
        }
    }
};

// Enum converter
template <typename T>
struct Convert<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
    using UnderlyingType = typename std::underlying_type<T>::type;

    using FromType = T;
    using ToType = typename Convert<UnderlyingType>::ToType;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsNumber();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected enum value");
        }
        return static_cast<T>(Convert<UnderlyingType>::from_v8(isolate, value));
    }

    static ToType to_v8(v8::Isolate* isolate, T value)
    {
        return Convert<UnderlyingType>::to_v8(isolate, value);
    }
};

// Float converter
template <typename T>
struct Convert<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
{
    using FromType = T;
    using ToType = v8::Local<v8::Number>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsNumber();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected float value");
        }

        return static_cast<T>(value->NumberValue(isolate->GetCurrentContext()).FromJust());
    }

    static ToType to_v8(v8::Isolate* isolate, T value)
    {
        return v8::Number::New(isolate, value);
    }
};

// Array converter
template <typename T, typename Alloc>
struct Convert<std::vector<T, Alloc>>
{
    using FromType = std::vector<T, Alloc>;
    using ToType = v8::Local<v8::Array>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsArray();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected array value");
        }

        v8::HandleScope scope(isolate);
        v8::Local<v8::Array> array = value.As<v8::Array>();

        FromType result;
        result.reserve(array->Length());
        for (uint32_t i = 0, count = array->Length(); i < count; ++i)
        {
            result.emplace_back(Convert<T>::from_v8(isolate, array->Get(i)));
        }
        return result;
    }

    static ToType to_v8(v8::Isolate* isolate, FromType const& value)
    {
        v8::EscapableHandleScope scope(isolate);

        uint32_t size = static_cast<uint32_t>(value.size());
        v8::Local<v8::Array> result = v8::Array::New(isolate, size);
        for (uint32_t i = 0; i < size; ++i)
        {
            result->Set(i, Convert<T>::to_v8(isolate, value[i]));
        }
        return scope.Escape(result);
    }
};

// List converter
template <typename T, typename Alloc>
struct Convert<std::list<T, Alloc>>
{
    using FromType = std::list<T, Alloc>;
    using ToType = v8::Local<v8::Array>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsArray();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected list value");
        }

        v8::HandleScope scope(isolate);
        v8::Local<v8::Array> array = value.As<v8::Array>();

        FromType result;
        for (uint32_t i = 0, count = array->Length(); i < count; ++i)
        {
            result.push_back(Convert<T>::from_v8(isolate, array->Get(i)));
        }
        return result;
    }

    static ToType to_v8(v8::Isolate* isolate, FromType const& value)
    {
        v8::EscapableHandleScope scope(isolate);

        uint32_t size = static_cast<uint32_t>(value.size());
        v8::Local<v8::Array> result = v8::Array::New(isolate, size);
        size_t i = 0;
        for (auto & e: value)
        {
            result->Set(i, Convert<T>::to_v8(isolate, e));
            ++i;
        }
        return scope.Escape(result);
    }
};

// Set converter
template <typename T, typename Alloc>
struct Convert<std::set<T, Alloc>>
{
    using FromType = std::set<T, Alloc>;
    using ToType = v8::Local<v8::Array>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsArray();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected set value");
        }

        v8::HandleScope scope(isolate);
        v8::Local<v8::Array> array = value.As<v8::Array>();

        FromType result;
        for (uint32_t i = 0, count = array->Length(); i < count; ++i)
        {
            result.insert(Convert<T>::from_v8(isolate, array->Get(i)));
        }
        return result;
    }

    static ToType to_v8(v8::Isolate* isolate, FromType const& value)
    {
        v8::EscapableHandleScope scope(isolate);

        uint32_t size = static_cast<uint32_t>(value.size());
        v8::Local<v8::Array> result = v8::Array::New(isolate, size);
        size_t i = 0;
        for (auto & e: value)
        {
            result->Set(i, Convert<T>::to_v8(isolate, e));
            ++i;
        }
        return scope.Escape(result);
    }
};

// Map converter
template <typename Key, typename Value, typename Less, typename Alloc>
struct Convert<std::map<Key, Value, Less, Alloc>>
{
    using FromType = std::map<Key, Value, Less, Alloc>;
    using ToType = v8::Local<v8::Object>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && value->IsObject();
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected an object");
        }

        v8::HandleScope scope(isolate);
        v8::Local<v8::Object> object = value.As<v8::Object>();
        v8::Local<v8::Array> prop_names = object->GetPropertyNames();

        FromType result;
        for (uint32_t i = 0, count = prop_names->Length(); i < count; ++i)
        {
            v8::Local<v8::Value> key = prop_names->Get(i);
            v8::Local<v8::Value> val = object->Get(key);
            result.emplace(Convert<Key>::from_v8(isolate, key), Convert<Value>::from_v8(isolate, val));
        }
        return result;
    }

    static ToType to_v8(v8::Isolate* isolate, FromType const& value)
    {
        v8::EscapableHandleScope scope(isolate);

        v8::Local<v8::Object> result = v8::Object::New(isolate);
        for (auto const& item : value)
        {
            result->Set(Convert<Key>::to_v8(isolate, item.first), Convert<Value>::to_v8(isolate, item.second));
        }
        return scope.Escape(result);
    }
};

// v8::Local converter
template <typename T>
struct Convert<v8::Local<T>>
{
    using FromType = v8::Local<T>;
    using ToType = v8::Local<T>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.As<T>().IsEmpty();
    }

    static v8::Local<T> from_v8(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return value.As<T>();
    }

    static v8::Local<T> to_v8(v8::Isolate*, v8::Local<T> value)
    {
        return value;
    }
};

// v8::Persistent converter
template <typename T>
struct Convert<v8::Persistent<T>>
{
    using FromType = v8::Local<T>;
    using ToType = v8::Local<T>;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.As<T>().IsEmpty();
    }

    static v8::Local<T> from_v8(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return value.As<T>();
    }

    static v8::Local<T> to_v8(v8::Isolate*, v8::Persistent<T> value)
    {
        return value;
    }
};

// Exported class converters
template <typename T>
struct IsExportedClass : std::is_class<T> {};

template <typename T>
struct IsExportedClass<v8::Local<T>> : std::false_type {};

template <typename T>
struct IsExportedClass<v8::Persistent<T>> : std::false_type {};

template <typename Char, typename Traits, typename Alloc>
struct IsExportedClass<std::basic_string<Char, Traits, Alloc>> : std::false_type {};

template <typename T, typename Alloc>
struct IsExportedClass<std::vector<T, Alloc>> : std::false_type {};

template <typename T, typename Alloc>
struct IsExportedClass<std::list<T, Alloc>> : std::false_type {};

template <typename T, typename Alloc>
struct IsExportedClass<std::set<T, Alloc>> : std::false_type {};

template <typename Key, typename Value, typename Less, typename Alloc>
struct IsExportedClass<std::map<Key, Value, Less, Alloc>> : std::false_type {};

template <typename T>
struct IsUniquePointer : std::false_type {};

template <typename T>
struct IsUniquePointer<std::unique_ptr<T>> : std::true_type { using type = T; };

template <typename T>
struct IsUniquePointer<std::unique_ptr<T const>> : std::true_type { using type = T; };

template <typename T>
struct Convert<T, typename std::enable_if<IsExportedClass<T>::value>::type>
{
    using FromType = T&;
    using ToType = v8::Local<v8::Object>;
    using ClassType = typename std::remove_cv<T>::type;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && (value->IsObject() || value->IsNull());
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected an object");
        }
        if (T* object = Convert<T*>::from_v8(isolate, value))
        {
            return *object;
        }
        throw std::runtime_error("expected an exported object");
    }

    template <typename O>
    static typename std::enable_if<IsUniquePointer<O>::value, ToType>::type to_v8(v8::Isolate* isolate, O const& value)
    {
        // Convert unique pointers to shared pointers before exposing them to v8
        std::shared_ptr<typename IsUniquePointer<T>::type> s_ptr{std::move(const_cast<T&>(value))};
        return Convert<std::shared_ptr<typename IsUniquePointer<T>::type>>::to_v8(isolate, s_ptr);
    }

    template <typename O>
    static typename std::enable_if<!IsUniquePointer<O>::value, ToType>::type to_v8(v8::Isolate* isolate, O const& value)
    {
        return Class<ClassType>::instance(isolate).export_object(new T(value));
    }
};

template <typename T>
struct Convert<T*, typename std::enable_if<IsExportedClass<T>::value>::type>
{
    using FromType = T*;
    using ToType = v8::Local<v8::Object>;
    using ClassType = typename std::remove_cv<T>::type;

    static bool is_valid(v8::Isolate*, v8::Local<v8::Value> value)
    {
        return !value.IsEmpty() && (value->IsObject() || value->IsNull());
    }

    static FromType from_v8(v8::Isolate* isolate, v8::Local<v8::Value> value)
    {
        if (!is_valid(isolate, value))
        {
            throw std::invalid_argument("expected an object");
        }
        return Class<ClassType>::instance(isolate).template import_object<T>(value);
    }

    static ToType to_v8(v8::Isolate* isolate, T const* value)
    {
        return Class<ClassType>::instance(isolate).export_object(value);
    }
};

template <typename T>
struct Convert<T&> : Convert<T>
{
};

template <typename T>
struct Convert<T const&> : Convert<T>
{
};

}  // namespace internal
}  // namespace v8cpp
