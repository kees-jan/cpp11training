#include <gtest/gtest.h>
#include <string>
#include <vector>

// TODO: alter a function template to return the number of arguments passed.
// GOAL: be able to specify a variadic function template and use the sizeof... operator
// GRADE: ENTRY
#define i_can_specify_a_variadic_function_template
#ifdef i_can_specify_a_variadic_function_template
template<typename... Ts>
size_t count_args(Ts... Vs) { return sizeof...(Ts); }

TEST(variadic, we_can_count_arguments)
{
    EXPECT_EQ(2u, count_args(1, 2));
    EXPECT_EQ(3u, count_args('one', 2.0, 3));
}
#endif

// TODO: alter the class template to know its number of arguments
// GOAL: be able to specify a variadic class template and use the sizeof... operator
// GRADE: ENTRY
#define i_can_specify_a_variadic_class_template
#ifdef i_can_specify_a_variadic_class_template

template<typename... Ts>
struct CountArgs
{
    static constexpr size_t value = sizeof...(Ts);
};

TEST(variadic, we_can_count_arguments2)
{
    EXPECT_EQ(2u, (CountArgs<int, bool>::value));
    EXPECT_EQ(3u, (CountArgs<int, char, long>::value));
}
#endif

namespace {

	//template<int n, typename T>
	//struct add_five_helper
	//{
	//	static void help(T& t)
	//	{
	//		std::get<n>(t) += 5;
	//		add_five_helper<n - 1, T>::help(t);

	//	}

	//};

	//template<typename T>
	//struct add_five_helper<0, T>
	//{
	//	static void help(T& in)
	//	{
	//	}
	//};

	//template<typename T>
	//T add5(T t)
	//{
	//	add_five_helper<std::tuple_size<T>::value-1, T>::help(t);
	//	return t;
	//}

	//auto add5 = [](auto t) { return t; };

	template <class Tuple, size_t... Is>
	constexpr auto add5_impl(Tuple t, std::index_sequence<Is...>) 
	{
		return std::make_tuple((std::get<Is>(t)+5)...);
	}

	template <class Tuple>
	constexpr auto add5(Tuple t) {
		return add5_impl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
	}

}
TEST(tuples, i_can_transform_all_elements_of_a_tuple) {
    // TODO: make `add5` process each element of the `input` tuple
    // to generate a new tuple where each element is 5 bigger
    // GOAL: learn to use pack expansion in function arguments
    // LEVEL: BASIC
    // HINT: return types can be `auto`
    // HINT: make_tuple deduces its template arguments
    const auto input = std::make_tuple(5, 5.0, 'A');
    const auto &result = add5(input);
    EXPECT_EQ(10, std::get<0>(result));
    EXPECT_EQ(10.0, std::get<1>(result));
    EXPECT_EQ('F', std::get<2>(result));
}

namespace {

	std::string serialize_one(int i)
	{
		std::stringstream ss;
		ss << "int@" << i;
		return ss.str();
	}

	std::string serialize_one(std::string const& s)
	{
		std::stringstream ss;
		ss << "string@'" << s << "'";
		return ss.str();
	}

	std::string join(std::string separator)
	{
		return std::string();
	}

	std::string join(std::string separator, std::string element)
	{
		return element;
	}

	template<typename... Ts>
	std::string join(std::string separator, std::string first, Ts... rest)
	{
		return first + separator + join(separator, rest...);
	}


    template<typename ...Ts>
    std::string serialize(Ts ...ts) 
	{
		return join(", ", serialize_one(ts)...);
    }
}
TEST(serialization, serialize_different_types)
{
    // TODO: fill in serialize so that the test passes
    // GOAL: get familiar with template parameter packs
    // GOAL: learn how to expand a (single) parameter pack
    // GRADE: INTERMEDIATE
    EXPECT_EQ("", serialize());
    EXPECT_EQ("int@1", serialize(1));
    EXPECT_EQ("string@'abc'", serialize(std::string("abc")));
    EXPECT_EQ("int@1, string@'abc'", serialize(1, std::string("abc")));
}

// TODO: implement flatten to allow polymorphic concatenation
// GOAL: learn to separate iteration from dispatching
// HINT: the base case is a single vector
// GRADE: INTERMEDIATE

template<typename... Ts>
void flatten_add(std::vector<int>& result, int i, Ts... ts)
{
	result.push_back(i);
	if constexpr (0<sizeof...(ts))
		flatten_add(result, ts...);
}

