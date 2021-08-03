#pragma once

#include <iomanip>
#include <rcpp/rc.h>
#include <type_traits>

namespace Rcpp {

template<typename T>
class Prc
{
public:
    template<typename U>
    friend class Pweak;

    template<typename Base, typename Derived>
    friend Prc<Base> static_pointer_cast(const Prc<Derived> &);

    template<typename Base, typename Derived>
    friend Prc<Base> static_pointer_cast(Prc<Derived> &&);

    template<typename Derived, typename Base>
    friend Prc<Derived> dynamic_pointer_cast(const Prc<Base> &);

    template<typename Derived, typename Base>
    friend Prc<Derived> dynamic_pointer_cast(Prc<Base> &&);

    template<typename Derived, typename Base>
    friend Rc<Derived> dynamic_base_pointer_cast(const Prc<Base> &);

    template<typename Derived, typename Base>
    friend Rc<Derived> dynamic_base_pointer_cast(Prc<Base> &&);

    Prc()
        : m_controlBlock(nullptr), m_value(nullptr)
    {
    }

    friend void swap(Prc<T> &first, Prc<T> &second) noexcept
    {
        using std::swap;

        swap(first.m_controlBlock, second.m_controlBlock);
        swap(first.m_value, second.m_value);
    }

    Prc(const Prc<T> &other) noexcept
        : m_controlBlock(other.m_controlBlock), m_value(other.m_value)
    {
        if (m_controlBlock) {
            m_controlBlock->incrementStrong();
        }
    }

    Prc(Prc<T> &&other) noexcept
        : Prc()
    {
        swap(*this, other);
    }

    Prc &operator=(Prc<T> other) noexcept
    {
        swap(*this, other);
        return *this;
    }

    Prc(const Rc<T> &rc) noexcept
        : m_controlBlock(rc.m_value), m_value(rc ? &*rc : nullptr)
    {
        if (m_controlBlock) {
            m_controlBlock->incrementStrong();
        }
    }

    Prc(Rc<T> &&rc) noexcept
        : m_controlBlock(rc.m_value), m_value(rc ? &*rc : nullptr)
    {
        rc.m_value = nullptr;
    }

    ~Prc()
    {
        reset();
    }

    void reset()
    {
        if (m_controlBlock) {
            if (m_controlBlock->decrementStrong() == 0) {
                m_value->~T();

                // all strong references destructed, remove the implicit weak
                // reference
                if (m_controlBlock->decrementWeak() == 0) {
                    delete m_controlBlock;
                }
            }
        }
        m_controlBlock = nullptr;
        m_value = nullptr;
    }

    T &operator*() const noexcept
    {
        return *m_value;
    }

    T *operator->() const noexcept
    {
        return m_value;
    }

    operator bool() const
    {
        return m_controlBlock;
    }

private:
    // for construction by ptr_cast
    Prc(RcControlBlock *controlBlock, T *value)
        : m_controlBlock(value ? controlBlock : nullptr), m_value(value)
    {
    }

    RcControlBlock *m_controlBlock;
    T *m_value;
};

template<typename T, typename... Args>
Prc<T> make_prc(Args &&...args)
{
    return make_rc<T>(std::forward<Args>(args)...);
}

template<typename Base, typename Derived>
Prc<Base> static_pointer_cast(const Prc<Derived> &other)
{
    Prc<Base> result(other.m_controlBlock, static_cast<Base *>(other.m_value));
    if (result.m_controlBlock) {
        result.m_controlBlock->incrementStrong();
    }
    return result;
}

template<typename Base, typename Derived>
Prc<Base> static_pointer_cast(Prc<Derived> &&other)
{
    Prc<Base> result(other.m_controlBlock, static_cast<Base *>(other.m_value));
    other.m_controlBlock = nullptr;
    other.m_value = nullptr;
    return result;
}

template<typename Base, typename Derived>
Prc<Base> static_pointer_cast(const Rc<Derived> &rc)
{
    return static_pointer_cast<Base>(Prc<Derived>(rc));
}

template<typename Base, typename Derived>
Prc<Base> static_pointer_cast(Rc<Derived> &&rc)
{
    return static_pointer_cast<Base>(Prc<Derived>(std::move(rc)));
}

template<typename Derived, typename Base>
Prc<Derived> dynamic_pointer_cast(const Prc<Base> &other)
{
    Prc<Derived> result(other.m_controlBlock, dynamic_cast<Derived *>(other.m_value));
    if (result.m_controlBlock) {
        result.m_controlBlock->incrementStrong();
    }
    return result;
}

template<typename Derived, typename Base>
Prc<Derived> dynamic_pointer_cast(Prc<Base> &&other)
{
    Prc<Derived> result(other.m_controlBlock, dynamic_cast<Derived *>(other.m_value));
    // only transfer the ownership of this reference if the dynamic cast was succesful
    // otherwise the other pointer will still need to decrement its ref-count
    if (result.m_controlBlock) {
        other.m_controlBlock = nullptr;
        other.m_value = nullptr;
    }
    return result;
}

template<typename Derived, typename Base>
Rc<Derived> dynamic_base_pointer_cast(const Prc<Base> &prc)
{
    auto *controlValue = dynamic_cast<RcValue<Derived> *>(prc.m_controlBlock);
    if (controlValue) {
        return Rc<Derived>(*controlValue);
    }
    return {};
}

template<typename Derived, typename Base>
Rc<Derived> dynamic_base_pointer_cast(Prc<Base> &&prc)
{
    auto *controlValue = dynamic_cast<RcValue<Derived> *>(prc.m_controlBlock);
    if (controlValue) {
        Rc<Derived> result;
        result.m_value = controlValue;

        // only move out of the prc if the cast was actually succesful.
        // Otherwise the prc will have to decrement it's own ref count, so leave it as is.
        prc.m_controlBlock = nullptr;
        prc.m_value = nullptr;
    }
    return {};
}

} // namespace Rcpp
