#pragma once
//#include "mem.h"
#include "memory.h"
#include "xen_assert.h"
#include "xen_primitives.h"
#include "arena.h"
#include "xen_string.h"
#include "util.h"
#include <iostream>
#include <utility>

//#include <iterator> // Added for iterator traits
//#include <type_traits>




//typedef uint32_t ArenaID;
template<typename T>
class Vector
{
public:
    static constexpr size_t DEFAULT_CAPACITY = 4;
    // static constexpr ArenaID DEFAULT_ARENA_ID = Arena::Frame;

    /*Vector()
        : length_(0), capacity_(DEFAULT_CAPACITY), class_alloc_(0)
    {

    }*/

    // Vector()
    //     : data_(nullptr), length_(0), capacity_(0), arena_()
    // {
    //     // data_ = arena_.Alloc<T>(length_);
    // }
    Vector(Arena arena) : data_(nullptr), length_(0), capacity_(0), arena_(arena)
    {
        //data_ = arena_.Alloc<T>(length_);
    }

    void Init(size_t len)
    {
        if(arena_ == nullptr) return;
        
        length_ = len;
        capacity_ = (len < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : capacity_;
        if (!Util::IsPowerOf2(capacity_)) NextCapacity_();
        data_ = arena_.Alloc<T>(length_);
        for (size_t i = 0; i < length_; i++)
        {
            new (&data_[i]) T();
        }
    }

    void Init(size_t len, const T& val)
    {
        if(arena_ == nullptr) return;
                
        length_ = len;
        capacity_ = (len < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : capacity_;
        if(!Util::IsPowerOf2(capacity_)) NextCapacity_();
        data_ = arena_.Alloc<T>(length_);
        for (size_t i = 0; i < length_; i++)
        {
            arena_.ConstructElementAt(data_, i, val);
        }
    }

   /* template<typename ItType>
    Init(ItType first, ItType last, AllocID alloc = DEFAULT_ALLOC_ID)
        : length_(last - first), capacity_(last - first), class_alloc_(alloc)
   
        data_ = arena_.Alloc<T>(length_);
        for (size_t i = 0; i < length_; i++)
        {
            new (&data_[i]) T(*first);
            ++first;
        }
    }*/

    void Init(std::initializer_list<T> list)
    {
        length_ = list.size();
        capacity_ = length_;
        Util::NextPowerOf2(capacity_);
        capacity_ = (capacity_ < DEFAULT_CAPACITY) ? DEFAULT_CAPACITY : capacity_;
        data_ = arena_.Alloc<T>(length_);
        for (size_t i = 0; i < length_; i++)
        {
            new (&data_[i]) T(list[i]);
        }
    }

    // Copy constructor
    Vector(const Vector& other)
        : length_(other.length_), capacity_(other.capacity_), arena_(other.arena_)
    {
        data_ = arena_.Alloc<T>(length_);
        for (size_t i = 0; i < length_; ++i)
        {
            new (&data_[i]) T(other[i]);
        }
    }

    // Move constructor
    Vector(Vector&& other) noexcept
        : data_(other.data_), length_(other.length_), capacity_(other.capacity_), arena_(other.arena_)
    {
        other.data_ = nullptr;
        other.length_ = 0;
        other.capacity_ = 0;
    }

    // Copy assignment operator
    Vector& operator=(const Vector& other_)
    {
        if (this == &other_) return *this;
        //arena_.DestructAll();
        length_ = other_.length_;
        capacity_ = other_.capacity_;
        //you should not copy arena
        // arena_ = other_.arena_;
        data_ = arena_.Alloc<T>(length_);
        for (size_t i = 0; i < length_; ++i)
        {
            new (&data_[i]) T(other_[i]);
        }
        return *this;
    }

    // Move assignment operator
    Vector& operator=(Vector&& other_) noexcept
    {
        //TODO: check if other arena is == to this arena
        // if it is not then, force a deep copy into current arena
        if (this == &other_) return *this;
        //arena_.DestructAll();
        data_ = other_.data_;
        length_ = other_.length_;
        capacity_ = other_.capacity_;
        //arena_ = other_.arena_;
        other_.data_ = nullptr;
        other_.length_ = 0;
        other_.capacity_ = 0;
        //other_.arena_id_ = 0;
        return *this;
    }

    virtual ~Vector() = default;

    T& operator[](size_t idx)
    {
        ASSERT_INBOUNDS(idx, length_);
        return data_[idx];
    }

    const T& operator[](size_t idx) const
    {
        ASSERT_INBOUNDS(idx, length_);
        return data_[idx];
    }

    void PushBack(const T& val)
    {
        if (capacity_ == 0)
        {
            capacity_ = DEFAULT_CAPACITY;
            data_ = arena_.Alloc<T>(capacity_);
        }
        while(length_ >= capacity_)
        {
            size_t old_capacity = capacity_;
            NextCapacity_();
            data_ = arena_.Realloc(data_, old_capacity, capacity_);
        }
        //new (&data_[length_]) T(val_);
        arena_.ConstructElementAt(data_, length_, val);
        ++length_;
        //arena_.IncrementDestructEntry(static_cast<void*>(data_));
    }


    void PopBack()
    {
        if(length_ == 0)return;
        --length_;
        arena_.DestructUnRegister((void*)data_, length_);
    } 

    const T* GetData() const { return data_; }
    size_t Length() const { return length_; }
    size_t Size() const { return length_; }
    bool Empty() const { return length_ == 0; }
    T& Back() const
    {
        return *(data_ + length_ - 1);
    }

    void Clear() { length_ = 0; }
    const String ToString() const
    {
        return String(reinterpret_cast<const char*>(data_), length_);
    }
    void Print()
    {
        std::cout << this << " vector len: " << length_ << " vector cap: " << capacity_ << " [";
        for (size_t i = 0; i < length_; i++)
        {
            std::cout << data_[i] << ", ";
        }
        std::cout << "]\n";
    }

    void Erase(size_t idx)
    {
        if (length_ == 0 || idx >= length_) return;
        for (size_t i = idx; i < length_ - 1; ++i)
        {
            (*this)[i] = std::move((*this)[i + 1]);
        }
        PopBack();
    }

    void EraseRange(size_t first, size_t last)
    {
        if (first == last - 1) return Erase(first);
        if (first >= last || last > length_) return;
        size_t num_after = length_ - last;
        for(size_t i = 0; i < num_after; ++i)
        {
            (*this)[first + i] = std::move((*this)[last + i]);
        }
        size_t num_erased = last - first;
        while(num_erased)
        {
            PopBack();
            --num_erased;
        }
    }

private:
    T* data_;
    size_t length_ = 0;
    size_t capacity_ = 0;
    Arena arena_;

    //void data_ = arena_.Alloc<T>(length_)
    //{
    //    //data_ = (T*)CALLOC_MEM(capacity_ * sizeof(T));
    //    if (capacity_ <= 0) return;
    //    data_ = arena_.Alloc<T>(capacity_);
    //    //data_ = arena_.NewArray<T>(length_, capacity_, );
    //}

    //void _Reallocate()
    //{
    //    // T* new_data = (T*)CALLOC_MEM(capacity_ * sizeof(T));
    //    // for (size_t i = 0; i < length_; ++i)
    //    // {
    //    //     new (&new_data[i]) T(data_[i]);
    //    // }
    //    // if constexpr (!std::is_trivially_destructible<T>::value)
    //    // {
    //    //     for (size_t i = 0; i < length_; ++i)
    //    //     {
    //    //         data_[i].~T();
    //    //     }
    //    // }
    //    // FREE_MEM(data_);
    //    // data_ = new_data;
    //    data_ = arena_.Realloc(data_, 
    //}

    void NextCapacity_()
    {
        capacity_ = Util::NextPowerOf2(capacity_);
    }

public:
    template<typename T, bool IsConst = false, bool IsReverse = false>
    class VectorIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const T*, T*>;
        using reference = std::conditional_t<IsConst, const T&, T&>;

        VectorIterator(T* ptr) : ptr_(ptr) {}

        template <bool OtherConst, std::enable_if_t<IsConst && !OtherConst, int> = 0>
        VectorIterator(const VectorIterator<T, OtherConst, IsReverse>& other)
            : ptr_(other.ptr_) {
        }

        reference operator*() const
        {
            if constexpr (IsReverse) return *(ptr_ - 1);
            else return *ptr_;
        } 
        pointer operator->() const
        {
            if constexpr (IsReverse) return ptr_ - 1; else return ptr_;
        }
        VectorIterator& operator++()
        { 
            if constexpr (IsReverse) --ptr_; else ++ptr_; return *this; 

        } 
        VectorIterator operator++(int) { VectorIterator tmp = *this; ++*this; return tmp; }
        VectorIterator& operator--() { if constexpr (IsReverse) ++ptr_; else --ptr_; return *this; }
        VectorIterator operator--(int) { VectorIterator tmp = *this; --*this; return tmp; }

        template <bool OtherConst>
        bool operator==(const VectorIterator<T, OtherConst, IsReverse>& other)const { return ptr_ == other.ptr_; } \

        template <bool OtherConst>
        bool operator!=(const VectorIterator<T, OtherConst, IsReverse>& other) const { return ptr_ != other.ptr_; }

        bool operator<(const VectorIterator& other) const {
            if constexpr (IsReverse) return ptr_ > other.ptr_;
            else return ptr_ < other.ptr_;
        }
        bool operator>(const VectorIterator& other) const {
            if constexpr (IsReverse) return ptr_ < other.ptr_;
            else return ptr_ > other.ptr_;
        }
        bool operator<=(const VectorIterator& other) const {
            if constexpr (IsReverse) return ptr_ >= other.ptr_;
            else return ptr_ <= other.ptr_;
        }
        bool operator>=(const VectorIterator& other) const {
            if constexpr (IsReverse) return ptr_ <= other.ptr_;
            else return ptr_ >= other.ptr_;
        }

        VectorIterator operator+(difference_type n) const
        {
            if constexpr (IsReverse) return VectorIterator(ptr_ - n);
            else return VectorIterator(ptr_ + n);
        }
        VectorIterator operator-(difference_type n) const
        {
            if constexpr (IsReverse) return VectorIterator(ptr_ + n);
            else return VectorIterator(ptr_ - n);
        }

        difference_type operator-(const VectorIterator& other) const
        {
            if constexpr (IsReverse)
                return other.ptr_ - ptr_;
            else
                return ptr_ - other.ptr_;
        }

        VectorIterator& operator+=(difference_type n)
        {
            ptr_ += (IsReverse ? -n : n);
            return *this;
        }
        VectorIterator& operator-=(difference_type n)
        {
            ptr_ += (IsReverse ? n : -n);
            return *this;
        }

        reference operator[](difference_type n) const
        {
            if constexpr (IsReverse) return *(ptr_ - n - 1);
            else return *(ptr_ + n);
        }


    private:
        pointer ptr_;
        template<typename, bool, bool> friend class VectorIterator;
    };

    using Iterator = VectorIterator<T, false, false>;
    using ConstIterator = VectorIterator<T, true, false>;
    using ReverseIterator = VectorIterator<T, false, true>;
    using ReverseConstIterator = VectorIterator<T, true, true>;
    Iterator begin() { return Iterator(data_); }
    ConstIterator begin() const { return ConstIterator(data_); }
    Iterator end() { return Iterator(data_ + length_); }
    ConstIterator end() const { return ConstIterator(data_ + length_); }
    ConstIterator cbegin() const { return ConstIterator(data_); }
    ConstIterator cend() const { return ConstIterator(data_ + length_); }

    ReverseIterator rbegin() { return ReverseIterator(data_ + length_); }
    ReverseIterator rend() { return ReverseIterator(data_); }
    ReverseConstIterator crbegin() const { return ReverseConstIterator(data_ + length_); }
    ReverseConstIterator crend() const { return ReverseConstIterator(data_); }
};
//template<typename T>
//class Vector {
//public:
//    static constexpr size_t DEFAULT_CAPACITY = 4;
//    static constexpr ArenaID DEFAULT_ARENA_ID = Arena::Frame;
//
//    // Constructors
//    Vector();
//    Vector(ArenaID id);
//
//    // Initialization methods
//    void Init(size_t len);
//    void Init(size_t len, const T& val);
//    void Init(std::initializer_list<T> list);
//
//    // Copy constructor
//    Vector(const Vector& other);
//
//    // Move constructor
//    Vector(Vector&& other) noexcept;
//
//    // Copy assignment operator
//    Vector& operator=(const Vector& other);
//
//    // Move assignment operator
//    Vector& operator=(Vector&& other) noexcept;
//
//    // Destructor
//    virtual ~Vector();
//
//    // Operators
//    T& operator[](size_t idx);
//    const T& operator[](size_t idx) const;
//
//    // Methods
//    void PushBack(const T& val);
//    const T* GetData() const;
//    size_t Length() const;
//    size_t Size() const;
//    bool IsEmpty() const;
//    T& Back() const;
//    void Clear();
//    const String ToString() const;
//    void Print();
//
//private:
//    T* data_;
//    size_t length_;
//    size_t capacity_;
//    Arena arena_;
//
//    void arena_->DestructAll();
//    void data_ = arena_.Alloc<T>(length_);
//    void _Reallocate();
//};