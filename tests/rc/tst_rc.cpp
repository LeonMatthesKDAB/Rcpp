
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <rcpp/rc.h>

#include <common/InstanceCounter.h>
#include <common/MemoryGuard.h>

using namespace Rcpp;

#define REQUIRE_INSTANCES(X) REQUIRE(InstanceCounter::instances == (X));

static_assert(sizeof(Rc<int>) == sizeof(void *));

TEST_CASE("Rc")
{
    SUBCASE("Can be default constructed")
    {
        MemoryGuard guard;

        Rc<int> rc;
        REQUIRE(!rc);
    }

    SUBCASE("A single Rc destructs the reference counted entity")
    {
        MemoryGuard guard;
        {
            auto rc = make_rc<InstanceCounter>();
            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("An Rc can be copied")
    {
        MemoryGuard guard;

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
        MemoryGuard guard;

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
        MemoryGuard guard;

        auto rc = make_rc<InstanceCounter>(5);

        REQUIRE(rc->value == 5);
        REQUIRE((*rc).value == 5);
    }

    SUBCASE("Multiple Rcs refer to the same object")
    {
        MemoryGuard guard;

        auto rc = make_rc<InstanceCounter>(5);
        auto secondRc = rc;

        REQUIRE(rc->value == 5);
        REQUIRE(secondRc->value == 5);

        rc->value = 10;

        REQUIRE(rc->value == 10);
        REQUIRE(secondRc->value == 10);
    }
}
