#pragma once

#include <rcpp/rc.h>
#include <rcpp/prc.h>

namespace Rcpp {

template<typename T>
class Pweak
{
public:
    Pweak()
        : m_controlBlock(nullptr), m_value(nullptr)
    {
    }

    Pweak(const Prc<T> &prc)
        : m_controlBlock(prc.m_controlBlock), m_value(prc.m_value)
    {
        if (m_controlBlock) {
            m_controlBlock->incrementWeak();
        }
    }

    Pweak(const Pweak<T> &other)
        : m_controlBlock(other.m_controlBlock), m_value(other.m_value)
    {
        if (m_controlBlock) {
            m_controlBlock->incrementWeak();
        }
    }

    Pweak(Pweak<T> &&other)
        : Pweak()
    {
        swap(*this, other);
    }

    friend void swap(Pweak<T> &first, Pweak<T> &second) noexcept
    {
        using std::swap;

        swap(first.m_controlBlock, second.m_controlBlock);
        swap(first.m_value, second.m_value);
    }

    Pweak &operator=(Pweak<T> other)
    {
        swap(*this, other);
        return *this;
    }

    Pweak &operator=(const Prc<T> &prc)
    {
        reset();
        m_controlBlock = prc.m_controlBlock;
        m_value = prc.m_value;
        if (m_controlBlock) {
            m_controlBlock->incrementWeak();
        }
        return *this;
    }

    ~Pweak()
    {
        reset();
    }

    Prc<T> lock() const noexcept
    {
        if (m_controlBlock && m_controlBlock->strong()) {
            Prc<T> result(m_controlBlock, m_value);
            if (result.m_controlBlock) {
                result.m_controlBlock->incrementStrong();
            }
            return result;
        }

        return {};
    }

    void reset()
    {
        if (m_controlBlock) {
            if (m_controlBlock->decrementWeak() == 0) {
                delete m_controlBlock;
            }
        }
        m_controlBlock = nullptr;
        m_value = nullptr;
    }

private:
    RcControlBlock *m_controlBlock;
    T *m_value;
};

} // namespace Rcpp
