#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <type_traits>

#include <boost/json/src.hpp>
#include <boost/mp11.hpp>
#include <boost/describe.hpp>



namespace json = boost::json;

namespace {
    
json::value readJson( std::istream& is, json::error_code& ec )
{
    json::stream_parser p;
    for(std::string line; std::getline( is, line ); )
    {
        p.write( line, ec );
        if (ec) 
        {
            return nullptr;
        }
    }

    p.finish( ec );

    if (ec) 
    {
        return nullptr;
    }

    return p.release();
}

void writeJson( std::ostream& os, json::value const& jv, std::string* indent = nullptr )
{
    std::string indent_;
    if(! indent)
        indent = &indent_;
    switch(jv.kind())
    {
    case json::kind::object:
    {
        os << "{\n";
        indent->append(4, ' ');
        auto const& obj = jv.get_object();
        if(! obj.empty())
        {
            auto it = obj.begin();
            for(;;)
            {
                os << *indent << json::serialize(it->key()) << " : ";
                writeJson(os, it->value(), indent);
                if(++it == obj.end())
                    break;
                os << ",\n";
            }
        }
        os << "\n";
        indent->resize(indent->size() - 4);
        os << *indent << "}";
        break;
    }

    case json::kind::array:
    {
        os << "[\n";
        indent->append(4, ' ');
        auto const& arr = jv.get_array();
        if(! arr.empty())
        {
            auto it = arr.begin();
            for(;;)
            {
                os << *indent;
                writeJson( os, *it, indent);
                if(++it == arr.end())
                    break;
                os << ",\n";
            }
        }
        os << "\n";
        indent->resize(indent->size() - 4);
        os << *indent << "]";
        break;
    }

    case json::kind::string:
    {
        os << json::serialize(jv.get_string());
        break;
    }

    case json::kind::uint64:
        os << jv.get_uint64();
        break;

    case json::kind::int64:
        os << jv.get_int64();
        break;

    case json::kind::double_:
        os << jv.get_double();
        break;

    case json::kind::bool_:
        if(jv.get_bool())
            os << "true";
        else
            os << "false";
        break;

    case json::kind::null:
        os << "null";
        break;
    }

    if(indent->empty())
        os << "\n";
}

}


template<class T> 
void extract( json::object const & obj, char const * name, T & value )
{
    value = json::value_to<T>( obj.at( name ) );
}

template<
    typename T,
    typename D1 = boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_protected>,
    typename D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
    typename En = std::enable_if_t<boost::mp11::mp_empty<D2>::value> 
>
T tag_invoke( json::value_to_tag<T> const&, json::value const& v )
{
    auto const& obj = v.as_object();

    T t{};

    boost::mp11::mp_for_each<D1>([&](auto D)
    {
        extract( obj, D.name, t.*D.pointer );
    });

    return t;
}

template<
    typename T,
    typename D1 = boost::describe::describe_members<T, boost::describe::mod_public | boost::describe::mod_protected>,
    typename D2 = boost::describe::describe_members<T, boost::describe::mod_private>,
    typename En = std::enable_if_t<boost::mp11::mp_empty<D2>::value> 
>
void tag_invoke( json::value_from_tag const&, json::value& v, T const & t )
{
    auto& obj = v.emplace_object();

    boost::mp11::mp_for_each<D1>([&](auto D)
    {
        obj[ D.name ] = json::value_from( t.*D.pointer );
    });
}

class JsonFile 
{
public:
    
    template <typename T>
    static void loadJsonFile(const std::string file, T& info) 
    {
        try
        {
            auto closeFile = [](std::ifstream* f) { f->close(); };
            std::unique_ptr<std::ifstream, decltype(closeFile)> inFile(new std::ifstream(file, std::ios::in), closeFile);

            json::error_code ec;
            auto val = readJson(*inFile, ec);
            if (ec)
            {
                throw json::system_error(ec, "read json file error reason");
            }

            info = json::value_to<T>(val);
        }
        catch (std::exception&)
        {
            throw;
        }
    }

    template <typename T>
    static void saveJsonFile(const std::string file, const T& info) 
    {
        try
        {
            auto closeFile = [](std::ofstream* f) { f->close(); };
            std::unique_ptr<std::ofstream, decltype(closeFile)> outFile(new std::ofstream(file, std::ios::out | std::ios::trunc), closeFile);

            auto jv = boost::json::value_from(info);
            writeJson(*outFile, jv);
        }
        catch (std::exception&)
        {
            throw;
        }
    }
};
