#ifndef INCLUDE_MRBIND_MR_CLASS_HPP__
#define INCLUDE_MRBIND_MR_CLASS_HPP__
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>

#include <string>

namespace mrbind {
template<typename T>
class MRClass {
  mrb_state *state_;

  public:
    static RClass *rclass;
    static mrb_data_type data_type;
    static std::string class_name;

    static MRClass create(mrb_state *state, const std::string &name, RClass *super);

    template<typename R, typename ... Args>
    struct MethodDefiner {
      MRClass *clazz;
      template<R Fn(T *, Args ...)>
      void from(const std::string &name);
    };

    struct Definer {
      MRClass *clazz;

      template<typename ... Ts>
      void initialize();

      template<typename ... Ts>
      MethodDefiner<Ts ...> method();
    };

    Definer define();

  private:
    explicit MRClass(mrb_state *state)
      : state_(state) {
    }

    static void free_instance(mrb_state *, void *ptr) {
      delete static_cast<T *>(ptr);
    }
};

template<typename T> RClass * MRClass<T>::rclass;
template<typename T> mrb_data_type MRClass<T>::data_type;
template<typename T> std::string MRClass<T>::class_name;
}  // namespace mrbind
#endif  // INCLUDE_MRBIND_MR_CLASS_HPP__

