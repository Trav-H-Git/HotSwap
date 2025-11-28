#pragma once
//#include "app.h"
#include "xen_primitives.h"
//#include <cstdint>
#include <type_traits>

#include <utility>
#include <new>
// non trivial get a metadata entry on allocations
// 4 bytes for number of elements in allocation
// 2 bytes for block index
// 2 bytes for DestructEntry index

// DestructEntry contains ptr to metadata and type erased destructor func and size of each element.
// Destruct() will read metadata number of elements and from end of metadata increment metadata ptr to by size of T
// calling destruct function for each element, single allocation will have num elements set to 1.

// Early destruct will be templated with T and pass the ptr to the existing allocation only continuing if non trivial
// if non trivial metadata exists which can be accessed be moving ptr back 8 bytes to locate and invalidate Destructor entry.
// class DestructEntry {
// public:
// 	using DestructorFn = void(*)(void*, size_t);
// 	DestructEntry(void* obj, size_t len, DestructorFn destructor)
// 		: ptr_(obj), length_(len), destructor_(destructor) {
// 	}
// 	void Destruct(size_t idx = 0) {
// 		if (destructor_ && ptr_ && idx < length_) {
// 			destructor_(ptr_, idx);
// 			--length_;
// 		}
// 	}

// 	void DestructAll()
// 	{
// 		if(destructor_ && ptr_ && length_)
// 		{
// 			for (size_t i = 0; i < length_; i++)
// 			{
// 				destructor_(ptr_, i);
// 			}
// 			length_ = 0;
// 		}
// 	}

// private:
// 	size_t length_;
// 	void* ptr_ = nullptr;
// 	DestructorFn destructor_ = nullptr;
// };

//NOTE: Objects will generate copy and move constr/assign,
// move constr/assign requires same Arena context.

template <typename T>
void(*create_destructor_fn())(void*, size_t) 
{
    return [](void* ptr, size_t idx) 
	{
        T* typed_ptr = static_cast<T*>(ptr);
		typed_ptr[idx].~T();
    };
}

// template <typename T>
// void(*create_destructor_fn())(void*, size_t) {
// 	return [](void* ptr, size_t length) {
// 		size_t size = sizeof(T);
// 		for (size_t i = 0; i < length; i++)
// 		{
// 			static_cast<T *>(ptr)->~T();
// 			ptr = static_cast<void*>(static_cast<u8*>(ptr) + size)

// 		}
		
// 	};
// }
//

// typedef uint32_t ArenaID;
class ArenaPool;
class Arena
{
public:
	
	enum ArenasEnum : uint32_t
	{
		Static = 1,
		Frame = 2,
		Node = 3
	};
	Arena() = delete;
	~Arena() = default;
	// Arena(ArenaID id, ArenaPool& arena_pool);// : arena_id(id), thread_id(arena_pool) {}
	Arena(ArenaID arena_id, ThreadID thread_id) : arena_id_(arena_id), thread_id_(thread_id) {};
	Arena(const Arena& other) : arena_id_(other.arena_id_), thread_id_(other.thread_id_) {}
	Arena(Arena&& other) noexcept : arena_id_(other.arena_id_), thread_id_(other.thread_id_) 
	{
		other.arena_id_ = -1;
	}
	Arena& operator=(const Arena&) = delete;
	Arena& operator=(Arena&&) = delete;
	

	template<typename T>
	T* Realloc(T* old_ptr, size_t old_len, size_t new_len)
	{
		if (old_ptr == nullptr || old_len == 0) return Alloc<T>(new_len);
		if (arena_id_ == 0 || new_len == 0) return nullptr;

		void* new_mem = nullptr;
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			new_mem = AllocNonTrivial_(new_len, sizeof(T), alignof(T));
			void(*df)(void*, size_t) = create_destructor_fn<T>();
			RegisterDestructEntry(new_mem, df);
		}
		else
		{
			new_mem = Alloc_(new_len, sizeof(T), alignof(T));
		}

		for(size_t i = 0; i < old_len && i < new_len; ++i)
		{
			ConstructElementAt<T>(static_cast<T*>(new_mem), i, old_ptr[i]);
		}

