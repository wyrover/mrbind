#ifndef INCLUDE_MRBIND_MR_TYPE_INL_HPP__
#define INCLUDE_MRBIND_MR_TYPE_INL_HPP__
namespace mrbind { template<typename T>
T *MRType<T *>::to_c_value(mrb_state *state, mrb_value v) {
  return static_cast<T *>(mrb_get_datatype(state, v, &MRClass<T>::data_type));
}

template<typename T>
mrb_value MRType<T *>::to_mrb_value(mrb_state *state, T *v) {
  auto obj = mrb_obj_alloc(state, MRB_TT_DATA, MRClass<T>::rclass);
  DATA_PTR(obj) = v;
  return obj;
}

template<typename T>
const T *MRType<const T *>::to_c_value(mrb_state *state, mrb_value v) {
  return static_cast<const T *>(mrb_get_datatype(state, v, &MRClass<T>::data_type));
}

template<typename T>
mrb_value MRType<const T *>::to_mrb_value(mrb_state *state, const T *v) {
  auto obj = mrb_obj_alloc(state, MRB_TT_DATA, MRClass<T>::rclass);
  DATA_PTR(obj) = v;
  return obj;
}
}  // namespace mrbind

#endif  // INCLUDE_MRBIND_MR_TYPE_INL_HPP__

