#include "mem.h"
#include "app.h"
#include "arena_pool.h"
#include "arena.h"
#include <iostream>
#include <cassert>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

// static Mem::ArenaPool* main_arena_pool_ = nullptr;
// static Mem::ArenaPool* physics_arena_pool_ = nullptr;
// static Mem::ArenaPool* renderer_arena_pool_ = nullptr; 
Mem* Mem::instance_ = nullptr;
//AllocID Mem::static_allocator_ = 0;
//AllocID Mem::frame_allocator_ = 0;

// Mem::Mem(size_t total_size, Byte* base, size_t offset)
//     : committed_size_(offset),
//     base_address_(base),
//     max_committed_size_(total_size),
//     mandatory_offset_(offset)
// {

//     CreateArena();//static allocator 1
//     CreateArena();//frame allocator 2
//     CreateArena();//node allocator 3
//     //TODO: construct vector and hashmpa members with allocator_

// }
// Mem::Mem()
// {

// }

Mem::~Mem() {
    for (size_t i = 0; i < (size_t)Threads::Count; ++i) instance_->arena_pools_[i].~ArenaPool();
    operator delete[]((void*)instance_->arena_pools_);
    
    //delete instance_;
    //instance_ = nullptr;
}

bool Mem::ConstructArenaPool_(ThreadID thread_id, ArenaPool* pool, size_t size)
{
    size_t page_size = 4096;
    size = (size + page_size - 1) & ~(page_size - 1);
    if(page_size > size) return false;

    void* base_virtual = VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
    if (!base_virtual) 
    {
        //VirtualFree(base_virtual, 0, MEM_RELEASE);
        return false;
    }

    // void* committed_page = VirtualAlloc(base_virtual, page_size, MEM_COMMIT, PAGE_READWRITE);
    // if(!committed_page)
    // {
    //     VirtualFree(base_virtual, 0, MEM_RELEASE);
    //     return false;
    // }
    
    // size_t ap_alignment = alignof(ArenaPool);
    // uintptr_t current_committed = reinterpret_cast<uintptr_t>(committed_page);
    // uintptr_t aligned_committed = (current_committed + ap_alignment - 1) & ~(ap_alignment - 1);
    new (pool) ArenaPool(thread_id, reinterpret_cast<Byte*>(base_virtual), size);
    return true;
}

Mem* Mem::GetInstance()
{
    return instance_;
}

bool Mem::InitInstance(std::initializer_list<size_t> list)
{
    if(list.size() != (size_t)Threads::Count) return false;
    instance_ = new Mem();
    
    instance_->arena_pools_ = (ArenaPool*)operator new[](sizeof(ArenaPool) * list.size());
    ArenaPool* pool = instance_->arena_pools_;
    for(size_t i = 0; i < list.size(); ++i)
    { 
        if (!ConstructArenaPool_((ThreadID)i, pool, *(list.begin() + i))) return false;
        ++pool;
        // new (&instance_->arena_pools_[i]) ArenaPool((ThreadID)i, instance_->arena_pools_[i], *(list.begin() + i));
    }

    // if (!CreateArenaPool_(instance_->main_arena_pool_, main_size)) return false;
    // if (!CreateArenaPool_(instance_->physics_arena_pool_, physics_size)) return false;
    // if (!CreateArenaPool_(instance_->renderer_arena_pool_, renderer_size)) return false;
    
    // size_t page_size = 4096;
    // if(main_size < page_size || physics_size < page_size || renderer_size < page_size) return false;
    // if(sizeof(Mem) + sizeof(ArenaPool) +alignof(ArenaPool) > page_size) return false;
    // main_size = (main_size + page_size - 1) & ~(page_size - 1);
    // physics_size = (physics_size + page_size - 1) & ~(page_size - 1); renderer_size = (renderer_size + page_size - 1) & ~(page_size - 1);

    
    // void* main_base_virtual = VirtualAlloc(nullptr, main_size, MEM_RESERVE, PAGE_READWRITE);
    // if (!main_base_virtual) {
    //     return false;
    // }
    // void* physics_base_virtual = VirtualAlloc(nullptr, physics_size, MEM_RESERVE, PAGE_READWRITE);
    // if (!physics_base_virtual) {
    //     return false;
    // }
    // void* renderer_base_virtual = VirtualAlloc(nullptr, renderer_size, MEM_RESERVE, PAGE_READWRITE);
    // if (!renderer_base_virtual) {
    //     return false;
    // }

    // void* base_committed = VirtualAlloc(main_base_virtual, page_size, MEM_COMMIT, PAGE_READWRITE);
    // if (!base_committed) {
    //     VirtualFree(main_base_virtual, 0, MEM_RELEASE);
    //     return false;
    // }
    // instance_ = new (base_committed) Mem();
    // if(!instance_) return false;
    // instance_->main_base_address_ = base_committed;

    // size_t ap_alignment = alignof(ArenaPool);
    // uintptr_t current_committed = reinterpret_cast<uintptr_t>(base_committed) + sizeof(Mem);
    // uintptr_t aligned_committed = (current_committed + ap_alignment - 1) & ~(ap_alignment - 1);
    // instance_->main_arena_pool_ = new (reinterpret_cast<void*>(aligned_committed)) ArenaPool(static_cast<Byte*>(main_base_virtual), page_size, main_size);
    // if(!instance_->main_arena_pool_) return false;

    // current_committed = aligned_committed + sizeof(ArenaPool);
    // aligned_committed = (current_committed + ap_alignment - 1) & ~(ap_alignment - 1);
    // instance_->physics_arena_pool_ = new (reinterpret_cast<void*>(aligned_committed)) ArenaPool(static_cast<Byte*>(physics_base_virtual), 0, physics_size);
    // if(!instance_->physics_arena_pool_) return false;

    // current_committed = aligned_committed + sizeof(ArenaPool);
    // aligned_committed = (current_committed + ap_alignment - 1) & ~(ap_alignment - 1);
    // instance_->renderer_arena_pool_ = new (reinterpret_cast<void *>(aligned_committed)) ArenaPool(static_cast<Byte*>(renderer_base_virtual), 0, renderer_size);
    // if(!instance_->renderer_arena_pool_) return false;

    return true;
}

