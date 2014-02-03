#ifndef INCLUDE_MRBIND_MR_CLASS_INL_HPP__
#define INCLUDE_MRBIND_MR_CLASS_INL_HPP__
#include <string>

namespace mrbind {
template<typename T>
MRClass<T> MRClass<T>::create(mrb_state *state, const std::string &name, RClass *super) {
  if (!super) {
    super = state->object_class;
  }
  rclass = mrb_define_class(state, name.c_str(), super);
  MRB_SET_INSTANCE_TT(rclass, MRB_TT_DATA);
  class_name = name;
  data_type = { name.c_str(), free_instance };
  return MRClass<T>(state);
}

template<typename T>
typename MRClass<T>::Definer MRClass<T>::define() {
  return MRClass<T>::Definer({this});
}

template<typename T>
template<typename ... Args>
void MRClass<T>::Definer::initialize() {
  mrb_define_method(clazz->state_, MRClass<T>::rclass, "initialize",
      [](mrb_state *state, mrb_value self) -> mrb_value {
        // コンストラクタ引数の取得
        auto args = MRClassDefineHelper<Args ...>::get_args(state);

        // RData型とインスタンスの生成
        DATA_TYPE(self) = &MRClass<T>::data_type;
        DATA_PTR(self) = MRClassDefineHelper<Args ...>::template new_instance<T>(args);
        return self;
      }, (sizeof ... (Args)) ? ARGS_REQ(sizeof ... (Args)) : ARGS_NONE());
}

template<typename T>
template<typename ... Ts>
MRClass<T>::MethodDefiner<Ts ...> MRClass<T>::Definer::method() {
  return MRClass<T>::MethodDefiner<Ts ...>({clazz});
}

template<typename T>
template<typename R, typename ... Args>
template<R Fn(T *, Args ...)>
void MRClass<T>::MethodDefiner<R, Args ...>::from(const std::string & name) {
  mrb_define_method(clazz->state_, MRClass<T>::rclass, name.c_str(),
      [](mrb_state *state, mrb_value self) -> mrb_value {
        // receiverオブジェクトとメソッド引数の取得
        T *p = static_cast<T *>(mrb_get_datatype(state, self, &MRClass<T>::data_type));
        auto args = MRClassDefineHelper<Args ...>::get_args(state);

        // メソッドを実行
        auto result = MRClassDefineHelper<Args ...>::template call_method<T, R, Fn>(p, args);
        return MRType<R>::to_mrb_value(state, result);
      }, (sizeof ... (Args)) ? ARGS_REQ(sizeof ... (Args)) : ARGS_NONE());
}
}  // namespace mrbind
#endif  // INCLUDE_MRBIND_MR_CLASS_INL_HPP__

