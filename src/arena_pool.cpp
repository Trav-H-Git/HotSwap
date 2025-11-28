#include "arena_pool.h"
#include "arena.h"
#include "xen_assert.h"
#include "util.h"
#include <iostream>
// #include <cassert>
//#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

// ArenaPool* ArenaPool::instance_ = nullptr;
//AllocID ArenaPool::static_allocator_ = 0;
//AllocID ArenaPool::frame_allocator_ = 0;

ArenaPool::ArenaPool(ThreadID thread_id, Byte* base, size_t max_size)
    : base_address_(base),
    max_size_(max_size),
    max_blocks_(max_size / default_block_size_),
    thread_id_(thread_id)
{
    //blocks_ = new Block[max_blocks_]{};
    //blocks_ = new Block[max_blocks_]{};
    //for (size_t i = 0; i < max_blocks_; ++i) blocks_[i] = new Block();

    CreateArena();//static allocator 1
    CreateArena();//frame allocator 2
    CreateArena();//node allocator 3
    //TODO: construct vector and hashmpa members with allocator_

}

ArenaPool::~ArenaPool() {
    ResetAll_();
   /* for (size_t i = 0; i < committed_blocks_; ++i)
    {
        delete blocks_[i];
    }*/
    //delete[] blocks_;
}

// void ArenaPool::SetDestructEntryLength(u8 blk_idx, u8 destruct_idx, size_t len)
// {
//     //TODO: debug check
//     blocks_[blk_idx].destructor_entry[destruct_idx].length = len;

// }

// ArenaPool* ArenaPool::GetInstance()
// {
//     return instance_;
// }

// bool ArenaPool::InitInstance(size_t total_size)
// {
//     size_t page_size = 4096;
//     total_size = (total_size + page_size - 1) & ~(page_size - 1);

//     Byte* base_virtual = static_cast<Byte*>(
//         VirtualAlloc(nullptr, total_size, MEM_RESERVE, PAGE_READWRITE));
//     if (!base_virtual) {
//         return false;
//     }

//     size_t offset = 0;
//     void* base_committed = VirtualAlloc(base_virtual, sizeof(ArenaPool), MEM_COMMIT, PAGE_READWRITE);
//     if (!base_committed) {
//         VirtualFree(base_virtual, 0, MEM_RELEASE);
//         return false;
//     }
//     offset += sizeof(ArenaPool);
//     offset = (offset + page_size - 1) & ~(page_size - 1);
//     instance_ = new (base_committed) ArenaPool(total_size, base_virtual, offset);

//     return true;
// }

// void ArenaPool::DestroyInstance()
// {
//     if (instance_) {
//         Byte* base_address = instance_->base_address_;
//         instance_->~ArenaPool();
//         VirtualFree(base_address, 0, MEM_RELEASE);
//         instance_ = nullptr;
//     }
// }

Arena ArenaPool::CreateArena()
{
    ArenaID id = next_arena_id_++;
    //arena_block_idxs_.insert({ id, Vector<size_t>{Arena(Arena::Static, thread_id_)} });
    //arena_block_idxs_.insert({ id, IdxList{} });
    //arena_block_idxs_.insert({ id, IdxList{} });
    arena_block_idxs_.PushBack(MemVec<size_t>{});
    return Arena(id, thread_id_);
}

Arena ArenaPool::GetArena(ArenaID arena_id)
{
    //TODO: error on no arena

    return Arena(arena_id, thread_id_);
}