void Mem::DestroyInstance()
{
    if (instance_) {
        delete instance_;
        instance_ = nullptr;
    }
}

// Arena Mem::GetArena(ArenaID arena_id, ArenaPoolID arena_pool)
// {
//     ArenaPool* ap;
//     switch (arena_pool)
//     {
//     case MainArenaPool:
//         ap = main_arena_pool_;
//         break;
//     case PhysicsArenaPool:
//         ap = physics_arena_pool_;
//         break;
//     case RendererArenaPool:
//         ap = renderer_arena_pool_;
//         break;
//     default:
//         ap = main_arena_pool_;
//         break;
//     }
//     return ap->GetArena(arena_id);
// }

// Arena Mem::CreateArena(ArenaPoolID arena_pool)
// {
//     ArenaPool* ap;
//     switch (arena_pool)
//     {
//     case MainArenaPool:
//         ap = main_arena_pool_;
//         break;
//     case PhysicsArenaPool:
//         ap = physics_arena_pool_;
//         break;
//     case RendererArenaPool:
//         ap = renderer_arena_pool_;
//         break;
//     default:
//         ap = main_arena_pool_;
//         break;
//     }
//     return ap->CreateArena();
// }

ArenaPool& Mem::GetArenaPool(ThreadID thread_id)
{
  return arena_pools_[thread_id];  
}

// void* Mem::Alloc(ArenaID id, size_t size, size_t alignment, size_t non_trivial_length)
// {
//     //assert(allocator != 0 && "Invalid allocator");
//     //TODO: alignment is power of 2

//     if (!alloc_block_idxs_.contains(id))
//     {
//         return nullptr;
//     }

//     //size_t size = sizeof(T);
//     //size_t alignment = alignof(T);
//     size_t num_blocks = (size + default_block_size_ - 1) / default_block_size_;

//     // Check the last block for this allocator if single block needed
//     size_t block_idx = 0;
//     Vector<size_t>& block_idxs = alloc_block_idxs_.at(id);
//     if (block_idxs.Size() > 0)
//     {
//         block_idx = block_idxs.Back();
//         Block& block = blocks_[block_idx];
//         uintptr_t current_ptr = reinterpret_cast<uintptr_t>(static_cast<Byte*>(block.ptr) + block.used);
//         uintptr_t aligned_ptr = (current_ptr + alignment - 1) & ~(alignment - 1);
//         size_t padding = aligned_ptr - current_ptr;
//         if (block.used + padding + size <= block.size) {
//             // Use this block
//             block.used += padding + size;
//             void* ptr = reinterpret_cast<void*>(aligned_ptr);
//             /* T* obj = new (ptr) T(std::forward<Args>(args)...);
//              if constexpr (!std::is_trivially_destructible_v<T>) {
//                  block.destructor_funcs.push_back(Destructor{ptr, [](void* ptr) { static_cast<T*>(ptr)->~T(); } });
//              }*/

//              //block.destruct_funcs.push_back(std::move(alloc));
//             return ptr;
//         }
//         //block_idx = blocks_.size(); // Need a new block
//     }

//     // Allocate new block(s)
//     //std::vector<size_t> block_indices;
//     if (!AllocateBlocks_(num_blocks, block_idxs)) {
//         return nullptr;
//     }

//     // Use the last block for the allocation
//     block_idx = block_idxs.Back();
//     Block& block = blocks_[block_idx];
//     uintptr_t current_ptr = reinterpret_cast<uintptr_t>(block.ptr);
//     uintptr_t aligned_ptr = (current_ptr + alignment - 1) & ~(alignment - 1);
//     size_t padding = aligned_ptr - current_ptr;
//     block.used = padding + size;
    

//     void* ptr = reinterpret_cast<void*>(aligned_ptr);
//     /* T* obj = new (ptr) T();
//      if constexpr (!std::is_trivially_destructible_v<T>) {

