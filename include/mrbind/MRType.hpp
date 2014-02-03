#ifndef INCLUDE_MRBIND_MR_TYPE_HPP__
#define INCLUDE_MRBIND_MR_TYPE_HPP__
#include <mruby.h>
#include <mruby/data.h>
#include <mruby/string.h>

#include <string>

namespace mrbind {
template<typename T>
struct MRType;

template<>
struct MRType<int> {
  typedef mrb_int argument_type;

  static int expand_argument(argument_type arg) {
    return arg;
  }

  static mrb_int to_c_value(mrb_state *, mrb_value v) {
    return mrb_fixnum(v);
  }

  static mrb_value to_mrb_value(mrb_state *, mrb_int v) {
    return mrb_fixnum_value(v);
  }

  static std::string arg_char() {
    return "i";
  }
};

template<>
struct MRType<bool> {
  typedef int argument_type;

  static bool expand_argument(argument_type arg) {
    return static_cast<bool>(arg);
  }

  static bool to_c_value(mrb_state *, mrb_value v) {
    return static_cast<bool>(mrb_bool(v));
  }

  static mrb_value to_mrb_value(mrb_state *, bool v) {
    return mrb_bool_value(static_cast<int>(v));
  }

  static std::string arg_char() {
    return "b";
  }
};

template<>
struct MRType<std::string> {
  typedef char *argument_type;

  static std::string expand_argument(argument_type arg) {
    return arg;
  }

  static std::string to_c_value(mrb_state *state, mrb_value v) {
    return mrb_str_to_cstr(state, v);
  }

  static mrb_value to_mrb_value(mrb_state *state, const std::string &v) {
    return mrb_str_new_cstr(state, v.c_str());
  }

  static std::string arg_char() {
    return "z";
  }
};

template<>
struct MRType<const char *> {
  typedef char *argument_type;

  static const char *expand_argument(argument_type arg) {
    return arg;
  }

  static const char *to_c_value(mrb_state *state, mrb_value v) {
    return mrb_str_to_cstr(state, v);
  }

  static mrb_value to_mrb_value(mrb_state *state, const char *v) {
    return mrb_str_new_cstr(state, v);
  }

  static std::string arg_char() {
    return "z";
  }
};

template<>
struct MRType<char *> {
  typedef char *argument_type;

  static char *expand_argument(argument_type arg) {
    return arg;
  }

  static char *to_c_value(mrb_state *state, mrb_value v) {
    return mrb_str_to_cstr(state, v);
  }

  static mrb_value to_mrb_value(mrb_state *state, char *v) {
    return mrb_str_new_cstr(state, v);
  }

  static std::string arg_char() {
    return "z";
  }
};

template<>
struct MRType<mrb_sym> {
  typedef mrb_sym argument_type;

  static mrb_sym expand_argument(argument_type arg) {
    return arg;
  }

  static mrb_sym to_c_value(mrb_state *, mrb_value v) {
    return mrb_symbol(v);
  }

  static mrb_value to_mrb_value(mrb_state *, mrb_sym v) {
    return mrb_symbol_value(v);
  }

  static std::string arg_char() {
    return "n";
  }
};

template<>
struct MRType<mrb_value> {
  typedef mrb_value argument_type;

  static mrb_value expand_argument(argument_type arg) {
    return arg;
  }

  static mrb_value to_c_value(mrb_state *, mrb_value v) {
    return v;
  }

  static mrb_value to_mrb_value(mrb_state *, mrb_value v) {
    return v;
  }

  static std::string arg_char() {
    return "o";
  }
};

template<typename T>
struct MRType<T *> {
  typedef mrb_value argument_type;

  static T *expand_argument(argument_type arg) {
    return static_cast<T *>(DATA_PTR(arg));
  }

  static T *to_c_value(mrb_state *state, mrb_value v);

  static mrb_value to_mrb_value(mrb_state *state, T *v);

  static std::string arg_char() {
    return "o";
  }
};

template<typename T>
struct MRType<const T *> {
  typedef mrb_value argument_type;

  static const T *expand_argument(argument_type arg) {
    return static_cast<const T *>(DATA_PTR(arg));
  }

  static const T *to_c_value(mrb_state *state, mrb_value v);

  static mrb_value to_mrb_value(mrb_state *state, const T *v);

  static std::string arg_char() {
    return "o";
  }
};
}  // namespace mrbind
#endif  // INCLUDE_MRBIND_MR_TYPE_HPP__

