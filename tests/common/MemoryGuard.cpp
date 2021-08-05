#include "MemoryGuard.h"

#include <cstdlib>
#include <iostream>
#include <new>

#include <doctest.h>

static std::size_t allocations = 0;

// Example adapted from: https://en.cppreference.com/w/cpp/memory/new/operator_new
// replacement of a minimal set of functions:
void *operator new(std::size_t sz) // no inline, required by [replacement.functions]/3
{
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success

    if (void *ptr = std::malloc(sz)) {
        allocations++;
        return ptr;
    }

    throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void operator delete(void *ptr) noexcept
{
    std::free(ptr);
    allocations--;
}

MemoryGuard::MemoryGuard() 
{
    m_allocationsAtStart = allocations;
}

MemoryGuard::~MemoryGuard()
{
    // We cannot use REQUIRE here, as it actually does allocations, therefore failing the check
    if(m_allocationsAtStart != allocations) {
        FAIL("Memory leak! Old allocations: ", m_allocationsAtStart, " New allocations: ", allocations);
    }
}