		return static_cast<T*>(new_mem);
	}
	
	template<typename T>
	T* Alloc(size_t len = 1)
	{
		//TODO: debug check
		if (arena_id_ == 0 || len == 0) return nullptr;

		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			//void(*df)(void*, size_t) = create_destructor_fn<T>();
			//RegisterDestructEntry_(df);
			void* new_mem = AllocNonTrivial_(len, sizeof(T), alignof(T));
			void(*df)(void*, size_t) = create_destructor_fn<T>();
			RegisterDestructEntry(new_mem, df);
			return static_cast<T*>(new_mem);

		}
		else
		{
			return static_cast<T*>(Alloc_(len, sizeof(T), alignof(T)));

		}
		//void* new_mem = Alloc_(len, sizeof(T), alignof(T));

		//return static_cast<T*>(new_mem);
	}

	template<typename T, typename... Args>
	T* ConstructAt(T* at, Args... args)
	{
		//TODO: debug check
		if(arena_id_ == 0) return nullptr;

		// void* new_mem = Alloc_(arena_id_, sizeof(T), alignof(T), 1);
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			//void(*df)(void*, size_t) = create_destructor_fn<T>();
			//RegisterDestructEntry_(df);
			IncrementDestructEntry(static_cast<void*>(at));

		}

		return new (at) T(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	T* Construct(Args... args)
	{
		//TODO make efficient
		return ConstructAt<T>(Alloc<T>(1), std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	T* ConstructElementAt(T* base_ptr, size_t offset, Args&&... args)
	{
		//TODO: debug check
		if(arena_id_ == 0) return nullptr;
		//if(length > capacity || length == 0) return nullptr;
		
		//void* new_mem = Alloc_(arena_id_, sizeof(T), alignof(T), capacity);
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			//void(*df)(void*, size_t) = create_destructor_fn<T>();
			//RegisterDestructEntry_(df);
			IncrementDestructEntry(static_cast<void*>(base_ptr));
		}

		T* at = base_ptr + offset;
		return new (at) T(std::forward<Args>(args)...);

		//u8* ptr = static_cast<u8*>(at);
		//for (size_t i = 0; i < length; ++i)
		//{
		//	// Calculate pointer for the i-th element:
		//	T* elementPtr = reinterpret_cast<T*>(ptr + i * sizeof(T));

		//	// Placement new to construct element in-place:
		//	new (elementPtr) T(std::forward<Args>(args)...);
		//}
		//return static_cast<T*>(new_mem);
	}

	void Reset();

	template<typename T>
	void DestructAll(T* ptr)
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			u16 blk_idx, des_idx;
			LoadMetaData_(ptr, blk_idx, des_idx);
			DestructAll_(blk_idx, des_idx);
		}
	}

	template<typename T>
	void DestructKeepRegistered(T* ptr, size_t idx = 0)
	{
		if constexpr (std::is_trivially_destructible_v<T>)
		{
			return;
		}
		else
		{
			(ptr[idx]).~T();

		}
	}

	template<typename T>
	void DestructUnRegister(T* ptr, size_t idx = 0)
	{
		if constexpr (std::is_trivially_destructible_v<T>) return;

		u16 blk_idx, des_idx;
		LoadMetaData_(ptr, blk_idx, des_idx);
		Destruct_(blk_idx, des_idx, idx);
		
	}

	void RegisterDestructEntry(void* ptr, void(*df)(void*, size_t));
	void IncrementDestructEntry(void* ptr);
	void DecrementDestructEntry(void* ptr);

	
	//void UnRegisterDestructor(T*)
private:

	// ArenaPool& thread_id;
	ThreadID thread_id_;

	ArenaID arena_id_;

	// void* GetMem_(ArenaID id, size_t size, size_t alignment);
	void* Alloc_(size_t len, size_t size, size_t align);
	void* AllocNonTrivial_(size_t len, size_t size, size_t align);
	void LoadMetaData_(void *ptr, u16& block_idx, u16 &destructor_idx);
	void StoreMetaData_(void *ptr, u16 block_idx, u16 destructor_idx);
	void Destruct_(u16 b_idx, u16 d_idx, size_t idx);
	void DestructAll_(u16 b_idx, u16 d_idx);

};