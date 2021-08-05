#pragma once

#include <cctype>

class MemoryGuard {
  public:
    MemoryGuard();
    
    MemoryGuard(const MemoryGuard&) = delete;
    MemoryGuard(MemoryGuard&&) = delete;

    ~MemoryGuard();
  private:
    std::size_t m_allocationsAtStart;
};
