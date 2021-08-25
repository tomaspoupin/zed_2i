#ifndef __DEPTH_UTILS__
#define __DEPTH_UTILS__

#include <sl/Camera.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <iostream>

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

static std::unique_ptr<sl::Camera> get_camera(sl::DEPTH_MODE depth_mode, sl::UNIT unit)
{
    sl::InitParameters params;
    params.depth_mode = depth_mode;
    params.coordinate_units = unit;

    auto zed_camera = std::make_unique<sl::Camera>();
    auto err = zed_camera->open(params);

    if (err != sl::ERROR_CODE::SUCCESS)
    {
        throw err;
    }

    return zed_camera;
}

static inline sl::UNIT string2unit(const std::string &s_unit)
{
    if (s_unit.compare("milli") == 0)
        return sl::UNIT::MILLIMETER;

    else if (s_unit.compare("centi") == 0)
        return sl::UNIT::CENTIMETER;

    else if (s_unit.compare("meter") == 0)
        return sl::UNIT::METER;

    else if (s_unit.compare("inch") == 0)
        return sl::UNIT::INCH;

    else if (s_unit.compare("foot") == 0)
        return sl::UNIT::FOOT;

    return sl::UNIT::MILLIMETER;
}

static inline std::string unit_shorthand(const std::string &s_unit)
{
    if (s_unit.compare("milli") == 0)
        return std::string("[mm]");

    else if (s_unit.compare("centi") == 0)
        return std::string("[cm]");

    else if (s_unit.compare("meter") == 0)
        return std::string("[m]");

    else if (s_unit.compare("inch") == 0)
        return std::string("[in]");

    else if (s_unit.compare("foot") == 0)
        return std::string("[ft]");

    return std::string("[mm]");
}

static inline sl::DEPTH_MODE string2depth(const std::string &s_depth)
{
    if (s_depth.compare("ultra") == 0)
        return sl::DEPTH_MODE::ULTRA;

    else if (s_depth.compare("quality") == 0)
        return sl::DEPTH_MODE::QUALITY;

    else if (s_depth.compare("performance") == 0)
        return sl::DEPTH_MODE::PERFORMANCE;

    return sl::DEPTH_MODE::ULTRA;
}

static inline sl::SENSING_MODE string2sensing(const std::string &s_sensing)
{
    if (s_sensing.compare("standard") == 0)
        return sl::SENSING_MODE::STANDARD;

    else if (s_sensing.compare("fill") == 0)
        return sl::SENSING_MODE::FILL;

    return sl::SENSING_MODE::STANDARD;
}

#define BOX_WIDTH 70
#define BOX_HEIGHT 70
static float compute_distance(sl::Mat &depth_map)
{
    cv::Mat cv_depth_map = slMat2cvMat(depth_map);
    cv::Mat compute_region(
        cv_depth_map,
        cv::Rect(cv_depth_map.cols / 2 - BOX_WIDTH/2,
                 cv_depth_map.rows / 2 - BOX_HEIGHT/2, BOX_WIDTH, BOX_HEIGHT));

    float cum_sum = 0;
    int invalid_count = 0;
    for (int i = 0; i < compute_region.rows; ++i)
    {
        for (int j = 0; j < compute_region.cols; ++j)
        {
            float temp = compute_region.at<float>(i, j);

            if (isnanf(temp) || isinff(temp))
            {
                invalid_count++;
                continue;
            }

            cum_sum += temp;
        }
    }
    return cum_sum / (((float)compute_region.cols * (float)compute_region.rows) - (float)invalid_count);
}

static void display_depth_map(sl::Mat &view, float distance, std::string& unit)
{
    cv::Mat cv_view = slMat2cvMat(view);
    cv::cvtColor(cv_view, cv_view, cv::COLOR_BGRA2GRAY);
    cv::applyColorMap(cv_view, cv_view, cv::COLORMAP_JET);

    cv::rectangle(cv_view,
                  cv::Rect(cv_view.cols / 2 - BOX_WIDTH/2,
                           cv_view.rows / 2 - BOX_HEIGHT/2, BOX_WIDTH, BOX_HEIGHT),
                  cv::Scalar(0, 0, 255),
                  3);

    cv::namedWindow("Depth Map", cv::WINDOW_NORMAL);
    cv::resizeWindow("Depth Map", 800, 600);
    
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << distance;
    std::string message = "Distance: " + stream.str() + " " + unit;
    cv::putText(
        cv_view, message, cv::Point(50, 50),
        cv::FONT_HERSHEY_SIMPLEX, 2.0, cv::Scalar(255, 255, 255), 2);
    cv::imshow("Depth Map", cv_view);
    cv::waitKey(1);
}

#endif