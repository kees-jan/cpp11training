#include "stdafx.h"

#include <gtest/gtest.h>

#include <thread>
#include <future>
#include <algorithm>
#include <map>

#include "perftest.h"

class TheWeb {
public:
    // This is scaffolding to facilitate writing test cases
    // like "this function has been called before that function"
    using Args = std::map<std::string, std::string>;
    class Events {
        // a thread-safe event queue
    public:
        struct Event {
            std::string source;
            std::string message;
            bool operator == (const Event &other) const {
                return source == other.source && message == other.message;
            }
            Args arguments;
        };
        void push(Event s) {
            std::scoped_lock guard(mutex);
            events.emplace_back(std::move(s));
        }

        int index(const Event &e) {
            std::scoped_lock guard(mutex);
            auto it = std::find(std::begin(events), std::end(events), e);
            return distance(std::begin(events), it);
        }
        auto find_iterator(const Event &e) {
            std::scoped_lock guard(mutex);
            return std::find(std::begin(events), std::end(events), e);
        }
        bool ordered(const Event &e1, const Event &e2) {
            std::scoped_lock guard(mutex);
            return find_iterator(e1) < find_iterator(e2);
        }
        const Event &find(const Event &e) {
            std::scoped_lock guard(mutex);
            auto it = std::find(std::begin(events), std::end(events), e);
            if (it == std::end(events)) throw std::invalid_argument(e.message + ", " + e.source);
            return *it;
        }

    private:
        std::recursive_mutex mutex;
        std::vector<Event> events;
    };

    mutable Events events;
    TheWeb() = default;

    auto get(std::string url) const
    {
        events.push({"get: "+url, "entry"});
        std::this_thread::sleep_for(1000_ms);
        events.push({"get: " + url, "exit" });
        return "results for " + url;
    }

    auto get(std::string url, Args args) const
    {
        events.push({ "get: " + url, "entry", std::move(args) });
        std::this_thread::sleep_for(1000_ms);
        events.push({ "get: " + url, "exit"});
        return "results for " + url + " with args";
    }

};

const std::vector<std::string> urls{
    "http://google.com", "http://yahoo.com", "http://sioux.eu"
};

namespace myasync {
    // the purpose of this namespace is to contain
    // asynchronous versions of the methods offered in
    // TheWeb.

    struct Waitable {
        void wait() {};
    };

    auto get(TheWeb &web, const std::string &url) {
		return std::async(std::launch::async, [&web, &url]() { return web.get(url); });
    }

    auto get(TheWeb &web, const std::string &url, TheWeb::Args args) {
        return web.get(url, args);
    }

    template<typename T>
    auto wait_for(const T &t) {}

	auto get_three_urls() {
		TheWeb theWeb;
		auto w1 = get(theWeb, "url1");
		auto w2 = get(theWeb, "url2");
		auto w3 = get(theWeb, "url3");

		return std::vector{ // look mom!  no template arguments!
			w1.get(),
			w2.get(),
			w3.get()
		};
	}
    template<class Urls>
    auto get_parallel(TheWeb &web, const Urls &urls)
    {
		std::vector<std::future<std::string>> futures;
		std::transform(begin(urls), end(urls), back_inserter(futures), [&web](auto url) { return get(web, url); });
		std::vector<std::string> result;
		std::transform(begin(futures), end(futures), back_inserter(result), [](auto& f) { return f.get(); });
        return result;
    }
}

TEST(AsyncTest, we_can_delegate_stuff)
{
    // TODO: tweak get_three_urls in order to start retrieving all
    // urls simultaneously
    // PURPOSE: express parallelism, use std::async
    EXPECT_GT(2 * 1000_ms, duration(&myasync::get_three_urls, 1));
}

template<typename Document>
auto spellcheck(TheWeb &theWeb, Document document) {
    auto corrected = myasync::get(theWeb, "http://spell_checker.com", { { "text", document.get() } });
    return corrected;
}

