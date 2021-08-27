#ifndef __VID_UTILS__
#define __VID_UTILS__

#include <sl/Camera.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <boost/filesystem.hpp>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <iostream>

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

static void enable_recording(sl::Camera *camera, const std::string& filename)
{
    sl::RecordingParameters recordingParameters;
    recordingParameters.compression_mode = sl::SVO_COMPRESSION_MODE::H264;
    recordingParameters.video_filename = sl::String(filename.c_str());
    auto err = camera->enableRecording(recordingParameters);
    if (err != sl::ERROR_CODE::SUCCESS)
        throw err;
}

static std::string get_filename()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream buffer;
    buffer << std::put_time(&tm, "%d-%m-%Y_%Hh-%Mm-%Ss");
    std::string filename = "ZED2i_" + buffer.str() + ".svo";
    return filename;
}

// Mapping between MAT_TYPE and CV_TYPE
static int getOCVtype(sl::MAT_TYPE type)
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

static cv::Mat slMat2cvMat(sl::Mat &input)
{
    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), getOCVtype(input.getDataType()), input.getPtr<sl::uchar1>(sl::MEM::CPU), input.getStepBytes(sl::MEM::CPU));
}

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

#endif