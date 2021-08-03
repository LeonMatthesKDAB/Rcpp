#pragma once

#include <cstddef>
#include <utility>

namespace Rcpp {

namespace {

class RcControlBlock
{
    size_t m_weak = 0;
    size_t m_strong = 0;

public:
    virtual ~RcControlBlock() = default;

    size_t incrementStrong()
    {
        return ++m_strong;
    }

    size_t incrementWeak()
    {
        return ++m_weak;
    }

    size_t decrementStrong()
    {
        return --m_strong;
    }

    size_t decrementWeak()
    {
        return --m_weak;
    }

    size_t strong()
    {
        return m_strong;
    }

    size_t weak()
    {
        return m_weak;
    }
};

template<typename T>
class RcValue : public RcControlBlock
{
    struct Empty {
    };

    union RcContent {
        template<typename... Args>
        RcContent(Args &&...args)
            : value(std::forward<Args>(args)...)
        {
        }

        ~RcContent() { }

        T value;
        Empty empty;
    } m_content;

public:
    template<typename... Args>
    RcValue(Args &&...args)
        : RcControlBlock(), m_content(std::forward<Args>(args)...)
    {
    }

    virtual ~RcValue() = default;

    T &content() noexcept
    {
        return m_content.value;
    }

    void destructContent()
    {
        m_content.value.~T();
    }
};

} // namespace

// forward declaration necessary for friend declarations
template<typename T>
class Prc;

template<typename T>
class Rc
{
public:
    template<typename U, typename... Args>
    friend Rc<U> make_rc(Args &&...args);

    template<typename U>
    friend class Weak;

    template<typename U>
    friend class Prc;

    template<typename Derived, typename Base>
    friend Rc<Derived> dynamic_base_pointer_cast(const Prc<Base> &);

    template<typename Derived, typename Base>
    friend Rc<Derived> dynamic_base_pointer_cast(Prc<Base> &&);

    ~Rc()
    {
        reset();
    }

    Rc() noexcept
        : m_value{ nullptr }
    {
    }

    Rc(Rc<T> &&other) noexcept
        : Rc()
    {
        swap(*this, other);
    }

    Rc(const Rc<T> &other) noexcept
        : m_value(other.m_value)
    {
        if (m_value) {
            m_value->incrementStrong();
        }
    }

    friend void swap(Rc<T> &first, Rc<T> &second) noexcept
    {
        using std::swap;

        swap(first.m_value, second.m_value);
    }

    Rc &operator=(Rc<T> other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    void reset()
    {
        if (m_value) {
            if (m_value->decrementStrong() == 0) {
                m_value->destructContent();

                // all strong references destructed, remove the
                // implicit weak reference
                if (m_value->decrementWeak() == 0) {
                    delete m_value;
                }
            }
        }
        m_value = nullptr;
    }

    T &operator*() const noexcept
    {
        return m_value->content();
    }

    T *operator->() const noexcept
    {
        return &m_value->content();
    }

    operator bool() const noexcept
    {
        return m_value;
    }

private:
    RcValue<T> *m_value;

    Rc(RcValue<T> &value)
        : m_value(&value)
    {
        m_value->incrementStrong();
    }
};

template<typename T, typename... Args>
Rc<T> make_rc(Args &&...args)
{
    auto *value = new RcValue<T>(std::forward<Args>(args)...);
    // we add one implicit weak reference for all strong references,
    // so the weak destructor doesn't run the control block destructor
    // if another strong pointer exists
    value->incrementWeak();

    return Rc<T>(*value);
}

} // namespace Rcpp
