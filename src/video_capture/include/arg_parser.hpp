#ifndef __VID_ARG__
#define __VID_ARG__

#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <string>

using ArgBoolMap = std::map<std::string, bool>;
using ValidRes = std::vector<std::string>;
using ValidFps = std::map<std::string, std::vector<int>>;
using ArgStringMap = std::map<std::string, std::string>;

class ArgParser
{

public:
    ArgParser()
    {
        string_map.insert(std::make_pair(std::string("-r"), std::string("1080p")));
        string_map.insert(std::make_pair(std::string("-f"), std::string("30")));

        valid_res.push_back("wvga");
        valid_res.push_back("720p");
        valid_res.push_back("1080p");
        valid_res.push_back("2.2k");

        valid_fps.insert(std::make_pair(
            std::string("wvga"),
            std::vector<int>({15, 30, 60, 100})));
        valid_fps.insert(std::make_pair(
            std::string("720p"),
            std::vector<int>({15, 30, 60})));
        valid_fps.insert(std::make_pair(
            std::string("1080p"),
            std::vector<int>({15, 30})));
        valid_fps.insert(std::make_pair(
            std::string("2.2k"),
            std::vector<int>({15})));
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
                    if (bool_map.find(arg) != bool_map.end())
                    {
                        bool_map.at(arg) = true;
                    }
                    else if (string_map.find(arg) != string_map.end())
                    {
                        kw_flag = true;
                        key = &arg;
                    }
                    else
                    {
                        bad_option(arg);
                    }
                }
            }
            if (kw_flag == true)
                bad_keyword(args.back(), "");

            if (!is_framerate_viable())
            {
                std::string message = "Invalid framerate for resolution " + string_map.at("-r");
                throw std::invalid_argument(message);
            }
        }
    }

    std::string get_fps_value()
    {
        return string_map.at("-f");
    }
    std::string get_resolution_value()
    {
        return string_map.at("-r");
    }

private:
    ArgBoolMap bool_map;
    ArgStringMap string_map;
    ValidRes valid_res;
    ValidFps valid_fps;

    bool check_keyword(const std::string &key, const std::string &value)
    {
        if (key.compare("-r") == 0)
        {
            return check_resolution(key, value);
        }
        else if (key.compare("-f") == 0)
        {
            return check_framerate(key, value);
        }
        return false;
    }

    bool check_resolution(const std::string &key, const std::string &value)
    {
        for (auto &res : valid_res)
        {
            if (res.compare(value) == 0)
                return true;
        }
        return false;
    }

    bool check_framerate(const std::string &key, const std::string &value)
    {
        return is_number(value);
    }

    bool is_framerate_viable()
    {
        for (auto &fps : valid_fps.at(string_map.at("-r")))
        {
            if (fps == std::stoi(string_map.at("-f")))
                return true;
        }
        return false;
    }

    bool is_number(const std::string &s)
    {
        return !s.empty() && std::find_if(s.begin(),
                                          s.end(), [](unsigned char c)
                                          { return !std::isdigit(c); }) == s.end();
    }

    void bad_option(const std::string &option)
    {
        std::string message = "Unrecognize option: " + option;
        throw std::invalid_argument(message);
    }
    void bad_keyword(const std::string &key, const std::string &value)
    {
        std::string message = "Invalid keyword value pair: (" + key + ", " + value + ").";
        throw std::invalid_argument(message);
    }
};

#endif