#ifndef INCLUDE_MRBIND_MR_FUNCTION_HPP__
#define INCLUDE_MRBIND_MR_FUNCTION_HPP__
#include <mruby.h>

#include <string>
#include <functional>

namespace mrbind {
template<typename _Signature>
class MRFunction;

template<typename R, typename ... Args>
class MRFunction<R(Args ...)> {
  typedef std::function<R(Args ...)> function;
  mrb_state *mrb_;
  mrb_value receiver_;
  std::string name_;

  public:
    typedef R result_type;

    MRFunction(mrb_state *mrb, mrb_value receiver, const std::string &name)
      : mrb_(mrb), receiver_(receiver), name_(name) {
    }

    MRFunction(mrb_state *mrb, const std::string &name)
      : mrb_(mrb), receiver_(mrb_top_self(mrb)), name_(name) {
    }

    result_type operator()(Args ... args) {
      auto result = mrb_funcall(mrb_, receiver_, name_.c_str(), sizeof ... (Args),
              MRType<Args>::to_mrb_value(mrb_, args) ...);

      return MRType<result_type>::to_c_value(mrb_, result);
    }
};
}  // namespace mrbind
#endif  // INCLUDE_MRBIND_MR_FUNCTION_HPP__

