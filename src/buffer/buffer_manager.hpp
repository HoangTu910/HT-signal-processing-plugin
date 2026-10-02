#ifndef BUFFER_POOL_HPP
#define BUFFER_POOL_HPP

#include "utils.hpp"

#define BLOCK_SIZE 256
#define NUM_BLOCKS 128

typedef struct FreeBuffer {
    // Points to the next free buffer in the pool.
    FreeBuffer *next = nullptr;
} FreeBuffer;

typedef struct MemoryPool {
    void       *memory;
    FreeBuffer *free_list_buffer;
    u16         block_size;
    u16         num_blocks;
} MemoryPool;

class BufferManager {
public:
    BufferManager();
    ~BufferManager();

    void  InitMemoryPool(u16 block_size, u16 num_blocks);
    void* Alloc();
    void  Free(void *buffer);
private:
#ifdef RTAFE_BARE_METAL
    static constexpr u16 kBareMetalBlockSize = BLOCK_SIZE * sizeof(sample_t);
    static constexpr u16 kBareMetalBlockCount = 2;
    alignas(8) u8 bare_metal_memory_[kBareMetalBlockSize * kBareMetalBlockCount];
    MemoryPool bare_metal_pool_;
#else
    MemoryPool *mem_pool_;
#endif
};

#endif /* BUFFER_POOL_HPP */