//          block.destructor_funcs.push_back(Destructor{ ptr, [](void* ptr) { static_cast<T*>(ptr)->~T(); } });
//      }*/

//     return ptr;
// }

// void Mem::RegisterDestructor(ArenaID id, DestructorEntry de)
// {
//     if (!alloc_block_idxs_.contains(id))
//     {
//         return;
//     }
//     Vector<size_t>& block_idxs = alloc_block_idxs_.at(id);
//     if (block_idxs.Size() > 0)
//     {
//         size_t block_idx = block_idxs.Back();
//         Block& block = blocks_[block_idx];
//         block.destructor_funcs.PushBack(de);
//     }
// }

// void* Mem::CommitMemory(Byte* address, size_t size)
// {
// #ifdef _WIN32
//     return VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
// #else
//     return mmap(address, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
// #endif
// }


// void Mem::Reset(ArenaID allocator) {
//     auto it = alloc_block_idxs_.find(allocator);
//     if (it == alloc_block_idxs_.end()) return;

//     for (size_t block_idx : it->second) {
       
//         Block& main_block = blocks_[block_idx];
//         //memory_free_ += main_block.size;
//         size_t i = 1;
//         //for big overflow blocks 
//         while (main_block.size > default_block_size_)
//         {
//             Block& block = blocks_[block_idx + i];
//             block.used = 0;
//             block.size = 0;
//             for (auto& destructor_func : block.destructor_funcs) {
//                 //destruct_func();
//                 destructor_func.Destruct();
//             }
//             block.destructor_funcs.Clear();

//             free_blocks_idxs_.PushBack(block_idx + i);
//             main_block.size -= default_block_size_;
//             i++;
//         }

//         main_block.used = 0;
//         main_block.size = 0;
//         //for (auto& destructor_func : main_block.destructor_funcs)
//         for (auto it = main_block.destructor_funcs.rbegin(); it != main_block.destructor_funcs.rend(); ++it)
//         {
//             //it->destruct(it->ptr);
//             it->Destruct();
//         }
//         main_block.destructor_funcs.Clear();
//         free_blocks_idxs_.PushBack(block_idx);

//     }

//     // Sort free_blocks_idxs to group contiguous blocks
//     std::sort(free_blocks_idxs_.begin(), free_blocks_idxs_.end(),
//         [](const size_t& a, const size_t& b) { return a < b; });

//     it->second.Clear();
// }

// void Mem::ResetAll_() 
// {
//     while (next_arena_id_ > 1)
//     {
//         next_arena_id_--;
//         Reset(next_arena_id_);

//     }
//     committed_size_ = mandatory_offset_;
// }

void Mem::PrintDiagnostics() const {
    for (size_t i = 0; i < (size_t)Threads::Count; i++)
    {
        arena_pools_[i].PrintDiagnostics();
    }
    
}

// bool Mem::AllocateBlocks_(size_t num_blocks, Vector<size_t>& block_indices) {
//     if (num_blocks == 0) return false;

//     size_t total_alloc_size = num_blocks * default_block_size_;

//     // Try to find contiguous blocks in free_blocks_idxs
//     if (!free_blocks_idxs_.Empty()) {
//         // Ensure free_blocks_idxs is sorted (should be, due to Reset)
//         for (size_t i = 0; i <= free_blocks_idxs_.Size() - num_blocks; ++i) {
//             bool contiguous = true;
//             size_t first_block_idx = free_blocks_idxs_[i];
//             for (size_t j = 1; j < num_blocks; ++j) 
//             {
//                 if (first_block_idx + j != free_blocks_idxs_[i + j])
//                 {
//                     contiguous = false;
//                     i += j - 1;
//                     break;
//                 }
//             }
//             if (contiguous)
//             {
//                 // Found contiguous blocks
                
//                 //size_t first_block_idx = free_blocks_idxs_[i];
//                 Block block = blocks_[first_block_idx];
//                 block.used = 0;
//                 block.size = total_alloc_size;
//                 block.destructor_funcs.Clear();
//                 block_indices.PushBack(first_block_idx);
//                 free_blocks_idxs_.Erase(free_blocks_idxs_.begin() + i, free_blocks_idxs_.begin() + i + num_blocks);

//                 return true;
//             }
//         }
//     }

//     // Commit new contiguous blocks
//     if (committed_size_ + total_alloc_size > max_committed_size_) {
//         return false;
//     }

//     void* ptr = CommitMemory(base_address_ + committed_size_, total_alloc_size);
//     if (!ptr) {
//         return false;
//     }
//     committed_size_ += total_alloc_size;
//     //current_address_ += total_size;

//     // Create blocks
//     size_t block_idx = blocks_.Size();
//     for (size_t i = 0; i < num_blocks; ++i) 
//     {
//         void* block_ptr = static_cast<Byte*>(ptr) + i * default_block_size_;
//         blocks_.PushBack({ block_ptr, default_block_size_, 0, {} });
//     }
//     block_indices.PushBack(block_idx);
//     //total_allocated_ += total_size;
//     return true;
// }

