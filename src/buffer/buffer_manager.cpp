#include "buffer_manager.hpp"

BufferManager::BufferManager()
#ifdef RTAFE_BARE_METAL
    : bare_metal_pool_{}
{
#else
{
    mem_pool_ = new MemoryPool();
#endif
}

BufferManager::~BufferManager()
{
#ifndef RTAFE_BARE_METAL
    delete mem_pool_;
#endif
}

void BufferManager::InitMemoryPool(u16 block_size, u16 num_blocks)
{
#ifdef RTAFE_BARE_METAL
    (void)block_size;
    (void)num_blocks;
    MemoryPool *mem_pool = &bare_metal_pool_;
    mem_pool->block_size = kBareMetalBlockSize;
    mem_pool->num_blocks = kBareMetalBlockCount;
    mem_pool->memory = bare_metal_memory_;
#else
    MemoryPool *mem_pool = mem_pool_;
    mem_pool->block_size = block_size;
    mem_pool->num_blocks = num_blocks;

    mem_pool->memory = malloc(block_size * num_blocks);
#endif
    mem_pool->free_list_buffer = nullptr;

    for(int i = 0; i < mem_pool->num_blocks; ++i) {
        FreeBuffer *buffer = (FreeBuffer*)((u8*)mem_pool->memory + i * mem_pool->block_size);

        /* push_front(buffer) */
        buffer->next = mem_pool->free_list_buffer;
        mem_pool->free_list_buffer = buffer;
    }
}

void *BufferManager::Alloc()
{
#ifdef RTAFE_BARE_METAL
    MemoryPool *mem_pool = &bare_metal_pool_;
#else
    MemoryPool *mem_pool = mem_pool_;
#endif
    if(mem_pool->free_list_buffer == nullptr) {
        return nullptr;
    }

    FreeBuffer *buffer = mem_pool->free_list_buffer;
    mem_pool->free_list_buffer = buffer->next;

    return (void*)buffer;
}

void BufferManager::Free(void *buffer)
{
    if(buffer == nullptr) {
        return;
    }

    FreeBuffer *free_buffer = (FreeBuffer*)buffer;

    /* push_front(free_buffer) */
#ifdef RTAFE_BARE_METAL
    MemoryPool *mem_pool = &bare_metal_pool_;
#else
    MemoryPool *mem_pool = mem_pool_;
#endif
    free_buffer->next = mem_pool->free_list_buffer;
    mem_pool->free_list_buffer = free_buffer;
}
