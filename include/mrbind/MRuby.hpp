#ifndef INCLUDE_MRBIND_M_RUBY_HPP__
#define INCLUDE_MRBIND_M_RUBY_HPP__
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>

#include <string>
#include <memory>

namespace mrbind {
class MRuby {
  struct StateCloser {
    void operator()(mrb_state *mrb);
  };
  struct ContextCloser {
    mrb_state *mrb;
    void operator()(mrbc_context *cxt);
  };

  std::unique_ptr<mrb_state, StateCloser> mrb_;
  std::unique_ptr<mrbc_context, ContextCloser> cxt_;

  public:
    MRuby();

    mrb_value load_string(const std::string &str);
    mrb_value load_file(const std::string &filename);

    std::string to_string(mrb_value str);

    template<typename T>
    T *get_data(mrb_value o);

    template<typename Signature>
    MRFunction<Signature> get_function(const std::string &name);

    template<typename Signature>
    MRFunction<Signature> get_function(mrb_value receiver, const std::string &name);

    template<typename ResultType = mrb_value, typename ... Args>
    ResultType call(const std::string &name, Args ... args);

    template<typename ResultType = mrb_value, typename ... Args>
    ResultType call(mrb_value receiver, const std::string &name, Args ... args);

    template<typename T>
    MRClass<T> install_class(const std::string &name, RClass *super = nullptr);

    template<typename T>
    mrb_value new_instance(const MRClass<T> clazz, std::function<void(T *)> initialize);

    template<typename T = mrb_value>
    void each_array(mrb_value ary, std::function<void(T)> f);

    template<typename K = mrb_value, typename V = mrb_value>
    void each_hash(mrb_value hash, std::function<void(K, V)> f);

    template<typename T>
    T to_c_value(mrb_value v);

    template<typename T>
    mrb_value to_mrb_value(T v);

    template<typename ... Ts>
    static std::string args_format_string();

    bool is_nil(mrb_value v);

    mrb_sym sym(const std::string &str);

    bool exists_error();
    void print_error();
    void print_error_if_exists();
    void p(mrb_value v);
};
}  // namespace mrbind

#endif  // INCLUDE_MRBIND_M_RUBY_HPP__

