#pragma once

#include "xen_primitives.h"
//#include "xen_vector.h"
#include <cstdint>
#include <type_traits>
//#include <functional>
#include <vector>
//#include <unordered_map>

//typedef uint32_t Byte;

//typedef uint32_t ArenaID; // Allocator identifier
typedef uint32_t ui32;
// class DestructEntry;
struct Block;

/// <summary>
//#pragma once
//#include <new>
//#include <utility>
//#include <type_traits>
//#include <cassert>

template<typename T>
class MemVec
{
public:
    virtual ~MemVec() 
    {
        for (size_t i = 0; i < len_; ++i) data_[i].~T();   // call destructors manually

        ::operator delete(data_); // free the raw memory block
        len_ = 0;
        capacity_ = 0;
    }

    constexpr size_t NextCapacity(size_t current)
    {
        return current ? current * 2 : 4;
    }

    inline void Reallocate(size_t new_cap)
    {
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        size_t old_size = len_;
        if constexpr (std::is_trivially_copyable_v<T>)
        {
            // raw copy for trivial types
            if (old_size)
                std::memcpy(new_data, data_, old_size * sizeof(T));
        }
        else
        {
            // move-construct for non-trivial types
            for (size_t i = 0; i < old_size; ++i)
                new (&new_data[i]) T(std::move(data_[i]));

            // destroy old elements
            if constexpr (!std::is_trivially_destructible_v<T>)
                for (size_t i = 0; i < old_size; ++i)
                    data_[i].~T();
        }

        ::operator delete(data_);
        data_ = new_data;
    }

    inline void PushBack(const T& value)
    {
        if (len_ >= capacity_)
        {
            size_t new_capacity = NextCapacity(capacity_);
            Reallocate(new_capacity);
            capacity_ = new_capacity;
        }

        new (&data_[len_]) T(value);
        ++len_;
    }

    inline void Clear()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < len_; ++i)
                data_[i].~T();
        }
        len_ = 0;
    }

    inline void OrderedInsert(const T& value)
    {
        if (len_ >= capacity_)
        {
            size_t new_capacity = NextCapacity(capacity_);
            Reallocate(new_capacity);
            capacity_ = new_capacity;
        }

        size_t insert_index = 0;
        while (insert_index < len_ && data_[insert_index] < value)
            ++insert_index;

        if constexpr (std::is_trivially_copyable_v<T>)
        {
            if (insert_index < len_)
                std::memmove(&data_[insert_index + 1], &data_[insert_index], (len_ - insert_index) * sizeof(T));
        }
        else
        {
            // move-construct the last element in a new slot
            if (insert_index < len_)
            {
                new (&data_[len_]) T(std::move(data_[len_ - 1]));
                for (size_t i = len_ - 1; i > insert_index; --i)
                    data_[i] = std::move(data_[i - 1]);
            }
        }

        new (&data_[insert_index]) T(value);
        ++len_;
    }

    void EraseRange(size_t first, size_t last)
    {
        if (first >= last) {
            // nothing to erase
            return;
        }
        if (last > len_) last = len_;
        size_t num_after = len_ - last;
        //value_t* data = (capacity > INLINE_CAPACITY) ? buffer.dynamic_data : &(buffer.inline_data[0]);
        T* dest = data_ + first;
        // the source in the array from which to copy:
        T* src = data_ + last;
        // copy `num_to_move` elements (each of sizeof(size_t))
        std::memcpy(dest, src, num_after * sizeof(T));
        // adjust the length
        len_ = first + num_after;

    }

    T& Back() { return this->operator[](len_ - 1); }

    T& operator[](size_t idx) { return data_[idx]; }
    //const T& operator[] (size_t idx) { return data_[idx]; }

private:
    T* data_ = nullptr;
    size_t len_ = 0;
    size_t capacity_ = 0;

    friend class DestructEntry;
    friend class ArenaPool;
    friend class Block;

};

class DestructEntry {
public:
	using DestructorFn = void(*)(void*, size_t);

	DestructEntry(void* obj, DestructorFn destructor)
		: ptr_(obj), length_(0), destructor_(destructor) {
	}
	void DestructElement(size_t idx = 0) {
		if (destructor_ && ptr_ && idx < length_) {
			destructor_(ptr_, idx);
			--length_;
		}
	}

	void Destruct()
	{
		if(destructor_ && ptr_ && length_)
		{
			for (size_t i = 0; i < length_; i++)
			{
				destructor_(ptr_, i);
			}
			length_ = 0;
		}
	}

private:
	size_t length_ = 0;
	void* ptr_ = nullptr;
	DestructorFn destructor_ = nullptr;
    friend class Arena;
};

