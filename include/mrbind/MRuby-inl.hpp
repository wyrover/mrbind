#ifndef INCLUDE_MRBIND_M_RUBY_INL_HPP__
#define INCLUDE_MRBIND_M_RUBY_INL_HPP__
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/data.h>

#include <string>
#include <memory>

namespace mrbind {
inline void MRuby::StateCloser::operator()(mrb_state *mrb) {
  mrb_close(mrb);
}
inline void MRuby::ContextCloser::operator()(mrbc_context *cxt) {
  mrbc_context_free(mrb, cxt);
}

inline MRuby::MRuby() : mrb_(mrb_open()), cxt_(mrbc_context_new(mrb_.get()), {mrb_.get()}) {
}

inline mrb_value MRuby::load_string(const std::string &str) {
  return mrb_load_string_cxt(mrb_.get(), str.c_str(), cxt_.get());
}

inline mrb_value MRuby::load_file(const std::string &filename) {
  auto closer = [](FILE *fp) {
    std::fclose(fp);
  };
  std::unique_ptr<FILE, decltype(closer)> f(fopen(filename.c_str(), "r"), closer);
  return mrb_load_file_cxt(mrb_.get(), f.get(), cxt_.get());
}

inline std::string MRuby::to_string(mrb_value str) {
  return mrb_str_to_cstr(mrb_.get(), str);
}

template<typename T>
T *MRuby::get_data(mrb_value o) {
  return static_cast<T *>(mrb_data_get_ptr(mrb_.get(), o, &MRClass<T>::data_type));
}

template<typename Signature>
MRFunction<Signature> MRuby::get_function(const std::string &name) {
  return MRFunction<Signature>(mrb_.get(), name);
}

template<typename Signature>
MRFunction<Signature> MRuby::get_function(mrb_value receiver, const std::string &name) {
  return MRFunction<Signature>(mrb_.get(), receiver, name);
}

template<typename ResultType, typename ... Args>
ResultType MRuby::call(const std::string &name, Args ... args) {
  return get_function<ResultType(Args ...)> (name)(args ...);
}

template<typename ResultType, typename ... Args>
ResultType MRuby::call(mrb_value receiver,
    const std::string &name, Args ... args) {
  return get_function<ResultType(Args ...)>(receiver, name) (args ...);
}

template<typename T>
MRClass<T> MRuby::install_class(const std::string &name, RClass *super) {
  return MRClass<T>::create(mrb_.get(), name, super);
}

template<typename T>
mrb_value MRuby::new_instance(const MRClass<T> clazz, std::function<void(T *)> initialize) {
  mrb_value result = load_string(MRClass<T>::class_name + ".new");
  initialize(get_data<T>(result));
  return result;
}

template<typename T>
void MRuby::each_array(mrb_value ary, std::function<void(T)> f) {
  auto size = call<int>(ary, "size");
  for (int i = 0; i < size; i++) {
    f(call<T>(ary, "[]", i));
  }
}

template<typename K, typename V>
void MRuby::each_hash(mrb_value hash, std::function<void(K, V)> f) {
  auto keys = call<mrb_value>(hash, "keys");
  each_array<mrb_value>(keys, [&](mrb_value key) {
        f(to_c_value<K>(key), call<V>(hash, "[]", key));
      });
}

template<typename T>
T MRuby::to_c_value(mrb_value v) {
  return MRType<T>::to_c_value(mrb_.get(), v);
}

template<typename T>
mrb_value MRuby::to_mrb_value(T v) {
  return MRType<T>::to_mrb_value(mrb_.get(), v);
}

template<typename ... Ts>
std::string MRuby::args_format_string() {
  std::string result;
  for (auto ch : {MRType<Ts>::arg_char() ...}) {
    result += ch;
  }
  return result;
}

inline bool MRuby::is_nil(mrb_value v) {
  return mrb_nil_p(v);
}

inline mrb_sym MRuby::sym(const std::string &str) {
  return to_c_value<mrb_sym>(load_string(str));
}

inline bool MRuby::exists_error() {
  return mrb_.get()->exc;
}

inline void MRuby::print_error() {
  mrb_print_error(mrb_.get());
}

inline void MRuby::print_error_if_exists() {
  if (exists_error()) {
    print_error();
  }
}

inline void MRuby::p(mrb_value v) {
  mrb_p(mrb_.get(), v);
  std::fflush(stdout);
}
}  // namespace mrbind
#endif  // INCLUDE_MRBIND_M_RUBY_INL_HPP__

