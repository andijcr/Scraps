//
// Created by andrea on 15/09/15.
//

#include <mutex>
#include <deque>
#include <condition_variable>

using namespace std;

template<typename T>
class ThreadSafeChannel {
public:
    ThreadSafeChannel() noexcept = default;

    ThreadSafeChannel(const ThreadSafeChannel&) = delete;
    ThreadSafeChannel & operator=(const ThreadSafeChannel&) = delete;
    ThreadSafeChannel(ThreadSafeChannel&&) = delete;
    ThreadSafeChannel & operator=(ThreadSafeChannel&&) = delete;

    auto getSink(){
        return [this](T message){ put(std::move(message));};
    };

    auto getSource(){
        return [this](){ return get();};
    }

private:
    mutex mtx{};
    condition_variable cond{};
    deque<T> ch{};

    void put(T&& val){
        lock_guard<mutex> lck(mtx);
        ch.push_front(val);
        cond.notify_one();
    }


    T get(){
        unique_lock<mutex> lck(mtx);
        cond.wait(lck,[this]{ return !ch.empty(); });
        auto val=ch.back();
        ch.pop_back();
        return val;
    }
};