class ArenaPool {
public:

    // static ArenaPool* GetInstance();
    // static bool InitInstance(size_t total_size);
    // static void DestroyInstance();

    Arena CreateArena();
    Arena GetArena(ArenaID arena_id);

    void* Alloc(ArenaID id, size_t size, size_t alignment, size_t num_elems, bool has_meta_data);
    void RegisterDestructEntry(ArenaID id, DestructEntry destruct_func, u16& block_idx, u16& destructor_idx);

    void Reset(ArenaID allocator);
    void PrintDiagnostics();
    
    // void SetDestructEntryLength();
 
    ArenaPool(ThreadID thread_id, Byte* base, size_t max_size);
    ~ArenaPool();
    ArenaPool() = delete;
    ArenaPool(const ArenaPool&) = delete;
    ArenaPool(ArenaPool&&) = delete;
    DestructEntry* GetDestructEntry(u16 blk_idx, u16 destruct_idx);
    

private:
    void ResetAll_();
   
    //struct IdxList
    //{
    //private:
    //    static constexpr u32 INLINE_CAPACITY = 4;
    //    using value_t = size_t;

    //    union Buffer
    //    {
    //        value_t inline_data[INLINE_CAPACITY];
    //        value_t* dynamic_data;
    //    } buffer;

    //    size_t FindIdx_(value_t* data, value_t val)
    //    {
    //        size_t idx = 0;
    //        for (size_t i = 0; i < length; i++)
    //        {
    //            idx = i;
    //            if (val >= data[i])
    //            {
    //                break;
    //            }
    //        }
    //        return idx;
    //    }

    //    void InsertAndShift_(value_t* data, size_t idx, value_t val)
    //    {
    //        if (length >= capacity)
    //        {
    //            IncreaseCapacity();
    //        }
    //        for (size_t i = length; i > idx; --i) {
    //            data[i] = data[i - 1];
    //        }
    //        data[idx] = val;
    //        ++length;
    //    }
    //public:

    //    size_t length;
    //    size_t capacity;

    //    IdxList() : buffer{}, length(0), capacity(INLINE_CAPACITY) {}

    //    void PushBack(value_t val)
    //    {
    //        if (length >= capacity)
    //        {
    //            IncreaseCapacity();
    //        }
    //        if (capacity > INLINE_CAPACITY)
    //        {
    //            buffer.dynamic_data[length] = val;
    //        }
    //        else
    //        {
    //            buffer.inline_data[length] = val;
    //        }
    //        ++length;
    //    }

    //    void OrderedInsert(value_t val)
    //    {
    //        if (length >= capacity)
    //        {
    //            IncreaseCapacity();
    //        }
    //        size_t idx;
    //        if (capacity > INLINE_CAPACITY)
    //        {
    //            idx = FindIdx_(buffer.dynamic_data, val);
    //            InsertAndShift_(buffer.dynamic_data, idx, val);
    //        }
    //        else
    //        {
    //            idx = FindIdx_(&(buffer.inline_data[0]), val);
    //            InsertAndShift_(&(buffer.inline_data[0]), idx, val);
    //        }
    //    }

    //    void EraseRange(size_t first, size_t last)
    //    {
    //        if (first >= last) {
    //            // nothing to erase
    //            return;
    //        }
    //        if (last > length) last = length;
    //        size_t num_after = length - last;
    //        value_t* data = (capacity > INLINE_CAPACITY) ? buffer.dynamic_data : &(buffer.inline_data[0]);
    //        size_t* dest = data + first;
    //        // the source in the array from which to copy:
    //        size_t* src = data + last;
    //        // copy `num_to_move` elements (each of sizeof(size_t))
    //        std::memcpy(dest, src, num_after * sizeof(value_t));
    //        // adjust the length
    //        length = first + num_after;

    //    }

    //    void IncreaseCapacity()
    //    {
    //        if (capacity == 0)
    //        {
    //            capacity = INLINE_CAPACITY;
    //            return;
    //        }
    //        value_t* new_vals = new value_t[capacity * 2]{};
    //        if (capacity > INLINE_CAPACITY)
    //        {
    //            std::memcpy(new_vals, buffer.dynamic_data, capacity * sizeof(value_t));
    //            delete[] buffer.dynamic_data;
    //        }
    //        else
    //        {
    //            std::memcpy(new_vals, &buffer.inline_data, capacity * sizeof(value_t));
    //        }
    //        buffer.dynamic_data = new_vals;
    //        capacity *= 2;      
    //    }

