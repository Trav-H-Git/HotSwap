#pragma once

#include <cstddef>
#include <new>
//#include "xen_string.h"
#include <sstream>

class Memory
{
public:
	Memory() = delete;
	~Memory() = delete;

	static void* Allocate(size_t _size);
	static void* Callocate(size_t _size);
	static void DeAllocate(void* _ptr);
	template<typename TObj>
	static inline TObj* NewObject(TObj* _obj)
	{
		return _obj;
	};
	
	//static inline String GetDiagnostics()
	//{
	//	//std::ostringstream oss{};
	//	//oss << "memory allocations : " << m_alloc_count << " bytes used: " << m_bytes_uses;
	//	//return oss.str();

	//	//return String("memory allocations : " + (int)m_alloc_count + " bytes used: " + (int)m_bytes_uses);
	//	return String("memory allocations : ");
	//};

private:
	static uint32_t m_alloc_count;
	static uint32_t m_bytes_uses;
};



void* operator new(size_t size, const char* str);

#define NEW_OBJ(obj) Memory::NewObject(new ("") (obj))
#define NEW_OBJ_ARGS(obj, ...) Memory::NewObject(new ("") obj(__VA_ARGS__))
//#define ALLOC_OBJ(obj) NewObject(new ("") obj)
#define DELETE_OBJ(ptr) Memory::DeAllocate(ptr)
//#define FREE_OBJ(ptr) Memory::DeAllocate(ptr)

#define ALLOC_MEM(size) Memory::Allocate(size)
#define CALLOC_MEM(size) Memory::Callocate(size)
#define FREE_MEM(ptr) Memory::DeAllocate(ptr)

