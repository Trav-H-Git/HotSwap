#pragma once

#include "xen_primitives.h"
//#include "xen_vector.h"
//#include <cstdint>
//#include <type_traits>
//#include <functional>
#include <initializer_list>
//#include <vector>
//#include <unordered_map>

//typedef uint32_t Byte;

//typedef uint32_t ArenaID; // Allocator identifier
//typedef uint32_t ui32;
//class DestructorEntry;
//struct Block;
class ArenaPool;
//class ArenaID;

class Mem {
public:
   /* enum ArenaPoolID
    {
       MainArenaPool = 0,
       PhysicsArenaPool = 1,
       RendererArenaPool = 2,
       Count = 3
    };*/
    
    static bool InitInstance(std::initializer_list<size_t> list);
    static Mem* GetInstance();
    static void DestroyInstance();

    // Arena GetArena(ArenaID arena_id, ArenaPoolID arena_pool = MainArenaPool);
    // Arena CreateArena(ArenaPoolID arena_pool = MainArenaPool);
    ArenaPool& GetArenaPool(ThreadID thread_id);

    // void* Alloc(ArenaID id, size_t size, size_t alignment, size_t non_trivial_length);
    // void RegisterDestructor(ArenaID id, DestructorEntry destruct_func);

    // void Reset(ArenaID allocator);
    void PrintDiagnostics() const;


private:
    ArenaPool* arena_pools_;
    // ArenaPool* main_arena_pool_ = nullptr;
    // ArenaPool* physics_arena_pool_ = nullptr;
    // ArenaPool* renderer_arena_pool_ = nullptr;
    // void* main_base_address_ = nullptr;
    // void ResetAll_();
   
    // struct Block {
    //     void* ptr = nullptr;
    //     size_t size = 0; // Always default_block_size_ multiple
    //     size_t used = 0; // Bump-pointer offset
    //     Vector<DestructorEntry> destructor_funcs;
    // };

    // // static void* CommitMemory(Byte* address, size_t size);

    // Mem(size_t total_size, Byte* base, size_t offset);
    Mem() = default;
    ~Mem();
    Mem(const Mem&) = delete;
    Mem(Mem&&) = delete;
    
    static bool ConstructArenaPool_(ThreadID thread_id, ArenaPool* pool, size_t size);

    // bool AllocateBlocks_(size_t num_blocks, Vector<size_t>& block_indices);
    
    // Byte* base_address_ = nullptr;
    // size_t mandatory_offset_ = 0;
    // //Byte* current_address_ = nullptr;
    // size_t committed_size_ = 0;
    // size_t max_committed_size_ = 0;
    // static constexpr size_t default_block_size_ = 256 * 1024;
    // ArenaID next_arena_id_ = 1;
    // Vector<Block> blocks_;
    // Vector<size_t> free_blocks_idxs_;
    // std::unordered_map<ArenaID, Vector<size_t>> alloc_block_idxs_;
    static Mem* instance_;
   
};
