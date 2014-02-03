#include <gtest/gtest.h>

#include "mrbind.hpp"

namespace {
class mrbind_test : public testing::Test {
  protected:
    mrbind::MRuby mruby;
};
}  // anonymous namespace

TEST_F(mrbind_test, args_format_string) {
  auto iz = mrbind::MRuby::args_format_string<int, std::string>();
  EXPECT_EQ("iz", iz);
}

