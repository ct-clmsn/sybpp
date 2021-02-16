#  Copyright (c) 2020 Christopher Taylor
#
#  SPDX-License-Identifier: BSL-1.0
#  Distributed under the Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
CXX=g++
BOOST_INC=
CXX_FLAGS=-std=c++17 -O3 $(BOOST_INC)

all:
	$(CXX) $(CXX_FLAGS) demo.cpp -o demo

clean:
	rm demo