template<typename... Ts>
void flatten_add(std::vector<int>& result, std::vector<int> v, Ts... ts)
{
	copy(begin(v), end(v), back_inserter(result));
	if constexpr (0 < sizeof...(ts))
		flatten_add(result, ts...);
}

template<typename ...Ts>
auto flatten(Ts... ts) {
	std::vector<int> result;
	flatten_add(result, ts...);
    return result;
}
TEST(variadic_monad, flatten_operation)
{
    EXPECT_EQ((std::vector<int>{ 1, 2, 3, 4, 5 }),
        (flatten(std::vector<int>({ 1 }), 2, std::vector<int>{ 3, 4, 5 })));
}


// TODO: adapt `contained_by` to return a callable
// representing a predicate for presence in a compile-time list
// HINT: contained_by()( anything ) will return false.
// GRADE: INTERMEDIATE
template<typename T1, typename T2>
bool matches(T1 v1, T2 v2)
{
	return false;
}

template<typename T>
bool matches(T v1, T v2)
{
	return v1 == v2;
}

template<typename ...Ts>
class ContainedBy
{
public:
	ContainedBy(Ts... ts)
		: all(std::make_tuple(ts...))
	{}

	template<typename T>
	bool operator()(T v)
	{
		return match_impl(v, std::make_index_sequence<sizeof...(Ts)>());
	}

	template <typename T>
	constexpr auto match_impl(T t, std::index_sequence<>)
	{
		return false; 
	}

	template <typename T, size_t I, size_t... Is>
	constexpr auto match_impl(T t, std::index_sequence<I, Is...> sequence)
	{
		return  matches(t, std::get<I>(all)) ||
			match_impl(t, std::index_sequence<Is...>());
	}

	template <class Tuple>
	constexpr auto add5(Tuple t) {
		return ;
	}

private:
	std::tuple<Ts...> all;
};

template<typename ...Ts>
auto contained_by(Ts&&...ts)
{
	return ContainedBy(ts...);
}

TEST(variadic_templates, create_a_compile_time_list_lookup)
{
    auto in_list = contained_by(1, 2, 3, 4);
    EXPECT_TRUE(in_list(1));
    EXPECT_FALSE(in_list(0));
	EXPECT_FALSE(in_list(1.0));
}

template<typename F, typename A, typename T, typename ...Ts>
auto accumulate(F f, A a, T t, Ts...ts) {
    return a;
}

TEST(variable_templates, DISABLED_we_can_accumulate) {
    // TODO: create the `accumulate` function template so that
    // it behaves like a compile time accumulation
    // GOAL: create an ingredient for further good
    // LEVEL: INTERMEDIATE
    EXPECT_EQ(10, accumulate(std::plus<>(), 0,   1, 8, 1));
    EXPECT_EQ(10, accumulate(std::plus<>(), 0,   1, 2, 3, 4));
}

template<typename F, typename T>
auto transform(F f, T t) {
    return t;
};

TEST(variadic_tuple_iteration, DISABLED_we_can_transform_an_indexed_tuple) {
    // TODO: create a `transform` function to convert each
    // element of the provided tuple to a corresponding element
    // of a new tuple
    // GOAL: learn to construct a 'helper' argument.  This shows that
    // tuples are Functors, too.
    // HINT: this tuple has elements of the same type.
    // simply iterating using the Type doesn't work anymore.
    // So we need a helper structure/accumulator/loop variable to keep track
    // of the element index we're looking at.
    //
    const auto input = std::make_tuple(1, 2, 3);
    const auto f = [](auto i) { return i + 1; };
    const auto result = transform(f, input);
    EXPECT_EQ(2, std::get<0>(result));
    EXPECT_EQ(3, std::get<1>(result));
    EXPECT_EQ(4, std::get<2>(result));
}
auto product = [](auto... functions)
{
    return [](auto ...arguments) {
        return std::string{ "not implemented" };
    };
};

TEST(composition, DISABLED_print_a_matrix)
{

    // this exercise will take some more time...
    // 
    // TODO: fill in the `product` function so that it prints a table
    // of the functions applied to the arguments
    // GOAL: learn to deal with multiple packs and expansions
    // GRADE: HARD
    const auto table = product(
        [](auto i) { return i; },
        [](auto i) { return i * i; },
        [](auto i) { return i * i*i - 1; }
    )
    (1, 2, 3);
    EXPECT_EQ(R"(1, 2, 3
1, 4, 9
0, 7, 26)", table);

    EXPECT_EQ(R"(1, 4, 9, 100
1, 8, 27, 1000)",
    product([](auto i) { return i*i; },
            [](auto i) { return i*i*i; })
    (1, 2, 3, 10));
}
