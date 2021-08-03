
#include <type_traits>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <rcpp/rc.h>
#include <rcpp/weak.h>

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

TEST_CASE("Weak")
{
    SUBCASE("Can be constructed from an Rc")
    {
        Weak<InstanceCounter> weak;
        REQUIRE(!weak.lock());

        auto rc = make_rc<InstanceCounter>();
        Weak<InstanceCounter> secondWeak(rc); // direct construction
        weak = rc; // assignment construction

        REQUIRE(secondWeak.lock());
        REQUIRE(weak.lock());

        REQUIRE_INSTANCES(1);
    }

    SUBCASE("Does not keep the instance alive")
    {
        Weak<InstanceCounter> weak;
        {
            auto rc = make_rc<InstanceCounter>();
            weak = rc;
            REQUIRE_INSTANCES(1);
            REQUIRE(weak.lock());
        }
        REQUIRE_INSTANCES(0);

        REQUIRE(!weak.lock());
    }

    SUBCASE("Can be locked to an Rc")
    {
        Weak<InstanceCounter> weak;

        auto rc = make_rc<InstanceCounter>(5);
        weak = rc;
        REQUIRE_INSTANCES(1);

        auto locked = weak.lock();
        static_assert(std::is_same_v<decltype(locked), Rc<InstanceCounter>>, "Weak::lock must return an Rc");
        REQUIRE(locked);

        REQUIRE(locked->value == 5);
    }

    SUBCASE("can be copied")
    {
        Weak<InstanceCounter> weak;
        REQUIRE_INSTANCES(0);
        {
            auto rc = make_rc<InstanceCounter>();
            Weak<InstanceCounter> secondWeak(rc);

            Weak<InstanceCounter> thirdWeak(secondWeak); // copy construction
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
        Weak<InstanceCounter> weak;
        REQUIRE(!weak.lock());
        {
            auto rc = make_rc<InstanceCounter>();
            Weak<InstanceCounter> secondWeak(rc);

            Weak<InstanceCounter> thirdWeak(std::move(secondWeak));
            REQUIRE(!secondWeak.lock());
            REQUIRE(thirdWeak.lock());

            weak = std::move(thirdWeak);
            REQUIRE(weak.lock());
            REQUIRE(!thirdWeak.lock());

            REQUIRE_INSTANCES(1);
        }
    }
}
