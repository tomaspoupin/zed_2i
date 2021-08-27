#ifndef __DEPTH_ARG__
#define __DEPTH_ARG__

#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <string>

using ArgStringMap = std::map<std::string, std::string>;

class ArgParser
{

public:
    ArgParser()
    {
        string_map.insert(std::make_pair(std::string("-f"), std::string("")));
    }

    void parse(int argc, char *argv[])
    {
        std::vector<std::string> args;

        if (argc > 1)
        {
            args.assign(argv + 1, argv + argc);
            bool kw_flag = false;
            std::string *key = nullptr;
            for (auto &arg : args)
            {
                if (kw_flag)
                {
                    if (check_keyword(*key, arg))
                    {
                        string_map.at(*key) = arg;
                    }
                    else
                        bad_keyword(*key, arg);

                    kw_flag = false;
                    key = nullptr;
                }
                else
                {
                    if (string_map.find(arg) != string_map.end())
                    {
                        kw_flag = true;
                        key = &arg;
                    }
                    else
                    {
                        std::string message = "Invalid option: " + arg;
                        throw std::invalid_argument(message);
                    }
                }
            }
            if (kw_flag == true)
                bad_keyword(args.back(), "");
        }
        else 
        {
            throw std::invalid_argument("Usage -> playback -f <filename>");
        }
    }

    std::string get_filename()
    {
        return string_map.at("-f");
    }

private:
    ArgStringMap string_map;

    bool check_keyword(const std::string &key, const std::string &value)
    {
        if (key.compare("-f") == 0)
        {
            if (value.compare("") != 0)
                return true;
        }
        return false;
    }

    void bad_keyword(const std::string &key, const std::string &value)
    {
        std::string message = "Invalid keyword value pair: (" + key + ", " + value + ").";
        throw std::invalid_argument(message);
    }
};

#endif