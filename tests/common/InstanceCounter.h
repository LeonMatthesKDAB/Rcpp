#pragma once

#include <cctype>


// Add a Base class and a Derived class to InstanceCounter to test polymorphic
// reference counter pointers.
class Base
{
public:
    virtual ~Base() = default;

    virtual bool isBase()
    {
        return true;
    }
};

class InstanceCounter : public Base
{
public:
    InstanceCounter()
    {
        instances++;
    }

    InstanceCounter(int initial_value)
        : InstanceCounter()
    {
        value = initial_value;
    }

    virtual ~InstanceCounter()
    {
        instances--;
    }

    InstanceCounter(const InstanceCounter &)
    {
        instances++;
        copies++;
    }

    bool isBase() override
    {
        return false;
    }

    // a marker value to differentiate between different instances
    int value = 0;

    static std::size_t instances;
    static std::size_t copies;
};

class Derived : public InstanceCounter
{
public:
    virtual ~Derived() = default;
};

#define REQUIRE_INSTANCES(X) REQUIRE(InstanceCounter::instances == (X));

