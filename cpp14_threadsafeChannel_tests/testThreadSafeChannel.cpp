//
// Created by andrea on 15/09/15.
// Uses c++11 and https://github.com/philsquared/Catch as the bdd testing framework
#include <string>
#include <future>
#include <chrono>
#include <atomic>
#include <algorithm>

#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "ThreadSafeChannel.cpp"

using namespace std;

SCENARIO("a channel transmit a copy of the message", "[channel]") {
    GIVEN("an empty channel") {
        ThreadSafeChannel<string> chan{};
        auto sink = chan.getSink();
        auto source = chan.getSource();

        WHEN("a lvalue message is passed through its sink") {
            auto inputToken = "testToken"s;
            sink(inputToken);
            THEN("its source gives back a distinct object created by copy") {
                auto outputToken = source();

                REQUIRE(inputToken == outputToken);
                REQUIRE(&inputToken != &outputToken);
            }
        }
        WHEN("a rvalue message is passed") {
            auto inputToken = "testToken"s;
            auto copyOfInputT = inputToken;
            sink(move(inputToken));
            THEN("move construction takes place") {
                auto movedToToken = source();
                CHECK(copyOfInputT == movedToToken);
                REQUIRE(inputToken == ""s);
            }
        }
    }
}

SCENARIO("a channel does not cause unacconted starvation when executed multithread ") {

    auto gen = bind(uniform_int_distribution<>{}, default_random_engine{});
    using token_t = decltype(gen());

    auto genTokens = [&gen](auto numT) {
        vector<token_t> tokens(numT);
        for (auto &t: tokens) {
            t = gen();
        }
        return tokens;
    };
    auto genProducers = [](auto sink_f, auto const tokens) {
        vector<future<void>> producers{};
        for (auto t: tokens) {
            producers.push_back(async(launch::async, [sink_f](auto token) { sink_f(token); }, t));
        }
        return producers;
    };

    GIVEN("an empty channel and a consumer") {
        ThreadSafeChannel<token_t> chan{};
        auto source = chan.getSource();
        auto sink = chan.getSink();
        auto consumer = async(launch::async, [source]() { return source(); });
        WHEN("a producer wants to write") {
            auto token = gen();
            auto producer = async(launch::async, [sink](auto token) { sink(token); }, token);
            THEN("it can") {
                REQUIRE(producer.wait_for(3000ms) == future_status::ready);
            }
            AND_THEN("the consumer receive the message") {
                REQUIRE(consumer.wait_for(3000ms) == future_status::ready);
                CHECK(consumer.get() == token);
            }
        }
    }

    GIVEN("an empty channel and multiple consumers") {
        ThreadSafeChannel<token_t> chan{};

        auto source = chan.getSource();
        auto sink = chan.getSink();

        vector<future<token_t>> consumers(51);
        WHEN("enough producers wants to write") {

            for (auto &c: consumers) {
                c = async(launch::async, [source]() {
                    return source();
                });
            }

            auto tokens = genTokens(consumers.size() + 23);
            auto producers = genProducers(sink, tokens);

            THEN("they can") {
                for (auto &p:producers) {
                    REQUIRE(p.wait_for(3000ms) == future_status::ready);
                }
            }
            AND_THEN("each consumer receive a message") {
                for (auto &c:consumers) {
                    REQUIRE(c.wait_for(3000ms) == future_status::ready);
                }
            }
            AND_THEN("each message is accounted for (be them received or still in the queue)") {
                vector<token_t> out{};
                for (auto &c:consumers) {
                    out.push_back(c.get());
                }
                for (auto i = 0; i < producers.size() - consumers.size(); ++i) {
                    out.push_back(source());
                }

                sort(tokens.begin(), tokens.end());
                sort(out.begin(), out.end());
                REQUIRE(tokens == out);
            }
        }

        WHEN("not enough producers wants to write") {
            atomic<int> done{0};

            for (auto &c: consumers) {
                c = async(launch::async, [source, &done] {
                    auto val = source();
                    ++done;
                    return val;
                });
            }

            auto tokens = genTokens(consumers.size() * 2 / 3);
            auto producers = genProducers(sink, tokens);

            THEN("some consumers will wait") {
                for (auto &p:producers) {
                    CHECK(p.wait_for(3000ms) == future_status::ready);
                }

                REQUIRE(done.load() == tokens.size());

                auto waiting = count_if(consumers.begin(), consumers.end(),
                                        [](auto &c) { return c.wait_for(20ms) == future_status::timeout; });

                REQUIRE(waiting == (consumers.size() - producers.size()));

                for (auto i = producers.size(); i < consumers.size(); ++i) {
                    sink(gen());
                }

                for (auto &c:consumers) {
                    CHECK(c.wait_for(3000ms) == future_status::ready);
                }

                REQUIRE(done.load() == consumers.size());
            }
        }
    }


}
