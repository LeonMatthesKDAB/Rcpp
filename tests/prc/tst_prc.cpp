
#include "rcpp/rc.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <rcpp/prc.h>

#include <common/InstanceCounter.h>
#include <common/MemoryGuard.h>

using namespace Rcpp;


TEST_CASE("Prc")
{
    SUBCASE("Can be default constructed")
    {
        MemoryGuard guard;

        REQUIRE_INSTANCES(0);

        Prc<int> prc;
        REQUIRE(!prc);

        REQUIRE_INSTANCES(0);
    }

    SUBCASE("Can be constructed from an RC")
    {
        MemoryGuard guard;

        {
            REQUIRE_INSTANCES(0);

            auto rc = make_rc<InstanceCounter>(5);
            Prc<InstanceCounter> prc(rc);
            REQUIRE_INSTANCES(1);

            REQUIRE(prc);
            REQUIRE(&*prc == &*rc);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("Can be move constructed from an RC")
    {
        MemoryGuard guard;

        {
            REQUIRE_INSTANCES(0);

            auto rc = make_rc<InstanceCounter>(5);
            Prc<InstanceCounter> prc(std::move(rc));
            REQUIRE_INSTANCES(1);

            REQUIRE(prc);
            REQUIRE(!rc);
            REQUIRE(prc->value == 5);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("Can be copied")
    {
        MemoryGuard guard;

        {
            Prc<InstanceCounter> prc;
            REQUIRE_INSTANCES(0);
            {
                auto secondPrc = make_prc<InstanceCounter>();
                Prc thirdPrc(secondPrc); // copy construction
                prc = thirdPrc; // copy assignment

                REQUIRE(prc);
                REQUIRE(secondPrc);
                REQUIRE(thirdPrc);
                REQUIRE_INSTANCES(1);
            }
            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("Can be moved")
    {
        MemoryGuard guard;

        {
            Prc<InstanceCounter> prc;
            REQUIRE(!prc);
            REQUIRE_INSTANCES(0);
            {
                auto secondPrc = make_prc<InstanceCounter>();
                REQUIRE(secondPrc);

                Prc<InstanceCounter> thirdPrc(std::move(secondPrc)); // move construction
                REQUIRE(thirdPrc);
                REQUIRE(!secondPrc);

                prc = std::move(thirdPrc); // move assignment
                REQUIRE(!thirdPrc);
                REQUIRE(prc);
                REQUIRE_INSTANCES(1);
            }
            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("The value of a Prc can be accessed")
    {
        MemoryGuard guard;

        auto prc = make_prc<InstanceCounter>(5);

        REQUIRE(prc->value == 5);
        REQUIRE((*prc).value == 5);
    }

    SUBCASE("Multiple Prcs refer to the same object")
    {
        MemoryGuard guard;

        auto prc = make_prc<InstanceCounter>(5);
        auto secondPrc = prc;

        REQUIRE(prc->value == 5);
        REQUIRE(secondPrc->value == 5);

        prc->value = 10;

        REQUIRE(prc->value == 10);
        REQUIRE(secondPrc->value == 10);
    }
}

TEST_CASE("Prc casting")
{
    SUBCASE("A Prc can be static cast to it's base class")
    {
        MemoryGuard guard;

        {
            REQUIRE_INSTANCES(0);

            auto prcDerived = make_prc<InstanceCounter>();
            Prc<Base> prcBase = static_pointer_cast<Base>(prcDerived);
            Prc<Base> prcBaseMoved = static_pointer_cast<Base>(std::move(prcDerived));

            REQUIRE(prcBase);
            REQUIRE(prcBaseMoved);
            REQUIRE(!prcDerived);

            REQUIRE(!prcBase->isBase());
            REQUIRE(!prcBaseMoved->isBase());

            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("A Prc can be static cast from an Rc")
    {
        {
            REQUIRE_INSTANCES(0);

            auto rc = make_rc<InstanceCounter>();
            Prc<Base> prcBase = static_pointer_cast<Base>(rc);
            Prc<Base> prcBaseMoved = static_pointer_cast<Base>(std::move(rc));

            REQUIRE(prcBase);
            REQUIRE(prcBaseMoved);
            REQUIRE(!rc);

            REQUIRE(!prcBase->isBase());
            REQUIRE(!prcBaseMoved->isBase());

            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("A Prc can be dynamic cast")
    {
        {
            REQUIRE_INSTANCES(0);

            Prc<Base> prcBase = static_pointer_cast<Base>(make_prc<InstanceCounter>());

            REQUIRE(!prcBase->isBase());

            Prc<InstanceCounter> prcDerived = dynamic_pointer_cast<InstanceCounter>(prcBase);
            Prc<InstanceCounter> prcDerivedMoved = dynamic_pointer_cast<InstanceCounter>(std::move(prcBase));

            REQUIRE(prcDerived);
            REQUIRE(prcDerivedMoved);
            REQUIRE(!prcBase);

            REQUIRE_INSTANCES(1);
        }

        REQUIRE_INSTANCES(0);
    }

    SUBCASE("A Prc knows when a dynamic cast fails")
    {
        REQUIRE_INSTANCES(0);
        {
            auto prc = make_prc<InstanceCounter>();
            REQUIRE(!dynamic_pointer_cast<Derived>(prc));
            REQUIRE(!dynamic_pointer_cast<Derived>(std::move(prc)));
        }
        REQUIRE_INSTANCES(0);
    }

    SUBCASE("A Prc can be cast to a Rc if the type matches exactly")
    {
        REQUIRE_INSTANCES(0);
        {
            auto prc = static_pointer_cast<Base>(make_prc<InstanceCounter>());
            Rc<InstanceCounter> rc = dynamic_base_pointer_cast<InstanceCounter>(prc);
            REQUIRE(rc);

            Rc<Base> baseRc = dynamic_base_pointer_cast<Base>(prc);
            REQUIRE(!baseRc);

            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);
    }
}
