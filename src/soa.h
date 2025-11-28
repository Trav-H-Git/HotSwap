#include <vector>
#include <cassert>
#include <iostream>

// Helper macros for field iteration and generation
#define SOA_GENERATE_FIELD_VECTOR(Type, Name) \
    std::vector<Type> Name##_;

#define SOA_GENERATE_PROXY_ACCESSOR(Type, Name) \
    Type& Name() { return soa_.Name##_[index_]; } \
    const Type& Name() const { return soa_.Name##_[index_]; }

#define SOA_GENERATE_PUSH(Type, Name, StructVar) \
    Name##_.push_back(StructVar.Name);

#define SOA_GENERATE_PROXY_ASSIGN(Type, Name, StructVar) \
    Name() = StructVar.Name;

#define SOA_GENERATE_PROXY_TO_STRUCT(Type, Name, StructVar) \
    StructVar.Name = Name();

#define SOA_GENERATE_RESERVE(Type, Name) \
    Name##_.reserve(n);

// Macro to iterate over field list (type-name pairs)
#define SOA_FOR_EACH_FIELD(Macro, ...) \
    SOA_EXPAND_FIELDS(Macro, __VA_ARGS__)

#define SOA_EXPAND_FIELDS(Macro, Type1, Name1, ...) \
    Macro(Type1, Name1, ##__VA_ARGS__) \
    SOA_EXPAND_FIELDS_REST(Macro, ##__VA_ARGS__)

#define SOA_EXPAND_FIELDS_REST(Macro, Type2, Name2, ...) \
    Macro(Type2, Name2, ##__VA_ARGS__) \
    SOA_EXPAND_FIELDS_REST2(Macro, ##__VA_ARGS__)

#define SOA_EXPAND_FIELDS_REST2(Macro, ...) \
    /* Extend for more fields if needed */

// Macro to define SOA and POD struct
#define SOA_DEFINE(StructName, ...) \
    struct StructName { \
        __VA_ARGS__; \
    }; \
    template <> \
    class SOA<StructName> { \
    public: \
        /* Proxy class to emulate StructName& */ \
        class Proxy { \
        public: \
            Proxy(SOA<StructName>& soa, size_t index) : soa_(soa), index_(index) {} \
            /* Generate accessors for each field */ \
            SOA_FOR_EACH_FIELD(SOA_GENERATE_PROXY_ACCESSOR, __VA_ARGS__) \
            /* Assignment from StructName */ \
            Proxy& operator=(const StructName& s) { \
                SOA_FOR_EACH_FIELD(SOA_GENERATE_PROXY_ASSIGN, __VA_ARGS__, s) \
                return *this; \
            } \
            /* Convert to StructName */ \
            operator StructName() const { \
                StructName s; \
                SOA_FOR_EACH_FIELD(SOA_GENERATE_PROXY_TO_STRUCT, __VA_ARGS__, s) \
                return s; \
            } \
        private: \
            SOA<StructName>& soa_; \
            size_t index_; \
        }; \
        /* Constructor */ \
        SOA() = default; \
        explicit SOA(const StructName& s) { push(s); } \
        /* Push a StructName */ \
        void push(const StructName& s) { \
            SOA_FOR_EACH_FIELD(SOA_GENERATE_PUSH, __VA_ARGS__, s) \
        } \
        /* Access element at index i */ \
        Proxy operator[](size_t i) { \
            assert(i < size()); \
            return Proxy(*this, i); \
        } \
        const Proxy operator[](size_t i) const { \
            assert(i < size()); \
            return Proxy(const_cast<SOA<StructName>&>(*this), i); \
        } \
        /* Size */ \
        size_t size() const { \
            return x_.size(); /* Use first field's vector for size */ \
        } \
        /* Reserve */ \
        void reserve(size_t n) { \
            SOA_FOR_EACH_FIELD(SOA_GENERATE_RESERVE, __VA_ARGS__) \
        } \
        /* Direct access to field vectors for iteration */ \
        SOA_FOR_EACH_FIELD(SOA_GENERATE_FIELD_VECTOR, __VA_ARGS__) \
    private: \
        /* First field vector for size reference */ \
        std::vector<int> x_; /* Dummy if first field isn't int; replaced by first field */ \
    };

// Base SOA template
template <typename T>
class SOA;

// Define SOA for Bar with fields: int a, float b
//SOA_DEFINE(Bar, int, a, float, b)

// int main() {
//     // Create SOA with an initial Bar
//     Bar bar{.a = 1, .b = 2.0f};
//     SOA<Bar> soa(bar);

//     // Push another Bar using designated initializers (C++20)
//     soa.push({.a = 3, .b = 4.0f});

//     // Access and modify elements
//     soa[0].a() = 10;   // Modify a of first element
//     soa[1].b() = 20.0f; // Modify b of second element

//     // Read back as Bar
//     Bar first = soa[0];
//     Bar second = soa[1];
//     std::cout << "First: a=" << first.a << ", b=" << first.b << "\n";
//     std::cout << "Second: a=" << second.a << ", b=" << second.b << "\n";

//     // Assign a Bar to an element
//     soa[1] = {.a = 7, .b = 8.0f};
//     second = soa[1];
//     std::cout << "Updated second: a=" << second.a << ", b=" << second.b << "\n";

//     // Iterate over all 'a' fields (fast, cache-friendly)
//     std::cout << "All 'a' values: ";
//     for (const auto& a : soa.a_) {
//         std::cout << a << " ";
//     }
//     std::cout << "\n";

//     // Iterate over all 'b' fields
//     std::cout << "All 'b' values: ";
//     for (const auto& b : soa.b_) {
//         std::cout << b << " ";
//     }
//     std::cout << "\n";

//     return 0;
// }