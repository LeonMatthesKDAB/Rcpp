
#include <type_traits>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <rcpp/pweak.h>

#include <common/InstanceCounter.h>
#include <common/MemoryGuard.h>

using namespace Rcpp;

#define REQUIRE_INSTANCES(X) REQUIRE(InstanceCounter::instances == (X));

TEST_CASE("Weak")
{
    SUBCASE("Can be constructed from a Prc")
    {
        MemoryGuard guard;

        Pweak<InstanceCounter> weak;
        REQUIRE(!weak.lock());

        auto rc = make_prc<InstanceCounter>();
        Pweak<InstanceCounter> secondWeak(rc); // direct construction
        weak = rc; // assignment construction

        REQUIRE(secondWeak.lock());
        REQUIRE(weak.lock());

        REQUIRE_INSTANCES(1);
    }

    SUBCASE("Does not keep the instance alive")
    {
        MemoryGuard guard;

        Pweak<InstanceCounter> weak;
        {
            auto rc = make_prc<InstanceCounter>();
            weak = rc;
            REQUIRE_INSTANCES(1);
            REQUIRE(weak.lock());
        }
        REQUIRE_INSTANCES(0);

        REQUIRE(!weak.lock());
    }

    SUBCASE("Can be locked to a Prc")
    {
        MemoryGuard guard;

        Pweak<InstanceCounter> weak;

        auto rc = make_prc<InstanceCounter>(5);
        weak = rc;
        REQUIRE_INSTANCES(1);

        auto locked = weak.lock();
        static_assert(std::is_same_v<decltype(locked), Prc<InstanceCounter>>, "Weak::lock must return an Rc");
        REQUIRE(locked);

        REQUIRE(locked->value == 5);
    }

    SUBCASE("can be copied")
    {
        MemoryGuard guard;

        Pweak<InstanceCounter> weak;
        REQUIRE_INSTANCES(0);
        {
            auto rc = make_prc<InstanceCounter>();
            Pweak<InstanceCounter> secondWeak(rc);

            Pweak<InstanceCounter> thirdWeak(secondWeak); // copy construction
            weak = thirdWeak; // copy assignment

            REQUIRE(weak.lock());
            REQUIRE(secondWeak.lock());
            REQUIRE(thirdWeak.lock());
            REQUIRE_INSTANCES(1);
        }
        REQUIRE_INSTANCES(0);

        REQUIRE(!weak.lock());
    }

    SUBCASE("can be moved")
    {
        MemoryGuard guard;

        Pweak<InstanceCounter> weak;
        REQUIRE(!weak.lock());
        {
            auto rc = make_rc<InstanceCounter>();
            Pweak<InstanceCounter> secondWeak(rc);

            Pweak<InstanceCounter> thirdWeak(std::move(secondWeak));
            REQUIRE(!secondWeak.lock());
            REQUIRE(thirdWeak.lock());

            weak = std::move(thirdWeak);
            REQUIRE(weak.lock());
            REQUIRE(!thirdWeak.lock());

            REQUIRE_INSTANCES(1);
        }
    }
}
