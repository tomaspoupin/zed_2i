#ifndef __VID_UTILS__
#define __VID_UTILS__

#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include <string>
#include <sstream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <sl/Camera.hpp>

using ArgBoolMap = std::map<std::string, bool>;
using ValidRes = std::vector<std::string>;
using ValidFps = std::map<std::string, std::vector<int>>;
using ArgStringMap = std::map<std::string, std::string>;
using Checkpoint = std::chrono::_V2::system_clock::time_point;

class ArgParser
{

public:
    ArgParser()
    {
        bool_map.insert(std::make_pair(std::string("-g"), false));
        bool_map.insert(std::make_pair(std::string("-s"), false));
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

    bool get_gui_option()
    {
        return bool_map.at("-g");
    }
    bool get_fileformat_option()
    {
        return bool_map.at("-s");
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

static sl::RESOLUTION get_resolution(const std::string &resolution)
{
    if (resolution.compare("2.2k") == 0)
        return sl::RESOLUTION::HD2K;
    else if (resolution.compare("1080p") == 0)
        return sl::RESOLUTION::HD1080;
    else if (resolution.compare("720p") == 0)
        return sl::RESOLUTION::HD720;
    else
        return sl::RESOLUTION::VGA;
}

static cv::Size resolution_to_cvsize(sl::RESOLUTION resolution)
{
    switch (resolution)
    {
    case sl::RESOLUTION::HD2K:
        return cv::Size(2208, 1242);
        break;
    case sl::RESOLUTION::HD1080:
        return cv::Size(1920, 1080);
        break;
    case sl::RESOLUTION::HD720:
        return cv::Size(1280, 720);
        break;
    case sl::RESOLUTION::VGA:
        return cv::Size(800, 480);
        break;

    default:
        return cv::Size(1920, 1080);
        break;
    }
}

static std::unique_ptr<sl::Camera> get_camera(sl::RESOLUTION res, int fps)
{
    sl::InitParameters params;
    params.camera_resolution = res;
    params.camera_fps = fps;

    auto zed_camera = std::make_unique<sl::Camera>();
    auto err = zed_camera->open(params);

    if (err != sl::ERROR_CODE::SUCCESS)
    {
        throw err;
    }

    return zed_camera;
}

// Mapping between MAT_TYPE and CV_TYPE
int getOCVtype(sl::MAT_TYPE type)
{
    int cv_type = -1;
    switch (type)
    {
    case sl::MAT_TYPE::F32_C1:
        cv_type = CV_32FC1;
        break;
    case sl::MAT_TYPE::F32_C2:
        cv_type = CV_32FC2;
        break;
    case sl::MAT_TYPE::F32_C3:
        cv_type = CV_32FC3;
        break;
    case sl::MAT_TYPE::F32_C4:
        cv_type = CV_32FC4;
        break;
    case sl::MAT_TYPE::U8_C1:
        cv_type = CV_8UC1;
        break;
    case sl::MAT_TYPE::U8_C2:
        cv_type = CV_8UC2;
        break;
    case sl::MAT_TYPE::U8_C3:
        cv_type = CV_8UC3;
        break;
    case sl::MAT_TYPE::U8_C4:
        cv_type = CV_8UC4;
        break;
    default:
        break;
    }
    return cv_type;
}

cv::Mat slMat2cvMat(sl::Mat &input)
{
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), getOCVtype(input.getDataType()), input.getPtr<sl::uchar1>(sl::MEM::CPU), input.getStepBytes(sl::MEM::CPU));
}

static void enable_recording(sl::Camera *camera)
{
    sl::RecordingParameters recordingParameters;
    recordingParameters.compression_mode = sl::SVO_COMPRESSION_MODE::H264;
    recordingParameters.video_filename = "record.svo";
    auto err = camera->enableRecording(recordingParameters);
    if (err != sl::ERROR_CODE::SUCCESS)
        throw err;
}

static std::unique_ptr<cv::VideoWriter> get_video_writer(int fps, cv::Size &resolution)
{
    std::unique_ptr<cv::VideoWriter> writer = std::make_unique<cv::VideoWriter>();
    int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    std::string file_name = "./record.mkv";
    writer->open(file_name, codec, fps, resolution);

    if (!writer->isOpened())
        throw std::runtime_error("Could not open video writer");

    return writer;
}

#endif