void* ArenaPool::Alloc(ArenaID id, size_t size, size_t alignment, size_t num_elems, bool has_meta_data)
{
    ASSERT(id > 0 && Util::IsPowerOf2(alignment));
    //TODO: alignment is power of 2

    /*if (!arena_block_idxs_.contains(id))
    {
        return nullptr;
    }*/
    if (id > arena_block_idxs_.len_)
    {
        return nullptr;
    }
    size_t whole_size = size * num_elems;
    if(whole_size == 0) return nullptr;
    size_t meta_size = (has_meta_data) ? 4 : 0;
    //size_t size = sizeof(T);
    //size_t alignment = alignof(T);

    // Check the last block for this allocator if single block needed
    //Vector<size_t>& block_idxs = arena_block_idxs_.at(id);
    //size_t* block_idxs = arena_block_idxs_.at(id);
    //IdxList& block_idxs = arena_block_idxs_.at(id);
    MemVec<size_t>& block_idxs = arena_block_idxs_[id - 1];
    size_t block_idx = 0;
    if (block_idxs.len_ > 0)
    {
        block_idx = block_idxs.Back();
        Block* block = &blocks_[block_idx];
        uintptr_t current_ptr = reinterpret_cast<uintptr_t>(block->ptr) + block->used;
        //possible extra 4 bytes for meta data (needed to lookup destructorentry quick)
        current_ptr += meta_size;
        uintptr_t aligned_ptr = (current_ptr + alignment - 1) & ~(alignment - 1);
        //padding possibly includes 4 bytes of meta data
        size_t padding = aligned_ptr - current_ptr;
        if (block->used + padding + whole_size <= block->size) {
            // Use this block
            block->used += padding + whole_size;
            block->num_allocations++;
            void* ptr = reinterpret_cast<void*>(aligned_ptr);
            /* T* obj = new (ptr) T(std::forward<Args>(args)...);
             if constexpr (!std::is_trivially_destructible_v<T>) {
                 block.destruct_funcs.push_back(Destructor{ptr, [](void* ptr) { static_cast<T*>(ptr)->~T(); } });
             }*/

             //block.destruct_funcs.push_back(std::move(alloc));
            return ptr;
        }
        //block_idx = blocks_.size(); // Need a new block
    }

    // Allocate new block(s)
    //std::vector<size_t> block_indices;
    size_t num_blocks = (whole_size + meta_size + default_block_size_ - 1) / default_block_size_;
    if (!AllocateBlocks_(num_blocks, block_idxs)) {
        return nullptr;
    }

    // Use the last block for the allocation
    block_idx = block_idxs.Back();
    Block* block = &blocks_[block_idx];
    uintptr_t current_ptr = reinterpret_cast<uintptr_t>(block->ptr);
    current_ptr += meta_size;
    uintptr_t aligned_ptr = (current_ptr + alignment - 1) & ~(alignment - 1);
    size_t padding = aligned_ptr - current_ptr;
    block->used = padding + whole_size;
    block->num_allocations++;
    
    return reinterpret_cast<void*>(aligned_ptr);
}

void ArenaPool::RegisterDestructEntry(ArenaID id, DestructEntry destruct_func, u16& b_idx, u16& d_idx)
{
   /* if (!arena_block_idxs_.contains(id))
    {
        return;
    }*/
    if (id > arena_block_idxs_.len_) return;
    //Vector<size_t>& block_idxs = arena_block_idxs_.at(id);
    MemVec<size_t>& block_idxs = arena_block_idxs_[id - 1];
    if (block_idxs.len_ > 0)
    {
        size_t block_idx = block_idxs.Back();
        Block* block = &blocks_[block_idx];
        b_idx = (u16)block_idx;
        //d_idx = (u16)block->destruct_funcs.Size();
        d_idx = (u16)block->destruct_entries.len_;
       // block->destruct_funcs.PushBack(destruct_func);
        //block->PushDestructEntry(destruct_func);
        block->destruct_entries.PushBack(destruct_func);
        
    }
}

