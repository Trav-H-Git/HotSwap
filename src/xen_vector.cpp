#include "xen_vector.h"
//#include "xen_string.h"
//#include "arena.h"
//#include "memory.h"
//#include <iostream>
//#include <type_traits>
//
//template<typename T>
//Vector<T>::Vector()
//    : data_(nullptr), length_(0), capacity_(0), arena_(Arena(DEFAULT_ARENA_ID)) {
//    // Allocate_(); // Commented out as in original
//}
//
//template<typename T>
//Vector<T>::Vector(ArenaID id)
//    : data_(nullptr), length_(0), capacity_(0), arena_(Arena(id)) {
//    // Allocate_(); // Commented out as in original
//}
//
//template<typename T>
//void Vector<T>::Init(size_t len) {
//    length_ = len;
//    capacity_ = (len < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : len;
//    Allocate_();
//    for (size_t i = 0; i < length_; i++) {
//        new (&data_[i]) T();
//    }
//}
//
//template<typename T>
//void Vector<T>::Init(size_t len, const T& val) {
//    length_ = len;
//    capacity_ = (len < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : len;
//    Allocate_();
//    for (size_t i = 0; i < length_; i++) {
//        new (&data_[i]) T(val);
//    }
//}
//
//template<typename T>
//void Vector<T>::Init(std::initializer_list<T> list) {
//    length_ = list.size();
//    capacity_ = (length_ < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : length_;
//    Allocate_();
//    size_t i = 0;
//    for (const auto& item : list) {
//        new (&data_[i]) T(item);
//        ++i;
//    }
//}
//
//template<typename T>
//Vector<T>::Vector(const Vector& other)
//    : length_(other.length_), capacity_(other.capacity_), arena_(other.arena_) {
//    Allocate_();
//    for (size_t i = 0; i < length_; ++i) {
//        new (&data_[i]) T(other[i]);
//    }
//}
//
//template<typename T>
//Vector<T>::Vector(Vector&& other) noexcept
//    : data_(other.data_), length_(other.length_), capacity_(other.capacity_), arena_(other.arena_) {
//    other.data_ = nullptr;
//    other.length_ = 0;
//    other.capacity_ = 0;
//}
//
//template<typename T>
//Vector<T>& Vector<T>::operator=(const Vector& other) {
//    if (this == &other) return *this;
//    _Delete();
//    length_ = other.length_;
//    capacity_ = other.capacity_;
//    arena_ = other.arena_;
//    Allocate_();
//    for (size_t i = 0; i < length_; ++i) {
//        new (&data_[i]) T(other[i]);
//    }
//    return *this;
//}
//
//template<typename T>
//Vector<T>& Vector<T>::operator=(Vector&& other) noexcept {
//    if (this == &other) return *this;
//    _Delete();
//    data_ = other.data_;
//    length_ = other.length_;
//    capacity_ = other.capacity_;
//    arena_ = other.arena_;
//    other.data_ = nullptr;
//    other.length_ = 0;
//    other.capacity_ = 0;
//    return *this;
//}
//
//template<typename T>
//Vector<T>::~Vector() {
//    _Delete();
//}
//
//template<typename T>
//T& Vector<T>::operator[](size_t idx) {
//    ERROR_IDX_INBOUNDS(idx, length_);
//    return data_[idx];
//}
//
//template<typename T>
//const T& Vector<T>::operator[](size_t idx) const {
//    ERROR_IDX_INBOUNDS(idx, length_);
//    return data_[idx];
//}
//
//template<typename T>
//void Vector<T>::PushBack(const T& val) {
//    if (length_ >= capacity_) {
//        capacity_ = (capacity_ < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : (capacity_ * 2);
//        _Reallocate();
//    }
//    new (&data_[length_]) T(val);
//    ++length_;
//}
//
//template<typename T>
//const T* Vector<T>::GetData() const {
//    return data_;
//}
//
//template<typename T>
//size_t Vector<T>::Length() const {
//    return length_;
//}
//
//template<typename T>
//size_t Vector<T>::Size() const {
//    return length_;
//}
//
//template<typename T>
//bool Vector<T>::IsEmpty() const {
//    return length_ == 0;
//}
//
//template<typename T>
//T& Vector<T>::Back() const {
//    return *(data_ + length_ - 1);
//}
//
//template<typename T>
//void Vector<T>::Clear() {
//    length_ = 0;
//}
//
//template<typename T>
//const String Vector<T>::ToString() const {
//    return String(reinterpret_cast<const char*>(data_), length_);
//}
//
//template<typename T>
//void Vector<T>::Print() {
//    std::cout << this << " vector len: " << length_ << " vector cap: " << capacity_ << " [";
//    for (size_t i = 0; i < length_; i++) {
//        std::cout << data_[i] << ", ";
//    }
//    std::cout << "]\n";
//}
//
//template<typename T>
//void Vector<T>::_Delete() {
//    if constexpr (!std::is_trivially_destructible<T>::value) {
//        for (size_t i = 0; i < length_; ++i) {
//            data_[i].~T();
//        }
//    }
//    FREE_MEM(data_);
//    capacity_ = 0;
//    length_ = 0;
//}
//
//template<typename T>
//void Vector<T>::Allocate_() {
//    data_ = arena_.Alloc<T>();
//}
//
//template<typename T>
//void Vector<T>::_Reallocate() {
//    T* new_data = (T*)CALLOC_MEM(capacity_ * sizeof(T));
//    for (size_t i = 0; i < length_; ++i) {
//        new (&new_data[i]) T(data_[i]);
//    }
//    if constexpr (!std::is_trivially_destructible<T>::value) {
//        for (size_t i = 0; i < length_; ++i) {
//            data_[i].~T();
//        }
//    }
//    FREE_MEM(data_);
//    data_ = new_data;
//}
