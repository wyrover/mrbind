#include <gtest/gtest.h>

#include "mrbind.hpp"

namespace {
class Person {
  std::string name_;
  int age_;

  public:
    Person(const std::string &name, const int age)
      : name_(name), age_(age) {
    }

    std::string greeting() const {
      std::stringstream ss;
      ss << "My name is " << name_ << " and I am " << age_ << " years old.";
      return ss.str();
    }

    std::string greeting_n_times(int count) {
      std::string s;
      for (int i = 0; i < count; i++) {
        s += greeting();
      }
      return s;
    }

    int age_difference(const Person &other) const {
      return this->age_ - other.age_;
    }

    // 第1引数をreceiverとした非メンバ関数を用意
    struct MrbMethod {
      static std::string greeting(Person *self) {
        return self->greeting();
      }

      static std::string greeting_n_times(Person *self, int count) {
        return self->greeting_n_times(count);
      }

      static int age_difference(Person *self, const Person *other) {
        return self->age_difference(*other);
      }
    };
};

class mrbind_sample : public testing::Test {
  protected:
    mrbind::MRuby mruby;
};
}  // anonymous namespace

TEST_F(mrbind_sample, get_function) {
  mruby.load_string(
    "def mul(a, b)\n"
    "  return a * b\n"
    "end\n");

  // MRFunctionを生成
  auto mul = mruby.get_function<int(int, int)>("mul");
  auto mul_s = mruby.get_function<const char *(const char *, int)>("mul");
  auto mul_str = mruby.get_function<std::string(std::string, int)>("mul");

  // 引数を変えてcall
  EXPECT_EQ(200, mul(10, 20));
  EXPECT_EQ(450, mul(15, 30));

  EXPECT_STREQ("abcabcabc", mul_s("abc", 3));
  EXPECT_STREQ("zzzzz", mul_s("z", 5));

  EXPECT_EQ("abcabcabc", mul_str("abc", 3));
  EXPECT_EQ("zzzzz", mul_str("z", 5));
}

TEST_F(mrbind_sample, call) {
  mruby.load_string(
    "def mul(a, b)\n"
    "  return a * b\n"
    "end\n");

  // 関数をcall
  EXPECT_EQ(200, mruby.call<int>("mul", 10, 20));
  EXPECT_STREQ("abcabcabc", mruby.call<const char *>("mul", "abc", 3));
  EXPECT_EQ("abcabcabc", mruby.call<std::string>("mul", "abc", 3));

  // receiverを指定
  auto la = mruby.load_string("'la'");

  EXPECT_EQ("lalala", mruby.call<std::string>(la, "*", 3));
}

TEST_F(mrbind_sample, each_array) {
  auto ary = mruby.load_string("[10, 20, 30]");

  int sum = 0;
  mruby.each_array<int>(ary, [&sum](int n) { sum += n; });

  EXPECT_EQ(60, sum);
}

TEST_F(mrbind_sample, each_hash) {
  auto hash = mruby.load_string("{10 => 'a', 20 => 'b', 30 => 'c'}");

  int sum = 0;
  auto str = mruby.load_string("''");
  mruby.each_hash<int, std::string>(hash, [&](int k, std::string v) {
      sum += k;
      mruby.call(str, "<<", v);
      });

  EXPECT_EQ(60, sum);
  EXPECT_EQ(3, mruby.call<int>(str, "size"));
}

TEST_F(mrbind_sample, use_cpp_class) {
  mrbind::MRClass<Person> person_class = mruby.install_class<Person>("Person");
  person_class.define().initialize<char *, int>();
  person_class.define().method<std::string>()
    .from<&Person::MrbMethod::greeting>("greeting");
  person_class.define().method<std::string, int>()
    .from<&Person::MrbMethod::greeting_n_times>("greeting_n_times");
  person_class.define().method<int, const Person *>()
    .from<&Person::MrbMethod::age_difference>("age_difference");

  // C++
  Person alice("alice", 7);
  ASSERT_EQ(
    "My name is alice and I am 7 years old.", alice.greeting());

  // Ruby
  auto bob_greet = mruby.load_string(
    "bob = Person.new('bob', 35)\n"
    "bob.greeting\n");
  ASSERT_EQ(MRB_TT_STRING, mrb_type(bob_greet));
  EXPECT_EQ(
    "My name is bob and I am 35 years old.", mruby.to_string(bob_greet));

  // Ruby
  auto carol_greet = mruby.load_string(
    "carol = Person.new('carol', 72)\n"
    "carol.greeting_n_times(3)\n");
  EXPECT_EQ(
    "My name is carol and I am 72 years old."
    "My name is carol and I am 72 years old."
    "My name is carol and I am 72 years old.", mruby.to_string(carol_greet));
  auto diff = mruby.load_string(
    "Person.new('A', 42).age_difference(Person.new('B', 33))\n");
  EXPECT_EQ(9, mrb_fixnum(diff));

  mruby.load_string(
    "def mul(a, b)\n"
    "  return a.greeting * b\n"
    "end\n");
  auto bob = mruby.load_string("Person.new('bob', 35)");
  auto greet3 = mruby.call<std::string>("mul", bob, 3);

  EXPECT_EQ(
    "My name is bob and I am 35 years old."
    "My name is bob and I am 35 years old."
    "My name is bob and I am 35 years old.", greet3);
}

