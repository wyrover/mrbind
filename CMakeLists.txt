cmake_minimum_required(VERSION 2.8)
PROJECT(mrbind)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O0 -I/usr/include/ -I/usr/local/include/")

INCLUDE_DIRECTORIES(
  include/
)

ADD_EXECUTABLE(exec_test
  test/mruby_sample.cc
  test/mrbind_sample.cc
  test/mrbind_test.cc
)

TARGET_LINK_LIBRARIES(exec_test
  gflags glog gtest gtest_main mruby
)

