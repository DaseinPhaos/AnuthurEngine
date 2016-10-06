#include "Allocator.h"

Luxko::MemAlloc::ChunkAllocatorImpl<256> Luxko::MemAlloc::ChunkAllocators::alloc16Impl = Luxko::MemAlloc::ChunkAllocatorImpl<256>();
Luxko::MemAlloc::ChunkAllocatorImpl<64> Luxko::MemAlloc::ChunkAllocators::alloc64Impl = Luxko::MemAlloc::ChunkAllocatorImpl<64>();
Luxko::MemAlloc::ChunkAllocatorImpl<16> Luxko::MemAlloc::ChunkAllocators::alloc256Impl = Luxko::MemAlloc::ChunkAllocatorImpl<16>();
Luxko::MemAlloc::ChunkAllocatorImpl<4> Luxko::MemAlloc::ChunkAllocators::alloc1024Impl = Luxko::MemAlloc::ChunkAllocatorImpl<4>();