    //    void Clear()
    //    {
    //        if (capacity > INLINE_CAPACITY)
    //        {
    //            delete[] buffer.dynamic_data;
    //            buffer.dynamic_data = nullptr;
    //        }
    //        length = 0;
    //        capacity = INLINE_CAPACITY;
    //    }

    //    value_t Back() { return this->operator[](length - 1); }

    //    value_t operator[](size_t idx) { if (capacity > INLINE_CAPACITY) { return buffer.dynamic_data[idx]; } else { return buffer.inline_data[idx]; } }
    //    const value_t operator[](size_t idx) const { if (capacity > INLINE_CAPACITY) { return buffer.dynamic_data[idx]; } else { return buffer.inline_data[idx]; } }

    //};

    struct Block 
    {
        
        Block() : ptr(nullptr), size(0), used(0), destruct_entries({}) {}
        //Block() = delete;
        Block(ThreadID thread_id, void* ptr, size_t size, size_t used) : ptr(ptr), size(size), used(used) {}
        ~Block() = default;
        //~Block() { if (destructor_entries) { delete[] destructor_entries; } }
        void* ptr = nullptr;
        size_t size = 0; // Always default_block_size_ multiple
        size_t used = 0; // Bump-pointer offset
        size_t num_allocations = 0;
        //Vector<DestructEntry> destructor_entries;
        MemVec<DestructEntry> destruct_entries;
        //u32 destruct_entries_length = 0;

        //void PushDestructEntry(DestructEntry de)
        //{
        //    //should realloc?
        //    if (destruct_entries_length >= destruct_entries_capacity_)
        //    {
        //        //realloc
        //        destruct_entries_capacity_ = (destruct_entries_capacity_ * 2 < MIN_DF_CAPACITY) ? MIN_DF_CAPACITY : destruct_entries_capacity_ * 2;
        //        if (destruct_entries_length)
        //        {
        //            DestructEntry* new_entries = static_cast<DestructEntry*>(::operator new(sizeof(DestructEntry) * destruct_entries_capacity_));
        //            for (size_t i = 0; i < destruct_entries_length; i++)
        //            {
        //                new (&new_entries[i]) DestructEntry(destruct_entries[i]);
        //                destruct_entries[i].~DestructEntry();
        //            }
        //            ::operator delete(destruct_entries);
        //            destruct_entries = new_entries;
        //        }
        //        else
        //        {
        //            destruct_entries = static_cast<DestructEntry*>(::operator new(sizeof(DestructEntry) * destruct_entries_capacity_));
        //        }
        //    }
        //    //push
        //    new (&destruct_entries[destruct_entries_length++]) DestructEntry(de);
        //}

        void Reset()
        {
            for (i32 i = (i32)destruct_entries.len_ - 1; i >= 0; --i)
            {
                destruct_entries[i].Destruct();
                destruct_entries[i].~DestructEntry();
            }
            destruct_entries.len_ = 0;
            size = 0;
            used = 0;
            num_allocations = 0;
        }

    private:
        //u32 destruct_entries_capacity_ = 0;
        //static u32 constexpr MIN_DF_CAPACITY = 32;
    };

    static void* CommitMemory_(Byte* address, size_t size);

    bool AllocateBlocks_(size_t num_blocks, MemVec<size_t>& block_indices);
    
    ThreadID thread_id_ = 0;
    Byte* base_address_ = nullptr;
    //size_t mandatory_offset_ = 0;
    //Byte* current_address_ = nullptr;
    //size_t committed_size_ = 0;
    size_t max_size_ = 0;
    size_t max_blocks_ = 0;
    //size_t committed_blocks_ = 0;
    static constexpr size_t default_block_size_ = 256 * 1024;
    ArenaID next_arena_id_ = 1;
    //Vector<Block> blocks_;
    //Block* blocks_ = nullptr;
    MemVec<Block> blocks_;
    //Vector<size_t> free_blocks_idxs_;
    //IdxList free_blocks_idxs_;
    //size_t* free_blocks_idxs_ = nullptr;
    MemVec<size_t> free_block_idxs_;
    //Requires Default construction??????
    //std::unordered_map<ArenaID, Vector<size_t>> arena_block_idxs_;
    //std::unordered_map<ArenaID, IdxList> arena_block_idxs_;
    //size_t** arena_block_idxs_ = nullptr;
    MemVec<MemVec<size_t>> arena_block_idxs_;
    // static ArenaPool* instance_;
   
};
