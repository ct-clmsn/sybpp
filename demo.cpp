//  Copyright (c) 2020 Christopher Taylor
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/type_traits.hpp>

#include <variant>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <tuple>

#include "sybpp.hpp"

using namespace sybpp;

struct demo {
    int here;
    std::string beer;
};

BOOST_FUSION_ADAPT_STRUCT(
    demo,
    (int, here)
    (std::string, beer)
)

struct demo_recurse {
    demo d;
};

BOOST_FUSION_ADAPT_STRUCT(
    demo_recurse,
    (demo, d)
)

struct demo_vec {
    std::vector<int> here;
    std::string beer;
};

BOOST_FUSION_ADAPT_STRUCT(
    demo_vec,
    (std::vector<int>, here)
    (std::string, beer)
)

struct demo_var {
    using var_t = std::variant<std::monostate, double, int>;
    var_t here;
    std::string beer;
};

BOOST_FUSION_ADAPT_STRUCT(
    demo_var,
    (demo_var::var_t, here)
    (std::string, beer)
)

using pair_value_t = std::pair<int, int>;

struct demo_pair {
    pair_value_t value;
};

BOOST_FUSION_ADAPT_STRUCT(
    demo_pair,
    (pair_value_t, value)
)

using tuple_value_t = std::tuple<int, int>;

struct demo_tuple {
    tuple_value_t value;
};

BOOST_FUSION_ADAPT_STRUCT(
    demo_tuple,
    (tuple_value_t, value)
)

void update_demo(demo & d) {
    d.here += 1;
}

void update_here(int & d) {
    d += 1;
}

void update_beer(std::string & d) {
    d = "3";
}

struct count {
    std::int64_t value;

    count() : value(0) {}

    void operator()(int t) {
        value++;
    }
};

void count_fn(count & c, int t) {
    c(t);
}

int main(int argc, char ** argv) {
    demo d{1, "2"};
    demo c{1, "2"};
    demo e{1, "2"};
    demo_recurse f{ demo{1, "2"} };
    demo_vec g { {0,1,2}, "here" };
    demo_var h { 0, "here" };
    demo_pair dpair;
    demo_tuple dtuple;

    // update member variable of struct
    //
    {
        using mk_transform_demo = everywhere<demo, decltype(update_demo)>;
        mk_transform_demo dt(update_demo);
        apply(dt, d);

        std::cout << "value should be 2\t" << d.here << std::endl;
    }

    // update integer
    //
    {
        using mk_transform_demo = everywhere<int, decltype(update_here)>;
        mk_transform_demo dt(update_here);
        apply(dt, c);

        std::cout << "value should be 2\t" << d.here << std::endl;
    }

    // update string
    //
    {
        using mk_transform_demo = everywhere<std::string, decltype(update_beer)>;
        mk_transform_demo dt(update_beer);
        apply(dt, e);

        std::cout << "value should be 3\t" << e.beer << std::endl;

        apply(dt, f);

        std::cout << "value should be 3\t" << f.d.beer << std::endl;
    }

    // update struct
    //
    {
        using mk_transform_demo = everywhere<int, decltype(update_here)>;
        mk_transform_demo dt(update_here);
        apply(dt, f);

        std::cout << "value should be 2\t" << f.d.here << std::endl;
    }

    // update stl-container
    //
    {
        using mk_transform_demo = everywhere<int, decltype(update_here)>;
        mk_transform_demo dt(update_here);
        apply(dt, g);

        std::cout << "input {0,1,2}; expecting {1,2,3}; computed ->\t";

        std::for_each(std::begin(g.here), std::end(g.here), [](const auto& v) {
            std::cout << v << ' ';
        });

        std::cout << std::endl;
    }

    // update variant
    //
    {
        using mk_transform_demo = everywhere<int, decltype(update_here)>;
        mk_transform_demo dt(update_here);
        apply(dt, h);

        std::cout << "value should be 1\t" << std::get<2>(h.here) << std::endl;
    }

    // update with functor
    //
    {
        count ct{};
        std::cout << "value should be 0\t" << ct.value << std::endl; 

        using mk_transform_demo = everywhere<int, decltype(ct)>;
        mk_transform_demo dt(ct);

	apply(dt, c);
        apply(dt, d);
        apply(dt, e);
        apply(dt, f);
        apply(dt, g);
        apply(dt, h);

        std::cout << "value should be 8\t" << ct.value << std::endl; 
    }

    // update with std::bind
    //
    {
        count ct{};
        std::cout << "value should be 0\t" << ct.value << std::endl; 

        auto fn = std::bind(count_fn, std::ref(ct), std::placeholders::_1);
        using mk_transform_demo = everywhere<int, decltype(fn)>;
        mk_transform_demo dt(fn);

	apply(dt, c);
        apply(dt, d);
        apply(dt, e);
        apply(dt, f);
        apply(dt, g);
        apply(dt, h);

        std::cout << "value should be 8\t" << ct.value << std::endl; 
    }

    // update with lambda
    //
    {
        count ct{};
        std::cout << "value should be 0\t" << ct.value << std::endl; 

        auto fn = [&ct](int t) { ct(t); };
        using mk_transform_demo = everywhere<int, decltype(fn)>;
        mk_transform_demo dt(fn);

	apply(dt, c);
        apply(dt, d);
        apply(dt, e);
        apply(dt, f);
        apply(dt, g);
        apply(dt, h);

        std::cout << "value should be 8\t" << ct.value << std::endl; 
    }

    {
        count ct{};

        auto fn = [&ct](int t) { ct(t); };
        using mk_transform_demo = everywhere<int, decltype(fn)>;
        mk_transform_demo dt(fn);

	apply(dt, dpair);
        std::cout << "value should be 2\t" << ct.value << std::endl; 
    }

    {
        count ct{};

        auto fn = [&ct](int t) { ct(t); };
        using mk_transform_demo = everywhere<int, decltype(fn)>;
        mk_transform_demo dt(fn);

	apply(dt, dtuple);
        std::cout << "value should be 2\t" << ct.value << std::endl; 
    }

    return 0;
}