void* ArenaPool::CommitMemory_(Byte* address, size_t size)
{
#ifdef _WIN32
    return VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
#else
    return mmap(address, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
}


void ArenaPool::Reset(ArenaID allocator) {
    if (allocator > arena_block_idxs_.len_) return;
    MemVec<size_t>& idx_list = arena_block_idxs_[allocator - 1];
    //reverse for each arena block
    for (i32 i = idx_list.len_ - 1; i >= 0; --i) {
       
        size_t main_block_idx = idx_list[i];
        Block* main_block = &blocks_[main_block_idx];
        //memory_free_ += main_block->size;
        
        //for big overflow blocks 
        // size_t i = 1;
        // while (main_block->size > default_block_size_)
        // {
        //     Block& block = blocks_[main_block_idx + i];
        //     block.used = 0;
        //     block.size = 0;
        //     // TODO: if destructor_func.Size() > 0 then error
        //     block.destruct_funcs.Clear();
        //     free_blocks_idxs_.PushBack(main_block_idx + i);
        //     main_block->size -= default_block_size_;
        //     i++;
        // }
        u32 num_blocks = main_block->size / default_block_size_;
        
        //main_block->size = 0;
        //main_block->used = 0;
        //reverse for each destruct_entry in block
       /* for (i32 j = main_block->destruct_funcs_length - 1; j >= 0; --j)
        {
            main_block->destruct_funcs[j].Destruct();
        }*/
        main_block->Reset();
        //insert block, considering large blocks, in free list
        for(size_t j = 0; j < num_blocks; ++j)
        {
            //free_blocks_idxs_.PushBack(main_block_idx + i);
            free_block_idxs_.OrderedInsert(main_block_idx + j);
        }

    }

    // Sort free_blocks_idxs to group contiguous blocks
   /* std::sort(free_blocks_idxs_.begin(), free_blocks_idxs_.end(), 
        [](const size_t &a, const size_t &b) { return a < b; });*/

    idx_list.Clear();
}

DestructEntry* ArenaPool::GetDestructEntry(u16 b_idx, u16 d_idx)
{
    //TODO: debug check inbounds for both block and destructentry
    if (b_idx < blocks_.len_)
    {
        Block* blk = &blocks_[b_idx];
        if(d_idx < blk->destruct_entries.len_)
        {    
            return &blk->destruct_entries[d_idx];
        }   

    }
    
    return nullptr;

}

void ArenaPool::ResetAll_()
{
    //for range nextalloc - 1 to 1 inclusivly
    while (next_arena_id_ > 1)
    {
        next_arena_id_--;
        Reset(next_arena_id_);

    }
    //committed_size_ = mandatory_offset_;
    //committed_size_ = 0;
}

void ArenaPool::PrintDiagnostics() {
    std::cout << "--------------------------ArenaPool Thread: " << thread_id_ <<" --------------------------\n"
        << "  Max mem Allowed: " << max_size_ << " bytes\n"
        << "  mem Committed: " << blocks_.len_ * default_block_size_ << " bytes\n"
        << "  Blocks Committed: " << blocks_.len_ << "\n"
        << "  Blocks Used: " << blocks_.len_ - free_block_idxs_.len_ << "\n"
        << "  Free Blocks: " << free_block_idxs_.len_ << "\n"
        << "  Arenas Created: " << arena_block_idxs_.len_ << "\n";
    for (size_t i = 0; i < arena_block_idxs_.len_; ++i)
    {
        MemVec<size_t>& it = arena_block_idxs_[i];
        std::cout << "###### ArenaID: " << i + 1 << " #####\n";
        for (size_t j = 0; j < it.len_; ++j)
        {
            const size_t idx = it[j];
            Block* blk = &blocks_[idx];
            std::cout << "   Size: " << blk->size << "\n"
                << "   Used: " << blk->used << "\n"
                << "   Remaining: " << blk->size - blk->used << "\n"
                << "   Non-Trivial Allocs: " << blk->destruct_entries.len_ << "\n"
                << "   Total Allocs: " << blk->num_allocations << "\n";
        }
        std::cout << "\n";
    }
    std::cout << "-------------------------------------------------------------------" << std::endl;
}

bool ArenaPool::AllocateBlocks_(size_t num_blocks, MemVec<size_t>& block_indices) {
    if (num_blocks == 0) return false;

    size_t total_alloc_size = num_blocks * default_block_size_;

    // Try to find contiguous blocks in free_blocks_idxs
    if (free_block_idxs_.len_ >= num_blocks) {
        // Ensure free_blocks_idxs is sorted (should be, due to Reset)
        for (size_t i = 0; i <= free_block_idxs_.len_ - num_blocks; ++i) {
            bool contiguous = true;
            size_t first_block_idx = free_block_idxs_[i];
            for (size_t j = 1; j < num_blocks; ++j) 
            {
                //check if the following free block/s are not contiguous in the blocks vector
                if (first_block_idx + j != free_block_idxs_[i + j])
                {
                    contiguous = false;
                    //- 1 accounts for the loop incrementing i also,
                    // should be next first block to check as the one that failed to be contiguous
                    i += j - 1;
                    break;
                }
            }
            if (contiguous)
            {
                // Found contiguous blocks
                
                //size_t first_block_idx = free_blocks_idxs_[i];
                Block* block = &blocks_[first_block_idx];
                //block->used = 0;
                block->size = total_alloc_size;
                //block->destruct_funcs.Clear();
                block_indices.PushBack(first_block_idx);
                // free_blocks_idxs_.Erase(free_blocks_idxs_.begin() + i, free_blocks_idxs_.begin() + i + num_blocks);
                free_block_idxs_.EraseRange(i, i + num_blocks);
                
                return true;
            }
        }
    }

    // Commit new contiguous blocks
    size_t committed_blocks = blocks_.len_;
    size_t committed_size = committed_blocks * default_block_size_;
    if (committed_size + total_alloc_size > max_size_) {
        return false;
    }

    void* ptr = CommitMemory_(base_address_ + committed_size, total_alloc_size);
    if (!ptr) {
        return false;
    }
    //committed_size_ += total_alloc_size;
    //current_address_ += total_size;

    // Create blocks
    for (size_t i = 0; i < num_blocks; ++i) 
    {
        void* block_ptr = static_cast<Byte*>(ptr) + i * default_block_size_;
        blocks_.PushBack(Block{thread_id_, block_ptr, default_block_size_, 0});
        //++committed_blocks_;
    }
    size_t new_block_idx = committed_blocks;//for readability
    block_indices.PushBack(new_block_idx);
    //total_allocated_ += total_size;
    return true;
}
