#include <gtest/gtest.h>
#include "memory_pool.h"
#include "order.h"

TEST(MemoryPool, AllocateAndDeallocate)
{
    MemoryPool<Order, 64> pool;
    EXPECT_EQ(pool.available(), 64u);

    Order *o = pool.allocate();
    EXPECT_NE(o, nullptr);
    EXPECT_EQ(pool.available(), 63u);
    EXPECT_EQ(pool.allocated(), 1u);

    pool.deallocate(o);
    EXPECT_EQ(pool.available(), 64u);
    EXPECT_EQ(pool.allocated(), 0u);
}

TEST(MemoryPool, AllocateMany)
{
    MemoryPool<Order, 8> pool;
    Order *ptrs[8];
    for (int i = 0; i < 8; ++i)
        ptrs[i] = pool.allocate();

    EXPECT_TRUE(pool.full());
    EXPECT_EQ(pool.allocated(), 8u);

    for (int i = 0; i < 8; ++i)
        pool.deallocate(ptrs[i]);

    EXPECT_EQ(pool.available(), 8u);
}

TEST(MemoryPool, ReuseAfterDeallocate)
{
    MemoryPool<Order, 4> pool;
    Order *o1 = pool.allocate();
    pool.deallocate(o1);
    Order *o2 = pool.allocate();
    // should get the same slot back
    EXPECT_EQ(o1, o2);
    pool.deallocate(o2);
}

TEST(MemoryPool, PointersAreAligned)
{
    MemoryPool<Order, 16> pool;
    Order *o = pool.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(o) % alignof(Order), 0u);
    pool.deallocate(o);
}