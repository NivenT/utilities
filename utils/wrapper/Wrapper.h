#ifndef UTILS_WRAPPER_H_INCLUDED
#define UTILS_WRAPPER_H_INCLUDED

#include <type_traits>

#define UTILS_OP_CHECKER(name, op) \
  template<typename T, typename U> Nop operator op(const T&, const U&); \
  template<typename T, typename U=T> \
  struct name##Exists { \
    enum { value = !::std::is_same<decltype(*(T*)(0) op *(U*)(0)), Nop>::value }; \
  };

#define UTILS_UNARY_OP_CHECKER(name, op) \
  template<typename T> Nop operator op(T&); \
  template<typename T> \
  struct name##Exists { \
    enum { value = !::std::is_same<decltype(op *(T*)(0)), Nop>::value }; \
  };

#define UTILS_IMPL_WRAPPER_OP(wrapper, type, name, op)	\
  template<typename T> \
  auto operator op(const T& rhs) const -> decltype(::std::enable_if_t<::utils::check:: name##Exists<type, T>::value, wrapper>{}) { \
    return m_data op rhs;		\
  } \
  template<typename T> \
  auto operator op##=(const T& rhs) -> decltype(::std::enable_if_t<::utils::check:: name##Exists<type, T>::value, wrapper>{}) { \
    m_data = m_data op rhs;   \
    return *this; \
  } \

#define UTILS_IMPL_WRAPPER_OP2(wrapper, type, name, op, rhs_type)  \
  template<typename T, typename U, typename V>              \
  static auto __##name(const U& lhs, const V& rhs) -> decltype(::std::enable_if_t<::utils::check:: name##Exists<T, rhs_type>::value, wrapper>{}) { \
    return lhs.m_data op rhs;          \
  } \
  template<typename T, typename U, typename V>              \
  static auto __##name(const U& lhs, const V& rhs) -> decltype(::std::enable_if_t<!::utils::check:: name##Exists<T, rhs_type>::value, wrapper>{}) { \
    return wrapper();             \
  } \
  wrapper operator op(const rhs_type& rhs) const { \
    return __##name<type, wrapper, rhs_type>(*this, rhs);   \
  }             \
  wrapper& operator op##=(const rhs_type& rhs) {   \
    *this = *this op rhs;       \
    return *this;         \
  }

#define UTILS_IMPL_WRAPPER_COMP(wrapper, type, name, op)  \
  template<typename T, typename U>              \
  static auto __##name(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<::utils::check:: name##Exists<T>::value, bool>{}) { \
    return lhs.m_data op rhs.m_data;          \
  } \
  template<typename T, typename U>              \
  static auto __##name(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<!::utils::check:: name##Exists<T>::value, bool>{}) { \
    return wrapper();             \
  } \
  bool operator op(const wrapper& rhs) const { \
    return __##name<type, wrapper>(*this, rhs);   \
  }             \
  bool operator op##=(const wrapper& rhs) {   \
    return *this op rhs || *this == rhs; \
  }

// c should be "const" or "" (without the quotations)
#define UTILS_IMPL_WRAPPER_UNARY_OP(wrapper, type, name, op, c)		\
  template<typename T, typename U>					\
  static auto __##name(c U& lhs) -> decltype(::std::enable_if_t<::utils::check:: name##Exists<T>::value, wrapper>{}) { \
    return op lhs.m_data;						\
  }									\
  template<typename T, typename U>					\
  static auto __##name(c U& lhs) -> decltype(::std::enable_if_t<!::utils::check:: name##Exists<T>::value, wrapper>{}) { \
    return wrapper();							\
  }									\
  wrapper operator op() c {						\
    return __##name<type, wrapper>(*this);				\
  }									\