TEST(AsyncTest, we_can_wait_for_delegated_stuff)
{
    // TODO: adapt `spellcheck` so that it waits for the result of the `get` call
    // HINT: `get` should return a future, `spellcheck` should await it
    // PURPOSE: control execution over interdependent tasks
    TheWeb theWeb;
    auto corrected = spellcheck(theWeb, myasync::get(theWeb, "http://google.com"));
    myasync::wait_for(corrected);

    auto &events = theWeb.events;
    EXPECT_TRUE(events.ordered(
        { "get: http://google.com", "exit" },
        { "get: http://spell_checker.com", "entry" }));

    EXPECT_EQ("results for http://google.com",
              events.find({ "get: http://spell_checker.com", "entry" })
                    .arguments.at("text"));
}

TEST(AsyncTest, we_can_delegate_a_variable_amount_of_stuff)
{
    TheWeb theWeb;
    // TODO: tweak get_parallel in order to start retrieving all
    // urls simultaneously
    // PURPOSE: gather results from async operations
    EXPECT_GT(2 * 1000_ms, duration([&] {myasync::get_parallel(theWeb, urls); }, 1));
}

TEST(AsyncTest, DISABLED_we_can_delay_execution_till_input_is_known)
{
    TheWeb::Events events;

    const auto processing_task = [&](int size) {
        events.push({ "task: n received: " + std::to_string(size), "" });
        for (int i = 0; i != size; ++i) {
            std::this_thread::sleep_for(100_ms);
        }
        events.push({ "task returns " + std::to_string(size), "" });
        return size;
    };
    int input = 0;
    auto set_input = [&](auto i) { input = i; };

    // TODO: alter/wrap above `processing_task` and `input` so that
    // it waits for its `size` argument,
    // and that client code can wait for its return value.
    // HINT: ... accept and return a future, store the result in a promise.
    // PURPOSE: learn to use your own Async Provider

    auto result_fut = std::async(std::launch::async,
        processing_task, input);

    auto input_defined = std::async(std::launch::async, [&] {
        std::this_thread::sleep_for(1000_ms);
        events.push({ "input defined", "" });
        set_input(10);
    });

    input_defined.wait();
    const auto result = result_fut.get();
    events.push({ "return value known: " + std::to_string(result), "" });

    EXPECT_EQ(10, result);
    EXPECT_TRUE(events.ordered(
        { "input defined", "" },
        { "task: n received: 10", "" }));
    EXPECT_TRUE(events.ordered(
        { "task returns 10", "" },
        { "return value known: 10", "" }));
}

namespace myasync {
    template<typename F>
    Waitable for_n(int N, F f)
    {
        for (auto i = 0; i != N; ++i) {
           std::thread(f).detach();
        }
        return {};
    };
}

TEST(AsyncTest, DISABLED_keep_a_loop_going)
{
    // TODO: change the `myasync::for_n` function to keep
    // repeating its argument function N times, asynchronously
    // GOAL: allow the equivalent of a for-loop to be written asynchronously
    // LEVEL: ADVANCED
    // HINT: loop body and condition are now separated.
    // apparent recursion is going to be needed.
    std::atomic<int> counter{ 0 };
    std::vector<std::promise<int>> promises(10);
    std::vector<int> results;
    auto task = [&] { return std::async(std::launch::async, [&] {
        auto fut = promises[counter++].get_future();
        results.push_back(fut.get());
    }); };

    bool task_done = false;
    auto all_done = myasync::for_n(5, task);
    promises[0].set_value(100);
    promises[4].set_value(104);
    promises[2].set_value(102);
    promises[1].set_value(101);
    promises[3].set_value(103);
    wait_for(all_done);
    EXPECT_EQ(5, results.size());
    EXPECT_EQ((std::vector{ { 100, 101, 102, 103, 104 } }), results);
}