//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//#include <unordered_map>
//#include <memory>
//#include <mutex>
//
//enum class AllocatorType {
//    ARENA,
//    PAGE,
//    HEAP // Fallback to std::malloc
//};
//
///// <summary>
///// this function does this
///// param 1: does this
///// </summary>
//struct AllocationContext {
//    AllocatorType type;
//    std::string name;
//};
//
//class Allocator {
//public:
//    virtual ~Allocator() = default;
//    virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
//    virtual void Deallocate(void* ptr) = 0;
//    virtual void Reset() = 0;
//    virtual size_t GetTotalAllocated() const = 0;
//    virtual String GetName() const = 0;
//};
//
//class ArenaAllocator : public Allocator {
//public:
//    ArenaAllocator(size_t size, const std::string& name)
//        : name_(name), buffer_(new uint8_t[size]), size_(size), offset_(0) {
//    }
//
//    ~ArenaAllocator() { delete[] buffer_; }
//
//    void* allocate(size_t size, size_t alignment) override {
//        // Align the current offset
//        size_t aligned_offset = (offset_ + alignment - 1) & ~(alignment - 1);
//        if (aligned_offset + size > size_) {
//            return nullptr; // Out of memory
//        }
//        void* ptr = buffer_ + aligned_offset;
//        offset_ = aligned_offset + size;
//        return ptr;
//    }
//
//    void deallocate(void* /*ptr*/) override {
//        // No-op: Arena deallocates all at once via reset
//    }
//
//    void reset() override {
//        offset_ = 0;
//    }
//
//    size_t get_total_allocated() const override {
//        return offset_;
//    }
//
//    std::string get_name() const override {
//        return name_;
//    }
//
//private:
//    std::string name_;
//    uint8_t* buffer_;
//    size_t size_;
//    size_t offset_;
//};
//
//class PageAllocator : public Allocator {
//public:
//    struct Page {
//        uint8_t* buffer;
//        size_t offset;
//        Page* next;
//    };
//
//    PageAllocator(size_t page_size, const std::string& name)
//        : name_(name), page_size_(page_size), current_page_(nullptr) {
//        add_page();
//    }
//
//    ~PageAllocator() {
//        Page* page = current_page_;
//        while (page) {
//            Page* next = page->next;
//            delete[] page->buffer;
//            delete page;
//            page = next;
//        }
//    }
//
//    void* allocate(size_t size, size_t alignment) override {
//        if (size > page_size_) return nullptr;
//
//        Page* page = current_page_;
//        while (page) {
//            size_t aligned_offset = (page->offset + alignment - 1) & ~(alignment - 1);
//            if (aligned_offset + size <= page_size_) {
//                void* ptr = page->buffer + aligned_offset;
//                page->offset = aligned_offset + size;
//                return ptr;
//            }
//            page = page->next;
//        }
//        // No space in existing pages, add a new one
//        add_page();
//        return allocate(size, alignment);
//    }
//
//    void deallocate(void* /*ptr*/) override {
//        // No-op: Pages are deallocated in bulk
//    }
//
//    void reset() override {
//        Page* page = current_page_;
//        while (page) {
//            page->offset = 0;
//            page = page->next;
//        }
//    }
//
//    size_t get_total_allocated() const override {
//        size_t total = 0;
//        Page* page = current_page_;
//        while (page) {
//            total += page->offset;
//            page = page->next;
//        }
//        return total;
//    }
//
//    std::string get_name() const override {
//        return name_;
//    }
//
//private:
//    void add_page() {
//        Page* page = new Page;
//        page->buffer = new uint8_t[page_size_];
//        page->offset = 0;
//        page->next = current_page_;
//        current_page_ = page;
//    }
//
//    std::string name_;
//    size_t page_size_;
//    Page* current_page_;
//};
//
//class Memory {
//public:
//    static Memory& get() {
//        static Memory instance;
//        return instance;
//    }
//
//    void initialize() {
//        allocators_["default_arena"] = std::make_unique<ArenaAllocator>(1024 * 1024 * 16, "default_arena");
//        allocators_["default_page"] = std::make_unique<PageAllocator>(4096, "default_page");
//    }
//
//    void* allocate(size_t size, size_t alignment, const AllocationContext& context) {
//        std::lock_guard<std::mutex> lock(mutex_);
//        auto it = allocators_.find(context.name);
//        if (it == allocators_.end()) {
//            // Create appropriate allocator based on context
//            it = allocators_.emplace(context.name, create_allocator(context)).first;
//        }
//        return it->second->allocate(size, alignment);
//    }
//
//    void deallocate(void* ptr, const AllocationContext& context) {
//        std::lock_guard<std::mutex> lock(mutex_);
//        auto it = allocators_.find(context.name);
//        if (it != allocators_.end()) {
//            it->second->deallocate(ptr);
//        }
//    }
//
//    void reset_allocator(const std::string& name) {
//        std::lock_guard<std::mutex> lock(mutex_);
//        auto it = allocators_.find(name);
//        if (it != allocators_.end()) {
//            it->second->reset();
//        }
//    }
//    void dump_memory_stats() const {
//        std::lock_guard<std::mutex> lock(mutex_);
//        for (const auto& [name, allocator] : allocators_) {
//            printf("Allocator %s: %zu bytes allocated\n", name.c_str(), allocator->get_total_allocated());
//        }
//    }
//private:
//    std::unordered_map<std::string, std::unique_ptr<Allocator>> allocators_;
//    mutable std::mutex mutex_;
//};
//
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////             V2                    //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//
//#include <windows.h>
//#include <cstddef>
//#include <cstdint>
//#include <string>
//#include <stdexcept>
//#include <algorithm>
//#include <iostream>
//
//using String = std::string;
//
//// Base Allocator class
//class Allocator {
//public:
//    virtual ~Allocator() = default;
//    virtual void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t)) = 0;
//    virtual void Deallocate(void* ptr) = 0;
//    virtual void Reset() = 0;
//    virtual size_t GetTotalAllocated() const = 0;
//    virtual String GetName() const = 0;
//};
//
//// Virtual Memory Manager
//class VirtualMemoryManager {
//public:
//    VirtualMemoryManager(size_t total_size) {
//        // Ensure size is page-aligned (divisible by 4096)
//        size_t page_size = 4096;
//        total_size_ = (total_size + page_size - 1) & ~(page_size - 1);
//
//        // Reserve virtual address space
//        base_address_ = static_cast<uint8_t*>(
//            VirtualAlloc(nullptr, total_size_, MEM_RESERVE, PAGE_READWRITE)
//            );
//        if (!base_address_) {
//            throw std::runtime_error("Failed to reserve virtual memory");
//        }
//
//        current_offset_ = 0;
//    }
//
//    ~VirtualMemoryManager() {
//        if (base_address_) {
//            VirtualFree(base_address_, 0, MEM_RELEASE);
//        }
//    }
//
//    // Allocate a region from reserved space
//    uint8_t* AllocateRegion(size_t size) {
//        if (current_offset_ + size > total_size_) {
//            return nullptr;
//        }
//
//        uint8_t* region = base_address_ + current_offset_;
//        current_offset_ += size;
//        return region;
//    }
//
//    // Commit memory within a region
//    void* CommitMemory(uint8_t* address, size_t size) {
//        void* ptr = VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
//        if (!ptr) {
//            throw std::runtime_error("Failed to commit memory");
//        }
//        return ptr;
//    }
//
//    // Decommit memory
//    void DecommitMemory(void* address, size_t size) {
//        VirtualFree(address, size, MEM_DECOMMIT);
//    }
//
//    size_t GetTotalSize() const { return total_size_; }
//
//private:
//    uint8_t* base_address_ = nullptr;
//    size_t total_size_ = 0;
//    size_t current_offset_ = 0;
//};
//
//// Arena Allocator
//class ArenaAllocator : public Allocator {
//public:
//    ArenaAllocator(VirtualMemoryManager& vm, size_t size)
//        : memory_(vm.AllocateRegion(size)), size_(size), current_(memory_), total_allocated_(0) {
//        if (!memory_) {
//            throw std::runtime_error("Arena allocation failed");
//        }
//    }
//
//    void* Allocate(size_t size, size_t alignment) override {
//        // Align the current pointer
//        uintptr_t current_ptr = reinterpret_cast<uintptr_t>(current_);
//        uintptr_t aligned_ptr = (current_ptr + alignment - 1) & ~(alignment - 1);
//        size_t padding = aligned_ptr - current_ptr;
//
//        if (total_allocated_ + padding + size > size_) {
//            return nullptr;
//        }
//
//        // Commit memory if needed
//        if (!vm_.CommitMemory(current_, padding + size)) {
//            return nullptr;
//        }
//
//        current_ = reinterpret_cast<uint8_t*>(aligned_ptr) + size;
//        total_allocated_ += padding + size;
//        return reinterpret_cast<void*>(aligned_ptr);
//    }
//
//    void Deallocate(void* /*ptr*/) override {
//        // Arena doesn't support individual deallocation
//    }
//
//    void Reset() override {
//        vm_.DecommitMemory(memory_, total_allocated_);
//        current_ = memory_;
//        total_allocated_ = 0;
//    }
//
//    size_t GetTotalAllocated() const override {
//        return total_allocated_;
//    }
//
//    String GetName() const override {
//        return "Arena Allocator";
//    }
//
//private:
//    VirtualMemoryManager& vm_;
//    uint8_t* memory_;
//    size_t size_;
//    uint8_t* current_;
//    size_t total_allocated_;
//};
//
//// Pool Allocator
//class PoolAllocator : public Allocator {
//public:
//    PoolAllocator(VirtualMemoryManager& vm, size_t block_size, size_t num_blocks)
//        : vm_(vm), block_size_(block_size), num_blocks_(num_blocks), memory_(vm.AllocateRegion(block_size* num_blocks)), allocated_blocks_(0) {
//        if (!memory_) {
//            throw std::runtime_error("Pool allocation failed");
//        }
//        Reset();
//    }
//
//    void* Allocate(size_t size, size_t /*alignment*/) override {
//        if (size > block_size_ || !free_list_) {
//            return nullptr;
//        }
//
//        FreeBlock* block = free_list_;
//        if (!vm_.CommitMemory(block, block_size_)) {
//            return nullptr;
//        }
//
//        free_list_ = free_list_->next;
//        ++allocated_blocks_;
//        return block;
//    }
//
//    void Deallocate(void* ptr) override {
//        if (!ptr) return;
//        uint8_t* p = static_cast<uint8_t*>(ptr);
//        if (p < memory_ || p >= memory_ + block_size_ * num_blocks_) {
//            return;
//        }
//
//        FreeBlock* block = static_cast<FreeBlock*>(ptr);
//        block->next = free_list_;
//        free_list_ = block;
//        --allocated_blocks_;
//    }
//
//    void Reset() override {
//        vm_.DecommitMemory(memory_, block_size_ * num_blocks_);
//        free_list_ = nullptr;
//        for (size_t i = 0; i < num_blocks_; ++i) {
//            FreeBlock* block = reinterpret_cast<FreeBlock*>(memory_ + i * block_size_);
//            block->next = free_list_;
//            free_list_ = block;
//        }
//        allocated_blocks_ = 0;
//    }
//
//    size_t GetTotalAllocated() const override {
//        return allocated_blocks_ * block_size_;
//    }
//
//    String GetName() const override {
//        return "Pool Allocator";
//    }
//
//private:
//    struct FreeBlock {
//        FreeBlock* next;
//    };
//
//    VirtualMemoryManager& vm_;
//    size_t block_size_;
//    size_t num_blocks_;
//    uint8_t* memory_;
//    FreeBlock* free_list_;
//    size_t allocated_blocks_;
//};
//
//// Free List Allocator
//class FreeListAllocator : public Allocator {
//public:
//    FreeListAllocator(VirtualMemoryManager& vm, size_t size)
//        : vm_(vm), memory_(vm.AllocateRegion(size)), size_(size), allocated_size_(0) {
//        if (!memory_) {
//            throw std::runtime_error("FreeList allocation failed");
//        }
//        Reset();
//    }
//
//    void* Allocate(size_t size, size_t alignment) override {
//        size_t header_size = sizeof(BlockHeader);
//        size_t total_size = size + header_size;
//
//        BlockHeader* current = free_list_;
//        while (current) {
//            uintptr_t current_ptr = reinterpret_cast<uintptr_t>(current) + header_size;
//            uintptr_t aligned_ptr = (current_ptr + alignment - 1) & ~(alignment - 1);
//            size_t padding = aligned_ptr - current_ptr;
//
//            if (current->size >= total_size + padding) {
//                if (!vm_.CommitMemory(current, total_size + padding)) {
//                    return nullptr;
//                }
//
//                if (current->size > total_size + padding + header_size) {
//                    BlockHeader* new_block = reinterpret_cast<BlockHeader*>(
//                        reinterpret_cast<uint8_t*>(current) + total_size + padding
//                        );
//                    new_block->size = current->size - total_size - padding;
//                    new_block->free = true;
//                    new_block->next = current->next;
//                    new_block->prev = current->prev;
//
//                    if (current->prev) {
//                        current->prev->next = new_block;
//                    }
//                    else {
//                        free_list_ = new_block;
//                    }
//                    if (current->next) {
//                        current->next->prev = new_block;
//                    }
//                }
//                else {
//                    if (current->prev) {
//                        current->prev->next = current->next;
//                    }
//                    else {
//                        free_list_ = current->next;
//                    }
//                    if (current->next) {
//                        current->next->prev = current->prev;
//                    }
//                }
//
//                current->size = total_size + padding;
//                current->free = false;
//                allocated_size_ += total_size + padding;
//                return reinterpret_cast<void*>(aligned_ptr);
//            }
//            current = current->next;
//        }
//        return nullptr;
//    }
//
//    void Deallocate(void* ptr) override {
//        if (!ptr) return;
//        uint8_t* p = static_cast<uint8_t*>(ptr);
//        if (p < memory_ || p >= memory_ + size_) {
//            return;
//        }
//
//        BlockHeader* block = reinterpret_cast<BlockHeader*>(
//            p - sizeof(BlockHeader)
//            );
//        if (block->free) return;
//
//        block->free = true;
//        allocated_size_ -= block->size;
//
//        // Coalesce with next block
//        if (block->next && block->next->free) {
//            block->size += block->next->size;
//            block->next = block->next->next;
//            if (block->next) {
//                block->next->prev = block;
//            }
//        }
//
//        // Coalesce with previous block
//        if (block->prev && block->prev->free) {
//            block->prev->size += block->size;
//            block->prev->next = block->next;
//            if (block->next) {
//                block->next->prev = block->prev;
//            }
//            block = block->prev;
//        }
//    }
//
//    void Reset() override {
//        vm_.DecommitMemory(memory_, size_);
//        BlockHeader* initial_block = reinterpret_cast<BlockHeader*>(memory_);
//        initial_block->size = size_ - sizeof(BlockHeader);
//        initial_block->free = true;
//        initial_block->next = nullptr;
//        initial_block->prev = nullptr;
//        free_list_ = initial_block;
//        allocated_size_ = 0;
//    }
//
//    size_t GetTotalAllocated() const override {
//        return allocated_size_;
//    }
//
//    String GetName() const override {
//        return "Free List Allocator";
//    }
//
//private:
//    struct BlockHeader {
//        size_t size;
//        bool free;
//        BlockHeader* next;
//        BlockHeader* prev;
//    };
//
//    VirtualMemoryManager& vm_;
//    uint8_t* memory_;
//    size_t size_;
//    BlockHeader* free_list_;
//    size_t allocated_size_;
//};
//
//// Global allocator pointer
//static Allocator* global_allocator = nullptr;
//
//// Override global new/delete
//void* operator new(size_t size) {
//    if (!global_allocator) {
//        throw std::bad_alloc();
//    }
//    return global_allocator->Allocate(size);
//}
//
//void operator delete(void* ptr) noexcept {
//    if (global_allocator) {
//        global_allocator->Deallocate(ptr);
//    }
//}
//
//void* operator new[](size_t size) {
//    if (!global_allocator) {
//        throw std::bad_alloc();
//    }
//    return global_allocator->Allocate(size);
//}
//
//void operator delete[](void* ptr) noexcept {
//    if (global_allocator) {
//        global_allocator->Deallocate(ptr);
//    }
//}
//
//int main() {
//    try {
//        // Reserve 1 GB of virtual address space
//        VirtualMemoryManager vm(1ULL * 1024 * 1024 * 1024);
//
//        // Allocate regions for each allocator
//        ArenaAllocator arena(vm, 256 * 1024 * 1024); // 256 MB
//        PoolAllocator pool(vm, 64, 1000);           // 64-byte blocks, 1000 blocks
//        FreeListAllocator free_list(vm, 512 * 1024 * 1024); // 512 MB
//
//        // Set global allocator (for demo, use free_list)
//        global_allocator = &free_list;
//
//        // Example usage
//        void* ptr1 = arena.Allocate(128);
//        std::cout << "Arena allocated: " << ptr1 << ", Total: " << arena.GetTotalAllocated() << " bytes\n";
//
//        void* ptr2 = pool.Allocate(64);
//        std::cout << "Pool allocated: " << ptr2 << ", Total: " << pool.GetTotalAllocated() << " bytes\n";
//
//        void* ptr3 = free_list.Allocate(1024);
//        std::cout << "FreeList allocated: " << ptr3 << ", Total: " << free_list.GetTotalAllocated() << " bytes\n";
//
//        // Use overridden new
//        int* arr = new int[10];
//        std::cout << "New allocated: " << arr << "\n";
//        delete[] arr;
//
//        // Reset allocators
//        arena.Reset();
//        pool.Reset();
//        free_list.Reset();
//        std::cout << "After reset: Arena=" << arena.GetTotalAllocated()
//            << ", Pool=" << pool.GetTotalAllocated()
//            << ", FreeList=" << free_list.GetTotalAllocated() << "\n";
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error: " << e.what() << "\n";
//        return 1;
//    }
//
//    return 0;
//}
//class HybridAllocator : public Allocator {
//public:
//    HybridAllocator(VirtualMemoryManager& vm, size_t size, size_t default_chunk_size = 4 * 1024 * 1024)
//        : vm_(vm), memory_(vm.AllocateRegion(size)), size_(size), default_chunk_size_(default_chunk_size),
//        head_(nullptr), tail_(nullptr), total_allocated_(0), next_offset_(0) {
//        if (!memory_) {
//            throw std::runtime_error("HybridAllocator allocation failed");
//        }
//    }
//
//    ~HybridAllocator() {
//        Reset(); // Ensure all chunks are decommitted
//    }
//
//    void* Allocate(size_t size, size_t alignment) override {
//        // Find or create a chunk with enough space
//        ChunkHeader* chunk = head_;
//        while (chunk) {
//            if (chunk->current + size + alignment <= chunk->start + chunk->size) {
//                return AllocateInChunk(chunk, size, alignment);
//            }
//            chunk = chunk->next;
//        }
//
//        // No suitable chunk; create a new one
//        size_t chunk_size = std::max(default_chunk_size_, size + sizeof(ChunkHeader) + alignment);
//        chunk_size = (chunk_size + 4095) & ~4095; // Align to page size
//        if (next_offset_ + chunk_size > size_) {
//            return nullptr;
//        }
//
//        // Commit new chunk
//        uint8_t* chunk_start = memory_ + next_offset_;
//        if (!vm_.CommitMemory(chunk_start, chunk_size)) {
//            return nullptr;
//        }
//
//        // Initialize chunk header
//        ChunkHeader* new_chunk = reinterpret_cast<ChunkHeader*>(chunk_start);
//        new_chunk->size = chunk_size;
//        new_chunk->start = chunk_start + sizeof(ChunkHeader);
//        new_chunk->current = new_chunk->start;
//        new_chunk->next = nullptr;
//        new_chunk->prev = tail_;
//
//        // Update chunk list
//        if (tail_) {
//            tail_->next = new_chunk;
//        }
//        else {
//            head_ = new_chunk;
//        }
//        tail_ = new_chunk;
//        next_offset_ += chunk_size;
//
//        return AllocateInChunk(new_chunk, size, alignment);
//    }
//
//    void Deallocate(void* /*ptr*/) override {
//        // Individual deallocations not supported; use Reset for arena-like behavior
//    }
//
//    void Reset() override {
//        // Decommit all chunks
//        ChunkHeader* chunk = head_;
//        while (chunk) {
//            vm_.DecommitMemory(reinterpret_cast<uint8_t*>(chunk), chunk->size);
//            chunk = chunk->next;
//        }
//
//        // Reset state
//        head_ = nullptr;
//        tail_ = nullptr;
//        total_allocated_ = 0;
//        next_offset_ = 0;
//    }
//
//    size_t GetTotalAllocated() const override {
//        return total_allocated_;
//    }
//
//    String GetName() const override {
//        return "HybridAllocator";
//    }
//
//private:
//    struct ChunkHeader {
//        size_t size;          // Total size of chunk (including header)
//        uint8_t* start;       // Start of usable memory (after header)
//        uint8_t* current;     // Current bump pointer
//        ChunkHeader* next;    // Next chunk
//        ChunkHeader* prev;    // Previous chunk
//    };
//
//    void* AllocateInChunk(ChunkHeader* chunk, size_t size, size_t alignment) {
//        // Align the current pointer
//        uintptr_t current_ptr = reinterpret_cast<uintptr_t>(chunk->current);
//        uintptr_t aligned_ptr = (current_ptr + alignment - 1) & ~(alignment - 1);
//        size_t padding = aligned_ptr - current_ptr;
//
//        if (chunk->current + padding + size > chunk->start + chunk->size - sizeof(ChunkHeader)) {
//            return nullptr; // Shouldn�t happen due to prior check
//        }
//
//        chunk->current = reinterpret_cast<uint8_t*>(aligned_ptr) + size;
//        total_allocated_ += padding + size;
//        return reinterpret_cast<void*>(aligned_ptr);
//    }
//
//    VirtualMemoryManager& vm_;
//    uint8_t* memory_;
//    size_t size_;
//    size_t default_chunk_size_;
//    ChunkHeader* head_;
//    ChunkHeader* tail_;
//    size_t total_allocated_;
//    size_t next_offset_;
//};