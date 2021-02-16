//  Copyright (c) 2020 Christopher Taylor
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once
#ifndef __SYBPP_HPP__
#define __SYBPP_HPP__

#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/type_traits.hpp>

#include <type_traits>
#include <functional>
#include <algorithm>
#include <variant>
#include <iterator>
#include <optional>

namespace sybpp {

// is_iterable_trait
//
template<typename T, typename=void>
struct is_iterable : std::false_type {};

template<typename T>
struct is_iterable<T,
    std::void_t<
        decltype(std::begin(std::declval<T>())),
        decltype(std::end(std::declval<T>()))
    >
> : std::true_type {};

template<typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

// is variant trait
//
template<typename T, typename = void>
struct is_variant : std::false_type {};

template<typename T>
struct is_variant<T,
    std::void_t<
        decltype( std::declval<T>().index() ),
        decltype( std::declval<T>().valueless_by_exception() )
    >
> : std::true_type {};

// template variant_for_loop
//
template<typename WhenType, typename VariantType, std::size_t N>
struct when_variant_impl {

    // cache the current variant_alternative to test
    //
    using var_type = typename std::variant_alternative<N, VariantType>::type;

    // perform the variant_alternative test
    //
    enum { value = std::conditional<
               std::is_same< WhenType, var_type >::value,
               std::integral_constant<std::size_t, N>,
               std::integral_constant<std::size_t, when_variant_impl< WhenType, VariantType, N-1>::value >
           >::type::value
    };
};

// recursion termination condition - sets value to std::variant_npos
//
template<typename WhenType, typename VariantType>
struct when_variant_impl<WhenType, VariantType, std::variant_npos> {
    enum { value = std::variant_npos };
};

// entry point for recursion
//
template<typename WhenType, typename VariantType>
struct when_variant {
    enum { value = when_variant_impl<WhenType, VariantType,
               std::variant_size_v<VariantType>-1>::value
    };
};

// sybpp - everywhere
//
template<typename ToModify, typename ModifyFn>
struct everywhere {
    using to_modify_type = ToModify;
    using modify_fn = ModifyFn;

    using this_type = everywhere<ToModify, ModifyFn>;
    using modify_fn_ref = ModifyFn &;

    // reference to function
    //
    modify_fn_ref fn;

    // wrapper type dynamically instantiated at compile
    // time. type uses constexpr to insert execution
    // of the function reference `to_modify_fn_ref fn`
    // when the type we want to modify matches a
    // type traversed using `boost::fusion::for_each`
    //
    struct func {
        using this_type = func;
        using to_modify_type_ref = to_modify_type &;
        using fn_type = std::reference_wrapper<modify_fn>;

        fn_type fn;

        func() = delete;

        func(modify_fn_ref fn_) : fn(fn_) {}

        // at compile time, this method is `auto-magically` stamped
        // out by the compiler, the constexpr inside acts as a
        // template specialization technique
        //
        template<typename InputType>
        void operator()(InputType & in) {
            // matches types and determines when to insert function
            // reference for execution
            //
            if constexpr(std::is_same<InputType, ToModify>::value) {
                fn(in);
            }
            // recursion
            //
            else {
                // compile-time check that InputType is a variant
                //
                if constexpr ( is_variant<InputType>::value ) {
                    constexpr const std::size_t variant_index =
                        when_variant<ToModify, InputType>::value;

                    // compile-time check to make sure the variant_index
                    // isn't variant_npos
                    //
                    if constexpr( variant_index != std::variant_npos) {
                        using variant_index_type =
                            typename std::variant_alternative<variant_index, InputType>::type;

                        if constexpr( !std::is_same< std::monostate, variant_index_type >::value ) {
                            // if all the compile-time checks clear, insert this runtime check
                            // to make sure the variant is initialized at the variant_index
                            //
                            if(in.index() == variant_index) {
                                // original implementation
                                //
                                // fn(std::get<variant_index>(in));
                                //
                                // using this implementation to get compile-time checking
                                //
                                fn(std::get<variant_index_type>(in));
                            }
                        }
                    }
                }

                // handle container types with iterator support
                //
                else if constexpr(is_iterable<InputType>::value) {
                    std::for_each(std::begin(in), std::end(in), [&](auto && value) {
                        // situation where members of the container type are,
                        // themselves, typelists (boost::fusion type sequences)
                        //
                        if constexpr(boost::fusion::traits::is_sequence<decltype(value)>::value) {
                            boost::fusion::for_each(value, (*this));
                        }

                        // situation where elements are integral types,
                        // string types, or struct/class types
                        //
                        else {
                            this_type fwdfn(fn);
                            fwdfn(value);
                        }

                    });
                }
                
                // compile-time check that InputType isn't an integral type
                // or a possible std::(w)string type
                //
                else if constexpr( !std::is_same<InputType, std::string>::value &&
                    !std::is_same<InputType, std::wstring>::value &&
                    !std::is_integral<InputType>::value &&
                    !is_iterable<InputType>::value &&
                    !is_variant<InputType>::value ) {
                    boost::fusion::for_each(in, (*this));
                }
            }
        }
    };

    everywhere(ModifyFn & fn_) : fn(fn_) {
    }

    template<typename T>
    void operator()(T & in) {

        if constexpr( std::is_same<T, to_modify_type>::value ) {
            fn(in);
        }
        else {
            func fwdfn(fn);
            boost::fusion::for_each(in, fwdfn);
        }
    }
};

template<typename Transform, typename T>
void apply(Transform t, T & input) {
    t(input);
}

} // end namespace sybpp

#endif
