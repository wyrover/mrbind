#include <gtest/gtest.h>

#include <mruby.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/string.h>

#include <memory>
#include <sstream>

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
};

mrb_value add(mrb_state *mrb, mrb_value self) {
  mrb_int a, b;
  mrb_get_args(mrb, "ii", &a, &b);

  return mrb_fixnum_value(a + b);
}

class mruby_sample : public testing::Test {
  protected:
    mrb_state *mrb;

    void SetUp() {
      mrb = mrb_open();
    }

    void TearDown() {
      mrb_close(mrb);
    }
};
}  // anonymous namespace

TEST_F(mruby_sample, call_c_function) {
  mrb_define_method(mrb, mrb->kernel_module, "add", add, MRB_ARGS_REQ(2));
  mrb_load_string(mrb, "@ret = add(10, 20)");
  ASSERT_FALSE(mrb->exc);

  auto ret = mrb_load_string(mrb, "@ret");
  ASSERT_FALSE(mrb->exc);

  EXPECT_EQ(30, mrb_fixnum(ret));
}

TEST_F(mruby_sample, call_ruby_function) {
  auto cxt = mrbc_context_new(mrb);

  mrb_load_string_cxt(mrb,
      "def mul(a, b)\n"
      "  return a * b\n"
      "end\n",
      cxt);

  auto ret = mrb_funcall(mrb, mrb_top_self(mrb), "mul", 2,
          mrb_fixnum_value(10), mrb_fixnum_value(20));
  ASSERT_FALSE(mrb->exc);

  EXPECT_EQ(200, mrb_fixnum(ret));
}

namespace {
template<typename T>
void free_instance(mrb_state *mrb, void *ptr) {
  delete static_cast<T *>(ptr);
}

template<typename T>
mrb_data_type make_data_type(const std::string &name) {
  return mrb_data_type({ name.c_str(), free_instance<T> });
}

static mrb_data_type person_data_type = make_data_type<Person>("Person");
}  // anonymous namespace

TEST_F(mruby_sample, use_cpp_class) {
  Person alice("alice", 7);

  ASSERT_EQ("My name is alice and I am 7 years old.", alice.greeting());

  auto person = mrb_define_class(mrb, "Person", mrb->object_class);
  MRB_SET_INSTANCE_TT(person, MRB_TT_DATA);
  mrb_define_method(mrb, person, "initialize", [](mrb_state *mrb, mrb_value self) -> mrb_value {
        DATA_TYPE(self) = &person_data_type;
        DATA_PTR(self) = NULL;

        char *name;
        int age;
        mrb_get_args(mrb, "zi", &name, &age);

        DATA_PTR(self) = new Person(name, age);
        return self;
      }, ARGS_REQ(2));
  mrb_define_method(mrb, person, "greeting", [](mrb_state *mrb, mrb_value self) -> mrb_value {
        Person *person = static_cast<Person *>(mrb_get_datatype(mrb, self, &person_data_type));
        return mrb_str_new_cstr(mrb, person->greeting().c_str());
      }, ARGS_NONE());

  auto greet = mrb_load_string(mrb,
          "p = Person.new('bob', 35)\n"
          "p.greeting\n");
  EXPECT_STREQ("My name is bob and I am 35 years old.", mrb_str_to_cstr(mrb, greet));

  auto bob = mrb_load_string(mrb, "Person.new('bob', 35)");

  mrb_load_string(mrb,
      "def mul(a, b)\n"
      "  return a.greeting * b\n"
      "end\n");
  auto greet3 = mrb_funcall(mrb, mrb_top_self(mrb), "mul", 2,
         bob, mrb_fixnum_value(3));
  EXPECT_STREQ(
      "My name is bob and I am 35 years old."
      "My name is bob and I am 35 years old."
      "My name is bob and I am 35 years old.", mrb_str_to_cstr(mrb, greet3));
}

TEST_F(mruby_sample, mrb_load_file) {
  auto load_file = [&](const std::string &filename) {
        auto closer = [](FILE *fp) {
              fclose(fp);
            };
        std::unique_ptr<FILE, decltype(closer)> f(fopen(filename.c_str(), "r"), closer);

        mrb_load_file(mrb, f.get());
      };

  for (auto file : {"./test/rule/base.rb", "./test/rule/price.rb"}) {
    load_file(file);
  }
}

TEST_F(mruby_sample, use_array) {
  auto ary = mrb_load_string(mrb,
      "[:a, :b, :c]");

  auto size = mrb_funcall(mrb, ary, "size", 0);
  EXPECT_EQ(3, mrb_fixnum(size));
  auto empty = mrb_funcall(mrb, ary, "empty?", 0);
  EXPECT_FALSE(mrb_bool(empty));
  auto a_sym = mrb_funcall(mrb, ary, "[]", 1, mrb_fixnum_value(0));
  auto a = mrb_funcall(mrb, a_sym, "to_s", 0);
  EXPECT_STREQ("a", mrb_str_to_cstr(mrb, a));
}

TEST_F(mruby_sample, use_boolean) {
  auto true_value = mrb_load_string(mrb, "true");
  auto false_value = mrb_load_string(mrb, "false");

  EXPECT_TRUE(mrb_bool(true_value));
  EXPECT_FALSE(mrb_bool(false_value));
}

