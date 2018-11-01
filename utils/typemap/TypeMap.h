#ifndef UTILS_TYPEMAP_H_INCLUDED
#define UTILS_TYPEMAP_H_INCLUDED

#include <unordered_map>
#include <functional>
#include <cassert>
#include <cstring>

namespace utils {
    /// Struct for storing basic information about a type used by TypeMap
    struct TypeInfo {
    private:
        static const unsigned char TYPEINFO_POINTER_BIT   = 0b0000'0001;
        static const unsigned char TYPEINFO_SMALL_BIT     = 0b0000'0010;
        static const unsigned char TYPEINFO_REFERENCE_BIT = 0b0000'0100;

        TypeInfo() {}
    public:
        /// This is the only method that should be used for creating a TypeInfo
        ///
        /// The name is subject to change
        template<typename T>
        static const TypeInfo get() {
            TypeInfo ret;
            ret.hash = typeid(T).hash_code();
            ret.flags |= std::is_pointer<T>::value ? TYPEINFO_POINTER_BIT : 0;
            ret.flags |= sizeof(T) <= sizeof(void*) ? TYPEINFO_SMALL_BIT : 0;
            ret.flags |= std::is_reference<T>::value ? TYPEINFO_REFERENCE_BIT : 0;

            ret.destructor = [](void* o) {
                using type = typename std::remove_reference<T>::type;
                reinterpret_cast<type*>(o)->~type();
            };
            return ret;
        }
        bool operator==(const TypeInfo& rhs) const {
            return hash == rhs.hash && rhs.is_reference() == is_reference();
        }
        /// Is this a pointer type
        bool is_pointer() const {
            return flags & TYPEINFO_POINTER_BIT;
        }
        /// Returns false if this type is more bytes than a pointer
        bool is_small() const {
            return flags & TYPEINFO_SMALL_BIT;
        }
        /// Is this a reference type
        bool is_reference() const {
            return flags & TYPEINFO_REFERENCE_BIT;
        }

        /// The type's unique hash code
        std::size_t hash;
        /// A bit array of useful information on this type
        unsigned char flags = 0;
        /// The type's destructor
        std::function<void(void*)> destructor;
    };
}

namespace std {
    template<>
    struct hash<utils::TypeInfo> {
        std::size_t operator()(const utils::TypeInfo info) const {
            return info.hash;
        }
    };
}

namespace utils {
    /// A map from types to values of the given type
    ///
    /// Note: Only one value of a given type may be stored
    class TypeMap {
    public:
        using iterator = std::unordered_map<TypeInfo, void*>::iterator;
        using const_iterator = std::unordered_map<TypeInfo, void*>::const_iterator;
    private:
        void destroy(const TypeInfo& info);

        std::unordered_map<TypeInfo, void*> m_map; 
    public:
        TypeMap() {}
        ~TypeMap() { clear(); }

        template<typename T>
        bool contains() const { return m_map.find(TypeInfo::get<T>()) != m_map.end(); }
        template<typename T>
        void insert(T data);
        /// find is the same as get except a element of type T must already exist in the TypeMap.
        /// If not, it will crash.
        template<typename T>
        typename std::add_lvalue_reference<T>::type find() const;
        /// This version of find returns a nullptr if the key does not already exist
        ///
        /// Properly using this function requires some knowledge of how TypeMap works internally
        void* find(const TypeInfo& info) const;
        /// \note Only call this if T has a (public) constructor taking no arguments
        template<typename T>
        typename std::add_lvalue_reference<T>::type get();
        template<typename T>
        void erase() { erase(TypeInfo::get<T>()); }
        void erase(const TypeInfo& info);

        bool empty() const { return m_map.empty(); }
        bool is_empty() const { return empty(); }
        std::size_t size() const { return m_map.size(); }
    
        void clear();

        iterator begin() { return m_map.begin(); }
        const_iterator cbegin() { return m_map.cbegin(); }
        iterator end() { return m_map.end(); }
        const_iterator cend() { return m_map.cend(); }

    };
    template<typename T>
    void TypeMap::insert(T data) {
        using type = typename std::remove_reference<T>::type;

        erase<T>();
        TypeInfo info = TypeInfo::get<T>();
        // info.is_pointer() => info.is_small()
        if (info.is_reference()) {
            m_map[info] = std::addressof(data);
        } else if (info.is_small()) {
            new(&m_map[info]) type(data);
        } else {
            m_map[info] = new type(data);
        }
    }
    template<typename T>
    typename std::add_lvalue_reference<T>::type TypeMap::find() const {
        using type = typename std::remove_reference<T>::type;

        TypeInfo info = TypeInfo::get<T>();
        if (m_map.find(info) == m_map.end()) assert(false && "Tried getting nonexistent value from a TypeMap");
        
        if (info.is_reference()) {
            return *(type*)m_map.find(info)->second;
        } else if (info.is_small()) {
            return *(type*)&m_map.find(info)->second;
        } else {
            return *(type*)m_map.find(info)->second;
        }
    }
    template<typename T>
    typename std::add_lvalue_reference<T>::type TypeMap::get() {
        if (!contains<T>()) insert<T>(T());
        return find<T>();
    }
}

#endif // UTILS_TYPEMAP_H_INCLUDED
