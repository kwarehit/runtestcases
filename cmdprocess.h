#pragma once

#include <filesystem>
#include <iostream>
#include <memory>

#include <boost/process.hpp>
#include <boost/process/extend.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/regex.hpp>

#include "log.h"


namespace bp =  boost::process;
namespace fs = std::filesystem;


template <typename T>
class CmdProcess : public std::enable_shared_from_this<CmdProcess<T>>
{
public:

    using buffer_type = typename T::buffer_type;

    explicit CmdProcess(boost::asio::io_context& ioc, T& obj)
        : ioc_(ioc)
        , pipe_(ioc)
        , obj_(obj)
    {
    }


    void start(const std::string& cmd, const fs::path& startDir=fs::current_path())
    {
        try
        {
            BOOST_LOG(processLog::get()) << "start cmd: " << cmd << std::endl;
            BOOST_LOG(processLog::get()) << "work dir: " << startDir << std::endl;

            child_ = std::move(bp::child(
                  cmd

                , (bp::std_out & bp::std_err) > pipe_

                , bp::shell

                , bp::start_dir = startDir.string()

                , bp::extend::on_setup([](auto& e)
                    {
                        BOOST_LOG(processLog::get()) << "start process" << std::endl;
                    })

                , bp::extend::on_error([](auto&, const std::error_code& ec)
                    {
                        BOOST_LOG(processLog::get()) << ec.message() << std::endl;
                    })
             ));

            doRead();
        }
        catch(std::exception&)
        {
            throw;
        }
    }
    
    void stop()
    {
        try
        {
            pipe_.close();
            child_.terminate();
            obj_.stop();
        }
        catch(std::exception& e)
        {
            BOOST_LOG(processLog::get()) << e.what() << std::endl;
        }
    }

private:

    void doRead() 
    {
        try
        {
            boost::asio::async_read_until(pipe_
                , boost::asio::dynamic_buffer(buf_)
                , boost::regex("\r\n")
                , [this, self = this->shared_from_this()](const boost::system::error_code& ec, std::size_t size)
            {
                onRead(ec, size);
            });
        }
        catch(std::exception& e)
        {
            BOOST_LOG(processLog::get()) << e.what() << std::endl;
        }
    }

    void onRead(const boost::system::error_code& ec, std::size_t size)
    {
        try
        {
            if (ec)
            {
                throw std::system_error(ec, ec.message());
            }

            if (pipe_.is_open() && !buf_.empty())
            {
                auto line = buf_.substr(0, size);
                buf_.erase(0, size);

                obj_.show(line);

                doRead();
            }
        }
        catch (std::exception& e)
        {
            pipe_.close();
            obj_.stop();
            BOOST_LOG(processLog::get()) << e.what() << std::endl;
        }
    }


    boost::asio::io_context& ioc_;
    T& obj_;
    buffer_type buf_;
    bp::async_pipe pipe_;
    bp::child child_;
};