// I can't tell if this code is good or trash
#define UTILS_CREATE_WRAPPER(name, type) \
  class name { \
  public: \
    typedef type inner; \
  private:     \
    type m_data;				\
  public:    \
    name(type data = type()) : m_data(data) {}	\
    void __utils_wrapper() {} \
    type to_inner() const { return m_data; } \
    template<typename T, typename U>              \
    static auto __Equals(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<::utils::check::EqualsExists<T>::value, bool>{}) { \
      return lhs.m_data == rhs.m_data;          \
    } \
    template<typename T, typename U>              \
    static auto __Equals(const U& lhs, const U& rhs) -> decltype(::std::enable_if_t<!::utils::check::EqualsExists<T>::value, bool>{}) { \
      return false;            \
    } \
    bool operator==(const name& rhs) const { \
      return __Equals<type, name>(*this, rhs);   \
    } \
    bool operator!=(const name& rhs) const { return !operator==(rhs); } \
    UTILS_IMPL_WRAPPER_OP(name, type, Add, +)		\
    UTILS_IMPL_WRAPPER_OP(name, type, Sub, -)		\
    UTILS_IMPL_WRAPPER_UNARY_OP(name, type, Neg, -, const)	\
    UTILS_IMPL_WRAPPER_OP(name, type, Div, /)		\
    UTILS_IMPL_WRAPPER_OP(name, type, Mul, *)		\
    UTILS_IMPL_WRAPPER_OP(name, type, And, &)		\
    UTILS_IMPL_WRAPPER_OP(name, type, Or, |)		\
    UTILS_IMPL_WRAPPER_UNARY_OP(name, type, Not, ~, const)	\
    UTILS_IMPL_WRAPPER_UNARY_OP(name, type, Incr, ++,)		\
    name operator++(int) { auto ret = *this; operator++(); return ret; } \
    UTILS_IMPL_WRAPPER_UNARY_OP(name, type, Decr, --,)		\
    name operator--(int) { auto ret = *this; operator--(); return ret; } \
    UTILS_IMPL_WRAPPER_OP2(name, type, LShift, <<, int) \
    UTILS_IMPL_WRAPPER_OP2(name, type, RShift, >>, int) \
    UTILS_IMPL_WRAPPER_OP(name, type, Mod, %) \
    UTILS_IMPL_WRAPPER_COMP(name, type, Greater, >) \
    UTILS_IMPL_WRAPPER_COMP(name, type, Lesser, <) \
    template<typename T, typename U> \
    static auto __Print(::std::ostream& lhs, const U& rhs) -> decltype(::std::enable_if_t<::utils::check::LShiftExists<::std::ostream, T>::value, ::std::ostream*>{}) { \
      lhs<<#name<<"("<<rhs.m_data<<")";          \
      return &lhs; \
    } \
    template<typename T, typename U>              \
    static auto __Print(::std::ostream& lhs, const U& rhs) -> decltype(::std::enable_if_t<!::utils::check::LShiftExists<::std::ostream, T>::value, ::std::ostream*>{}) { \
      lhs<<#name;            \
      return &lhs; \
    } \
  };						\
  inline ::std::ostream& operator<<(::std::ostream& lhs, const name& rhs) { \
    return *name::__Print<type, name>(lhs, rhs);   \
  }
// The above function is inline to avoid the one-definition rule

/// Assumes type supports ::std::hash
///
/// Only use macro inside namespace std
#define UTILS_HASH_WRAPPER(name) \
  template<> \
  struct hash<name> { \
    ::std::size_t operator()(const name& n) const { \
      return hash<name::inner>()(n.to_inner()); \
    } \
  };

/// \todo Make all indeutilstion 2 spaces
namespace utils {
  /// Namespace for checking whether types support certain operations
  namespace check {
    struct Nop {};
    /// Struct for checking (at compile time) whether or not a class is a wrapper
    ///
    /// Usage: assert(is_wrapper<EntityID>::value == 1);
    template<typename T>
    struct is_wrapper { 
    private:
      typedef char one;
      struct two { char _[2]; };

      template<typename C> static one test(decltype(&C::__utils_wrapper));
      template<typename C> static two test(...);
    public:
      enum { value = sizeof(test<T>(0)) == sizeof(one) };
    };
    UTILS_OP_CHECKER(Equals, ==)
    UTILS_OP_CHECKER(Add, +)
    UTILS_OP_CHECKER(Sub, -)
    UTILS_UNARY_OP_CHECKER(Neg, -)
    UTILS_OP_CHECKER(Div, /)
    UTILS_OP_CHECKER(Mul, *)
    UTILS_OP_CHECKER(And, &)
    UTILS_OP_CHECKER(Or, |)
    UTILS_UNARY_OP_CHECKER(Not, ~)
    UTILS_UNARY_OP_CHECKER(Incr, ++)
    UTILS_UNARY_OP_CHECKER(Decr, --) 
    UTILS_OP_CHECKER(LShift, <<)
    UTILS_OP_CHECKER(RShift, >>)
    UTILS_OP_CHECKER(Mod, %)
    UTILS_OP_CHECKER(Greater, >)
    UTILS_OP_CHECKER(Lesser, <)
  }
}

#endif // UTILS_WRAPPER_H_INCLUDED
