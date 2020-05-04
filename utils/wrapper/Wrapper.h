#ifndef UTILS_WRAPPER_H_INCLUDED
#define UTILS_WRAPPER_H_INCLUDED

#include <type_traits>

#define UTILS_OP_CHECKER(name, op) \
    template<typename T, typename U> Nop operator op(const T&, const U&); \
    template<typename T, typename U=T> \
    struct name##Exists { \
        enum { value = !::std::is_same<decltype(std::declval<T>() op std::declval<U>()), Nop>::value }; \
    };

#define UTILS_UNARY_OP_CHECKER(name, op) \
    template<typename T> Nop operator op(T&); \
    template<typename T> \
    struct name##Exists { \
        enum { value = !::std::is_same<decltype(op *std::declval<T*>()), Nop>::value }; \
    };

#define UTILS_IMPL_WRAPPER_OP(name, op)  \
    template<typename U, typename std::enable_if_t<check:: name##Exists<T, U>::value>* = nullptr> \
    Wrapper operator op(const U& rhs) const { \
        return Wrapper(m_data op rhs);       \
    } \
    template<typename U, typename std::enable_if_t<check:: name##Exists<T, U>::value>* = nullptr> \
    Wrapper& operator op##=(const U& rhs) { \
        m_data = m_data op rhs; \
        return *this; \
    }   

#define UTILS_IMPL_WRAPPER_SELF_OP(name, op) \
    template<typename T, typename TAG, typename std::enable_if_t<check:: name##Exists<T>::value>* = nullptr> \
    Wrapper<T, TAG> operator op(const Wrapper<T, TAG>& lhs, const Wrapper<T, TAG>& rhs) { \
        return Wrapper<T, TAG>(lhs.to_inner() op rhs.to_inner()); \
    } \
    template<typename T, typename TAG, typename std::enable_if_t<check:: name##Exists<T>::value>* = nullptr> \
    Wrapper<T, TAG>& operator op##=(Wrapper<T, TAG>& lhs, const Wrapper<T, TAG>& rhs) { \
        lhs.to_inner() = lhs.to_inner() op rhs.to_inner(); \
        return Wrapper<T, TAG>(lhs); \
    }

#define UTILS_IMPL_WRAPPER_UNARY_OP(name, op, c) \
    template<typename T, typename TAG, typename std::enable_if_t<check:: name##Exists<T>::value>* = nullptr> \
    Wrapper<T, TAG> operator op(c Wrapper<T, TAG>& w) { \
        return Wrapper<T, TAG>(op w.to_inner()); \
    }

#define UTILS_IMPL_WRAPPER_COMP(name, op) \
    template<typename U, typename std::enable_if_t<check:: name##Exists<T, U>::value>* = nullptr> \
    bool operator op(const U& rhs) const { \
        return m_data op rhs; \
    } \
    template<typename U, typename std::enable_if_t<check:: name##Exists<T, U>::value && check::EqualsExists<T, U>::value>* = nullptr> \
    bool operator op##=(const U& rhs) const { \
        return m_data op rhs || m_data == rhs; \
    }

#define UTILS_IMPL_WRAPPER_SELF_COMP(name, op) \
    template<typename T, typename TAG, typename std::enable_if_t<check:: name##Exists<T>::value>* = nullptr> \
    bool operator op(const Wrapper<T, TAG>& lhs, const Wrapper<T, TAG>& rhs) { \
        return lhs.to_inner() op rhs.to_inner(); \
    } \
    template<typename T, typename TAG, typename std::enable_if_t<check:: name##Exists<T>::value && check::EqualsExists<T>::value>* = nullptr> \
    bool operator op##=(const Wrapper<T, TAG>& lhs, const Wrapper<T, TAG>& rhs) { \
        return lhs.to_inner() op rhs.to_inner() || lhs.to_inner() == rhs.to_inner(); \
    }

