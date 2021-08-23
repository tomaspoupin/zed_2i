#ifndef __VID_PIPELINE__
#define __VID_PIPELINE__

#include <utils.hpp>

using FramePayload = std::unique_ptr<sl::Mat>;
using ViewPayload = cv::Mat;


static FramePayload retrieve_frame(sl::Camera *camera)
{
    auto out = std::make_unique<sl::Mat>();
    if (camera->grab() == sl::ERROR_CODE::SUCCESS)
    {
        camera->retrieveImage(*out, sl::VIEW::SIDE_BY_SIDE);
        return out;
    }
    else
        return nullptr;
}

static sl::ERROR_CODE grab_frame(sl::Camera *camera)
{
    return camera->grab();
}

static ViewPayload proccess_frame(FramePayload f_frame)
{
    if (f_frame != nullptr)
    {
        cv::Mat frame = slMat2cvMat(*f_frame);
        cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
        cv::Mat resized_frame;
        cv::resize(frame, resized_frame, cv::Size(frame.cols / 2, frame.rows));

        return resized_frame;
    }
    else
    {
        return cv::Mat();
    }
}

static void record_frame(cv::VideoWriter *writer, ViewPayload& f_frame)
{
    if (!f_frame.empty())
    {
        writer->write(f_frame);
    }
}

static void show_frame(ViewPayload &view, int fps)
{
    cv::namedWindow("Recording", cv::WINDOW_NORMAL);
    cv::resizeWindow("Recording", 600, 400);
    std::string message = "FPS: " + std::to_string(fps);
    cv::putText(
        view, message, cv::Point(50, 50),
        cv::FONT_HERSHEY_SIMPLEX, 2.0, cv::Scalar(255, 255, 255), 2);
    cv::imshow("Recording", view);
    cv::waitKey(1);
}

#endif