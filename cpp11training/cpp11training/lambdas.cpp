#include "stdafx.h"
#include "gtest/gtest.h"

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <exception>
#include <algorithm>

//////// DON'T TOUCH THIS
// you should override these with local lambdas
// don't touch these functions...
void foo() { throw std::runtime_error{ "not implemented" }; }
int bar() { return 1; }
void baz(int) { }
int length(const std::string &s) { return 100; }
///////////////////////////

TEST(lambdas, DISABLED_we_can_define_local_lambdas)
{
    // TODO: create a lambda that returns the length
    // of a string, and a `foo` that does just nothing
    //
    // GOAL: get acquainted with the lambda expression syntax
    EXPECT_EQ(3, length("abc"));
    EXPECT_NO_THROW(foo());
}

TEST(lambdas, DISABLED_we_can_capture_the_local_variables_by_value)
{
    std::vector<std::function<int(int)>> multipliers;

    for (int i = 0; i != 10; ++i)
    {
        // TODO: alter the lambda represent a different
        // calculation on every iteration, so that we
        // end up with an array of multiplying functions
        // \n -> n * i
        //
        // GOAL: learn that lambda expressions can be used
        // to create functions on-the-fly!

        multipliers.push_back([](int x) { return 0; });
    }

    EXPECT_EQ(1, multipliers[1](1));
    EXPECT_EQ(2, multipliers[1](2));

    EXPECT_EQ(2, multipliers[2](1));
    EXPECT_EQ(9, multipliers[3](3));
}

TEST(lambdas, DISABLED_we_can_capture_local_variables_by_reference)
{
    int receiver = 0;
    // TODO: create a local lambda expression `baz` that
    // adds the argument to `receiver`;
    //
    // GOAL: learn about capture modes
    //
    const std::vector<int> ints{ {10, 20, 30, 40, 50} };
    std::for_each(begin(ints), end(ints), baz);
    EXPECT_EQ(150, receiver);
}


TEST(lambdas, DISABLED_we_can_add_state)
{
    // TODO:
    // create a lambda expression `foo` that
    // stores a different value into `foo_calls` upon subsequent calls
    //
    // HINT: lambda expressions are immutable, unless...
    //
    // WHY?  this is sometimes needed in tests to e.g. record the
    // number of times something was called
    //
    EXPECT_EQ(1, bar());
    EXPECT_EQ(2, bar());
}

TEST(lambdas, DISABLED_we_can_add_state2)
{
    // sum of lengths
    //
    // TODO: define a lambda `add_length` that returns
    // the length of all the strings that have been given to it
    auto add_length = [](std::string s) { return 0; };
    EXPECT_EQ(3, add_length("abc"));
    EXPECT_EQ(7, add_length("efgh"));
}


TEST(lambdas, DISABLED_we_can_add_state3)
{
    // average length
    //
    // TODO: define a lambda `add_length` that returns the average
    // length of all the strings that have been given to it
    auto add_length = [](std::string s) { return 0.0; };

    EXPECT_NEAR(3.000, add_length("abc"), 0.01);
    EXPECT_NEAR(3.500, add_length("efgh"), 0.01);
    EXPECT_NEAR(3.333, add_length("ijk"), 0.01);
}


TEST(lambdas, DISABLED_we_can_add_state4)
{
    std::vector<int> ints;
    // TODO: create a lambda that returns the next square
    // on every call
    //
    auto squares = [] { return -1; };
    std::generate_n(std::back_inserter(ints), 10, squares);
    EXPECT_EQ(9, ints.at(2));
    EXPECT_EQ(100, ints.at(9));
}


#include "class_design/Thing.h"
#include "class_design/MyBike.h"
#include "class_design/Piano.h"

// TODO: implement this `make` factory with a map<key, factory function>
// (cf. the next test)
std::unique_ptr<Thing> make(const std::string &what)
{
    using Constructor = std::function<std::unique_ptr<Thing>()>;
    static std::map<std::string, Constructor> constructors;
    return constructors.at(what)();
}

TEST(lambdas, DISABLED_we_can_instantiate_based_on_a_typename)
{
    // TODO: alter the `make` function to return an object
    // of the type specified by the provided argument.
    //
    // GOAL: demonstrate that lambdas can be stored in containers
    //
    // HINT: a std::map is great for lookup, but is not polymorphic.
    // a std::function can wrap any lambda expression
    auto f = make("my bike");
    EXPECT_FALSE(dynamic_cast<MyBike*>(f.get()) == nullptr);
    auto i = make("piano");
    EXPECT_FALSE(dynamic_cast<Piano*>(i.get()) == nullptr);
}


std::function<double(double)> make_adder(double operand)
{
    return std::function<double(double)>{};
}

TEST(lambdas, DISABLED_we_can_instantiate_functions_at_runtime)
{
    // TODO: alter make_adder so that it can be used
    // to create different functions, depending on the argument
    //
    // GOAL: an entry-level functional programming exercise
    //
    auto add5 = make_adder(5.0);
    EXPECT_NEAR(5.0, add5(0.0), 0.001);
    EXPECT_NEAR(15.0, add5(10.0), 0.001);
    EXPECT_NEAR(20.0, make_adder(-10.0)(30.0), .001);
}

TEST(lambdas, DISABLED_we_can_bind_arguments)
{
    auto add5 = std::plus<int>();
    //TODO: #define we_can_bind_an_argument
    // and define add5 with std::bind
    //
    // HINT: use a placeholder!
    // GOAL: lay the basis for expression trees; provide
    // an alternative to plain lambdas
    //
#ifdef we_can_bind_an_argument
    EXPECT_EQ(5, add5(0));
    EXPECT_EQ(15, add5(10));
#endif
}

std::function<double(double)> make_safe(
    std::function<double(double)> unsafe_function,
    std::function<bool(double)> check)
{
    return std::function<double(double)>{};
}

TEST(lambdas, DISABLED_we_can_add_a_policy_to_a_function)
{
    const auto reciproc = [](double f) { return 1. / f; };

    const auto &safe_reciproc = make_safe(reciproc, [&](double f) {
        return (f != 0.0);
    });
    // TODO: alter `make_safe` so that it throws runtime_error when
    // its policy returns false, and otherwise calls the provided function
    EXPECT_NEAR(2.0, safe_reciproc(0.5), 1e-5);
    EXPECT_THROW(safe_reciproc(0.0), std::runtime_error);
}


#if __cplusplus >= 201402L // but msvc has __cplusplus still defined to 199711L...
// https://blogs.msdn.microsoft.com/vcblog/2018/04/09/msvc-now-correctly-reports-__cplusplus/
// https://connect.microsoft.com/VisualStudio/feedback/details/763051/a-value-of-predefined-macro-cplusplus-is-still-199711l
#include <algorithm>

TEST(lambdas, DISABLED_you_dont_have_to_specify_the_argument_types_in_cpp14)
{
    auto stringify = [](auto i) {
        return std::to_string(i);
    };
    {
        EXPECT_EQ("1", stringify(1));
    }
    {
        EXPECT_EQ(std::to_string(1.0), stringify(1.0));
    }
}
#endif

// MORE ADVANCED:
// revisit use_auto.cpp: implement TEST(find_number, DISABLED_we_can_just_auto_translate)
// with lambda expressions