namespace utils {
    /// Namespace for checking whether types support certain operations
    namespace check {
        struct Nop {};
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
    /// A generic class for creating wrappers around (non-reference) types
    ///
    /// Automatically inherts all operations available to the inner type T.
    /// This includes the ability to print it using e.g. cout<<
    ///
    /// usage: using Name = utils::Wrapper<std::string, struct NameTag>
    template<typename T, typename TAG>
    class Wrapper {
    public:
        typedef T inner;
    private:
        T m_data;
    public:
        Wrapper(T data = T()) : m_data(data) {}
        const T& to_inner() const { return m_data; }
        T& to_inner() { return m_data; }
        explicit operator T() const { return to_inner(); }
        template<typename U>
        auto operator==(const U& rhs) const -> decltype(std::enable_if_t<check::EqualsExists<T, U>::value, bool>{}) {
            return m_data == rhs;
        }
        template<typename std::enable_if_t<check::EqualsExists<T>::value>* = nullptr>
        bool operator==(const Wrapper& rhs) const {
            return m_data == rhs.m_data;
        }
        template<typename U>
        auto operator!=(const U& rhs) const -> decltype(std::enable_if_t<check::EqualsExists<T, U>::value, bool>{}) {
            return m_data != rhs;
        }
        template<typename std::enable_if_t<check::EqualsExists<T>::value>* = nullptr>
        bool operator!=(const Wrapper& rhs) const {
            return !(m_data == rhs.m_data);
        }
        UTILS_IMPL_WRAPPER_OP(Add, +)
        UTILS_IMPL_WRAPPER_OP(Sub, -)
        UTILS_IMPL_WRAPPER_OP(Div, /)
        UTILS_IMPL_WRAPPER_OP(Mul, *)
        UTILS_IMPL_WRAPPER_OP(And, &)
        UTILS_IMPL_WRAPPER_OP(Or, |)
        UTILS_IMPL_WRAPPER_OP(LShift, <<)
        UTILS_IMPL_WRAPPER_OP(RShift, >>)
        UTILS_IMPL_WRAPPER_OP(Mod, %)
        UTILS_IMPL_WRAPPER_COMP(Greater, >)
        UTILS_IMPL_WRAPPER_COMP(Lesser, <)
    };
    UTILS_IMPL_WRAPPER_SELF_OP(Add, +)
    UTILS_IMPL_WRAPPER_SELF_OP(Sub, -)
    UTILS_IMPL_WRAPPER_SELF_OP(Div, /)
    UTILS_IMPL_WRAPPER_SELF_OP(Mul, *)
    UTILS_IMPL_WRAPPER_SELF_OP(And, &)
    UTILS_IMPL_WRAPPER_SELF_OP(Or, |)
    UTILS_IMPL_WRAPPER_SELF_OP(LShift, <<)
    UTILS_IMPL_WRAPPER_SELF_OP(RShift, >>)
    UTILS_IMPL_WRAPPER_SELF_OP(Mod, %)
    UTILS_IMPL_WRAPPER_SELF_COMP(Greater, >)
    UTILS_IMPL_WRAPPER_SELF_COMP(Lesser, <)
    UTILS_IMPL_WRAPPER_UNARY_OP(Neg, -, const)
    UTILS_IMPL_WRAPPER_UNARY_OP(Not, ~, const)
    UTILS_IMPL_WRAPPER_UNARY_OP(Incr, ++,) // pre-incrementation
    template<typename T, typename TAG, typename std::enable_if_t<check::IncrExists<T>::value>* = nullptr>
    Wrapper<T, TAG> operator++(Wrapper<T, TAG>& lhs, int) {
        auto ret = lhs;
        ++lhs;
        return ret;
    }
    UTILS_IMPL_WRAPPER_UNARY_OP(Decr, --,) // pre-decrementation
    template<typename T, typename TAG, typename std::enable_if_t<check::DecrExists<T>::value>* = nullptr>
    Wrapper<T, TAG> operator--(Wrapper<T, TAG>& lhs, int) {
        auto ret = lhs;
        --lhs;
        return ret;
    }
    template<typename T, typename TAG, typename std::enable_if_t<check::LShiftExists<std::ostream, T>::value>* = nullptr>
    std::ostream& operator<<(std::ostream& lhs, const Wrapper<T, TAG>& w) {
        return lhs<<w.to_inner();
    }

    /// A generic class for creating wrappers around (non-reference) types
    ///
    /// Automatically inherts all operations available to the inner type T.
    /// This includes the ability to print it using e.g. cout<<
    ///
    /// Must be explicitly constructed.
    ///
    /// usage: using Name = utils::StrongWrapper<std::string, struct NameTag>
    template<typename T, typename TAG>
    class StrongWrapper : public Wrapper<T, TAG> {
    public:
         explicit StrongWrapper(T data) : Wrapper<T, TAG>(data) {}
    };
}

namespace std {
    /// \todo SFINAE or if constexpr or whatever
    template<typename T, typename TAG>
    struct hash<utils::Wrapper<T, TAG>> {
        std::size_t operator()(const utils::Wrapper<T, TAG>& w) const {
            return std::hash<T>()(w.to_inner());
        }
    };
    template<typename T, typename TAG>
    struct hash<utils::StrongWrapper<T, TAG>> {
        std::size_t operator()(const utils::StrongWrapper<T, TAG>& w) const {
            return std::hash<T>()(w.to_inner());
        }
    };
}

#endif // UTILS_WRAPPER_H_INCLUDED
