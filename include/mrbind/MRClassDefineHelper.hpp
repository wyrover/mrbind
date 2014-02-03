#ifndef INCLUDE_MRBIND_MR_CLASS_DEFINE_HELPER_HPP__
#define INCLUDE_MRBIND_MR_CLASS_DEFINE_HELPER_HPP__
#include <mruby.h>

namespace mrbind {
/*!
 * クラス定義の補助ツール. 引数の数に応じて実装を変更する.
 *
 * - get_args() : 引数の取得
 * - new_instance() : 引数を使用してインスタンスの生成
 * - call_method() : 引数を使用してメソッドの実行
 */
template<typename ... Ts>
struct MRClassDefineHelper;

// 0引数
template<>
struct MRClassDefineHelper<> {
  static std::tuple<> get_args(mrb_state *state) {
    return std::make_tuple();
  }

  template<typename T>
  static T *new_instance(const std::tuple<> &args) {
    return new T();
  }

  template<typename T, typename R, R Fn(T *)>
  static R call_method(T *first, const std::tuple<> &args) {
    return Fn(first);
  }
};

// 1引数
template<typename Arg1>
struct MRClassDefineHelper<Arg1> {
  typedef typename MRType<Arg1>::argument_type MRArg1;

  static std::tuple<MRArg1> get_args(mrb_state *state) {
    MRArg1 arg1;

    mrb_get_args(state,
        MRuby::args_format_string<Arg1>().c_str(),
        &arg1);
    return std::make_tuple(arg1);
  }

  template<typename T>
  static T *new_instance(const std::tuple<MRArg1> &args) {
    return new T(
      MRType<Arg1>::expand_argument(std::get<0>(args)));
  }

  template<typename T, typename R, R Fn(T *, Arg1)>
  static R call_method(T *first, const std::tuple<MRArg1> &args) {
    return Fn(first,
               MRType<Arg1>::expand_argument(std::get<0>(args)));
  }
};

// 2引数
template<typename Arg1, typename Arg2>
struct MRClassDefineHelper<Arg1, Arg2> {
  typedef typename MRType<Arg1>::argument_type MRArg1;
  typedef typename MRType<Arg2>::argument_type MRArg2;

  static std::tuple<MRArg1, MRArg2> get_args(mrb_state *state) {
    MRArg1 arg1;
    MRArg2 arg2;

    mrb_get_args(state,
        MRuby::args_format_string<Arg1, Arg2>().c_str(),
        &arg1,
        &arg2);
    return std::make_tuple(arg1, arg2);
  }

  template<typename T>
  static T *new_instance(const std::tuple<MRArg1, MRArg2> &args) {
    return new T(
      MRType<Arg1>::expand_argument(std::get<0>(args)),
      MRType<Arg2>::expand_argument(std::get<1>(args)));
  }

  template<typename T, typename R, R Fn(T *, Arg1, Arg2)>
  static R call_method(T *first, const std::tuple<MRArg1, MRArg2> &args) {
    return Fn(first,
               MRType<Arg1>::expand_argument(std::get<0>(args)),
               MRType<Arg2>::expand_argument(std::get<1>(args)));
  }
};

// 3引数
template<typename Arg1, typename Arg2, typename Arg3>
struct MRClassDefineHelper<Arg1, Arg2, Arg3> {
  typedef typename MRType<Arg1>::argument_type MRArg1;
  typedef typename MRType<Arg2>::argument_type MRArg2;
  typedef typename MRType<Arg3>::argument_type MRArg3;

  static std::tuple<MRArg1, MRArg2, MRArg3> get_args(mrb_state *state) {
    MRArg1 arg1;
    MRArg2 arg2;
    MRArg3 arg3;

    mrb_get_args(state,
        MRuby::args_format_string<Arg1, Arg2, Arg3>().c_str(),
        &arg1,
        &arg2,
        &arg3);
    return std::make_tuple(arg1, arg2, arg3);
  }

  template<typename T>
  static T *new_instance(const std::tuple<MRArg1, MRArg2, MRArg3> &args) {
    return new T(
      MRType<Arg1>::expand_argument(std::get<0>(args)),
      MRType<Arg2>::expand_argument(std::get<1>(args)),
      MRType<Arg3>::expand_argument(std::get<2>(args)));
  }

  template<typename T, typename R, R Fn(T *, Arg1, Arg2, Arg3)>
  static R call_method(T *first, const std::tuple<MRArg1, MRArg2, MRArg3> &args) {
    return Fn(first,
               MRType<Arg1>::expand_argument(std::get<0>(args)),
               MRType<Arg2>::expand_argument(std::get<1>(args)),
               MRType<Arg3>::expand_argument(std::get<2>(args)));
  }
};
}  // namespace mrbind
#endif  // INCLUDE_MRBIND_MR_CLASS_DEFINE_HELPER_HPP__

