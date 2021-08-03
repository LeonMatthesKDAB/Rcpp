#pragma once

#include <rcpp/rc.h>

namespace Rcpp {

template<typename T>
class Weak
{
public:
    Weak(const Rc<T> &strong)
        : m_value(strong.m_value)
    {
        if (m_value) {
            m_value->incrementWeak();
        }
    }

    Weak(const Weak<T> &other)
        : m_value(other.m_value)
    {
        if (m_value) {
            m_value->incrementWeak();
        }
    }

    Weak(Weak<T> &&other)
        : Weak()
    {
        swap(*this, other);
    }

    Weak()
        : m_value(nullptr) { }

    friend void swap(Weak<T> &first, Weak<T> &second) noexcept
    {
        using std::swap;

        swap(first.m_value, second.m_value);
    }

    Weak &operator=(Weak<T> other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    Weak &operator=(const Rc<T> &strong)
    {
        reset();
        m_value = strong.m_value;
        if (m_value) {
            m_value->incrementWeak();
        }
        return *this;
    }

    ~Weak()
    {
        reset();
    }

    Rc<T> lock() const noexcept
    {
        if (m_value && m_value->strong() > 0) {
            return Rc<T>(*m_value);
        } else {
            return Rc<T>();
        }
    }

    void reset()
    {
        if (m_value && m_value->decrementWeak() == 0) {
            delete m_value;
        }

        m_value = nullptr;
    }

private:
    RcValue<T> *m_value;
};

} // namespace Rcpp
