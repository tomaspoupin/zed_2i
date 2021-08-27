#ifndef __DEPTH_ARG__
#define __DEPTH_ARG__

#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <string>

using ValidDepth = std::vector<std::string>;
using ValidUnit = std::vector<std::string>;
using ValidSensing = std::vector<std::string>;
using ValidGui = std::vector<std::string>;
using ArgStringMap = std::map<std::string, std::string>;

class ArgParser
{

public:
    ArgParser()
    {
        string_map.insert(std::make_pair(std::string("-u"), std::string("milli")));
        string_map.insert(std::make_pair(std::string("-d"), std::string("ultra")));
        string_map.insert(std::make_pair(std::string("-s"), std::string("standard")));
        string_map.insert(std::make_pair(std::string("-g"), std::string("off")));

        valid_depth.push_back("ultra");
        valid_depth.push_back("quality");
        valid_depth.push_back("performance");

        valid_sensing.push_back("standard");
        valid_sensing.push_back("fill");

        valid_unit.push_back("milli");
        valid_unit.push_back("centi");
        valid_unit.push_back("meter");
        valid_unit.push_back("inch");
        valid_unit.push_back("foot");

        valid_gui.push_back("on");
        valid_gui.push_back("off");
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
    }

    std::string get_measurement_unit()
    {
        return string_map.at("-u");
    }
    std::string get_depth_mode()
    {
        return string_map.at("-d");
    }
    std::string get_sensing_mode()
    {
        return string_map.at("-s");
    }
    bool get_gui_option()
    {
        if (string_map.at("-g").compare("on") == 0)
            return true;
        else
            return false;
    }

private:
    ArgStringMap string_map;
    ValidDepth valid_depth;
    ValidUnit valid_unit;
    ValidSensing valid_sensing;
    ValidGui valid_gui;

    bool check_keyword(const std::string &key, const std::string &value)
    {
        if (key.compare("-u") == 0)
        {
            if (std::find(valid_unit.begin(), valid_unit.end(), value) != valid_unit.end())
                return true;
        }
        else if (key.compare("-d") == 0)
        {
            if (std::find(valid_depth.begin(), valid_depth.end(), value) != valid_depth.end())
                return true;
        }
        else if (key.compare("-s") == 0)
        {
            if (std::find(valid_sensing.begin(), valid_sensing.end(), value) != valid_sensing.end())
                return true;
        }
        else
        {
            if (std::find(valid_gui.begin(), valid_gui.end(), value) != valid_gui.end())
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