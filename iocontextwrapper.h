#pragma once

#include <memory>
#include <thread>

#include <boost/noncopyable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/executor_work_guard.hpp>


class IOContextWrapper : public boost::noncopyable
{

public:
    explicit IOContextWrapper() 
    {
        work_ = std::make_unique<WorkGuard>(ioc_.get_executor());
    }

    void run() 
    {
        if(!work_ && ioc_.stopped())
        {
            work_ = std::make_unique<WorkGuard>(ioc_.get_executor());
            ioc_.restart();
        }

        std::thread t([](auto ioc) {ioc->run(); }, &ioc_);
        t.join();
    }

    void stop() 
    {
        work_.reset();
        ioc_.stop();
    }

    boost::asio::io_context& getIOContext() 
    {
        return ioc_;
    }

    using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using WorkGuardPtr = std::unique_ptr<WorkGuard>;

private:
    boost::asio::io_context ioc_;
    WorkGuardPtr work_;
};

