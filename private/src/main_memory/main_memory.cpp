#include "base/prerequisites.h"
#include "main_memory.h"

#include <mimalloc.h>
// #include <mimalloc-override.h>
// #include <mimalloc-new-delete.h>

namespace Arieo
{

    class MiMallocMemoryAllocator
        : public Base::Memory::IAllocator
    {
    private:
        mi_heap_t* m_mi_heap = nullptr;
    public:
        MiMallocMemoryAllocator()
        {
            m_mi_heap = mi_heap_new();
        }   
        
        ~MiMallocMemoryAllocator()
        {
            mi_heap_destroy(m_mi_heap);
        }

        void* allocate(size_t bytes, size_t alignment) override
        {
            return mi_heap_malloc_aligned(m_mi_heap, bytes, alignment);
        }

        void deallocate(void* p, size_t bytes, size_t alignment) override
        {
            mi_free(p);
        }
    };

    Base::Memory::MemoryManager* MainMemory::getMainMemoryManager()
    {
        static Base::Memory::MemoryManager memory_manager;
        static MiMallocMemoryAllocator default_memory_allocator;
        
        memory_manager.m_default_memory_allocator = &default_memory_allocator;
        memory_manager.m_frame_memory_allocator = &default_memory_allocator;
        memory_manager.m_resource_memory_allocator = &default_memory_allocator;
        memory_manager.m_runtime_memory_allocator = &default_memory_allocator;
        memory_manager.m_stack_memory_allocator = &default_memory_allocator;

        return &memory_manager;
    }
}