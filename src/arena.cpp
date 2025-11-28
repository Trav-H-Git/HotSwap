#include "arena.h"

#include "arena_pool.h"
#include "mem.h"

// void Arena::DestructBack(T* ptr)
// {
//     u16 blk_idx, des_idx;
//     LoadMetaData_(ptr, blk_idx, des_idx);

// }

// void *Arena::GetMem_(ArenaID id, size_t size, size_t alignment)
// {
//     return arena_pool_->Alloc(id, size, alignment);
// }

//Arena::Arena(ArenaID arena_id, ThreadID thread_id) : arena_id_(arena_id), thread_id_(thread_id)
//{
//}
//
//Arena::Arena(const Arena& other) : arena_id_(other.arena_id_), thread_id_(other.thread_id_)
//{
//}

void Arena::Reset()
{
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    pool.Reset(arena_id_);
}

void *Arena::Alloc_(size_t num_elements, size_t size, size_t align)
{
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    return pool.Alloc(arena_id_, size, align, num_elements, false);
}

void* Arena::AllocNonTrivial_(size_t num_elements, size_t size, size_t align)
{
    /*void* mem = arena_pool_->Alloc(id, size, align, num_elems, true);
    u16 block_idx, destructor_entry_idx;
    DestructEntry de{mem, num_elems, df};
    arena_pool_->RegisterDestructEntry(id, de, block_idx, destructor_entry_idx);
    StoreMetaData_(mem, block_idx, destructor_entry_idx);
    return mem;*/
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    return pool.Alloc(arena_id_, size, align, num_elements, true);
}

void Arena::StoreMetaData_(void* ptr, u16 block_idx, u16 destructor_idx) 
{
    // Combine src1 (lower 16 bits) and src2 (upper 16 bits) into a u32
    u32 combined = (static_cast<u32>(destructor_idx) << 16) | (static_cast<u32>(block_idx) & 0xFFFF);
    
    // Store the 32-bit value in the 4 bytes before ptr
    uint8_t* dest = reinterpret_cast<uint8_t*>(ptr) - 4;
    std::memcpy(dest, &combined, sizeof(u32));
}
void Arena::Destruct_(u16 b_idx, u16 d_idx, size_t idx)
{
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    DestructEntry* de = pool.GetDestructEntry(b_idx, d_idx);
    de->DestructElement(idx);
}

void Arena::DestructAll_(u16 b_idx, u16 d_idx)
{
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    DestructEntry* de = pool.GetDestructEntry(b_idx, d_idx);
    de->Destruct();
}

void Arena::RegisterDestructEntry(void* ptr, void (*df)(void *, size_t))
{
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    DestructEntry de{ptr, df};
    u16 block_idx, destructor_entry_idx;
    pool.RegisterDestructEntry(arena_id_, de, block_idx, destructor_entry_idx);
    StoreMetaData_(ptr, block_idx, destructor_entry_idx);
}
void Arena::IncrementDestructEntry(void* ptr)
{
    u16 block_idx, destruct_idx;
    LoadMetaData_(ptr, block_idx, destruct_idx);
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    DestructEntry* de = pool.GetDestructEntry(block_idx, destruct_idx);
    de->length_++;
}

void Arena::DecrementDestructEntry(void *ptr)
{
    u16 block_idx, destruct_idx;
    LoadMetaData_(ptr, block_idx, destruct_idx);
    ArenaPool& pool = Mem::GetInstance()->GetArenaPool(thread_id_);
    DestructEntry* de = pool.GetDestructEntry(block_idx, destruct_idx);
    de->length_--;
}

void Arena::LoadMetaData_(void *ptr, u16 &block_idx, u16 &destructor_idx)
{
    // Load 32 bits from the 4 bytes before ptr
    uint8_t* src = reinterpret_cast<uint8_t*>(ptr) - 4;
    u32 combined;
    std::memcpy(&combined, src, sizeof(u32));
    
    // Extract lower 16 bits (dest1) and upper 16 bits (dest2)
    block_idx = static_cast<u16>(combined & 0xFFFF);
    destructor_idx = static_cast<u16>((combined >> 16) & 0xFFFF);
}
