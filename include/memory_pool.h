#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>

template <typename T, std::size_t Capacity>
class MemoryPool
{
public:
    MemoryPool()
    {
        // build the free list — each slot points to the next
        for (std::size_t i = 0; i < Capacity - 1; ++i)
            reinterpret_cast<Slot *>(&slots_[i])->next =
                reinterpret_cast<Slot *>(&slots_[i + 1]);
        reinterpret_cast<Slot *>(&slots_[Capacity - 1])->next = nullptr;
        free_head_ = reinterpret_cast<Slot *>(&slots_[0]);
    }

    // allocate one T — returns raw memory, caller uses placement new
    T *allocate()
    {
        assert(free_head_ != nullptr && "MemoryPool exhausted");
        Slot *slot = free_head_;
        free_head_ = slot->next;
        ++allocated_;
        return reinterpret_cast<T *>(slot);
    }

    // return a T back to the pool
    void deallocate(T *ptr)
    {
        Slot *slot = reinterpret_cast<Slot *>(ptr);
        slot->next = free_head_;
        free_head_ = slot;
        --allocated_;
    }

    std::size_t allocated() const { return allocated_; }
    std::size_t available() const { return Capacity - allocated_; }
    bool full() const { return free_head_ == nullptr; }

private:
    // each free slot is reused to store the next-free pointer
    union Slot
    {
        alignas(T) std::byte storage[sizeof(T)];
        Slot *next;
    };

    Slot slots_[Capacity];
    Slot *free_head_ = nullptr;
    std::size_t allocated_ = 0;
};
