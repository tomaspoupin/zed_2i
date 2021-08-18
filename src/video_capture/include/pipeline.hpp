#ifndef __VID_PIPELINE__
#define __VID_PIPELINE__

#include <algorithm>
#include <deque>
#include <chrono>
#include <mutex>
#include <future>
#include <sl/Camera.hpp>

class VQueue {
public:
    std::unique_ptr<sl::Mat> pop_front()
    {
        std::lock_guard(m);
        if (deque.empty())
            return;

        std::unique_ptr<sl::Mat> front = std::move(deque.front());
        deque.pop_front();
        return front;
    }
    std::unique_ptr<sl::Mat> pop_back()
    {
        std::lock_guard(m);
        if (deque.empty())
            return;

        std::unique_ptr<sl::Mat> back = std::move(deque.back());
        deque.pop_back();
        return back; 
    }
    void push_front(std::unique_ptr<sl::Mat> in)
    {
        std::lock_guard(m);
        deque.push_front(std::move(in));
    }
    void push_back(std::unique_ptr<sl::Mat> in)
    {
        std::lock_guard(m);
        deque.push_back(std::move(in));
    }

private:
    std::mutex m;
    std::deque<std::unique_ptr<sl::Mat> > deque;
};

#endif