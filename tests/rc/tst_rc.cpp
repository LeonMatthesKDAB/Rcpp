
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <rcpp/rc.h>

class InstanceCounter
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

    ~InstanceCounter()
    {
        instances--;
    }

    InstanceCounter(const InstanceCounter &)
    {
        instances++;
        copies++;
    }

    // a marker value to differentiate between different instances
    int value = 0;

    static std::size_t instances;
    static std::size_t copies;
};

std::size_t InstanceCounter::instances = 0;
std::size_t InstanceCounter::copies = 0;

using namespace Rcpp;

#define REQUIRE_INSTANCES(X) REQUIRE(InstanceCounter::instances == (X));

static_assert(sizeof(Rc<int>) == sizeof(void *));

TEST_CASE("Rc")
{
    SUBCASE("Can be default constructed")
    {
        Rc<int> rc;
        REQUIRE(!rc);
    }

    SUBCASE("A single Rc destructs the reference counted entity")
    {
        {
            auto rc = make_rc<InstanceCounter>();
            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("An Rc can be copied")
    {
        {
            Rc<InstanceCounter> rc;
            REQUIRE_INSTANCES(0);
            {
                auto secondRc = make_rc<InstanceCounter>();
                Rc<InstanceCounter> thirdRc(secondRc); // copy construction
                rc = secondRc; // copy assignment

                REQUIRE(rc);
                REQUIRE(secondRc);
                REQUIRE(thirdRc);
                REQUIRE_INSTANCES(1);
            }
            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("An Rc can be moved")
    {
        {
            Rc<InstanceCounter> rc;
            REQUIRE(!rc);
            REQUIRE_INSTANCES(0);
            {
                auto secondRc = make_rc<InstanceCounter>();
                REQUIRE(secondRc);

                Rc<InstanceCounter> thirdRc(std::move(secondRc)); // move construction
                REQUIRE(thirdRc);
                REQUIRE(!secondRc);

                rc = std::move(thirdRc); // move assignment
                REQUIRE(!thirdRc);
                REQUIRE(rc);
                REQUIRE_INSTANCES(1);
            }
            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("The value of an Rc can be accessed")
    {
        auto rc = make_rc<InstanceCounter>(5);

        REQUIRE(rc->value == 5);
        REQUIRE((*rc).value == 5);
    }

    SUBCASE("Multiple Rcs refer to the same object")
    {
        auto rc = make_rc<InstanceCounter>(5);
        auto secondRc = rc;

        REQUIRE(rc->value == 5);
        REQUIRE(secondRc->value == 5);

        rc->value = 10;

        REQUIRE(rc->value == 10);
        REQUIRE(secondRc->value == 10);
    }
}
