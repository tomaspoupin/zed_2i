#ifndef __VID_TASKS__
#define __VID_TASKS__

#include <pipeline.hpp>

static void svo_with_gui(sl::Camera *camera, int &fps)
{
    FramePayload payload = retrieve_frame(camera);
    ViewPayload view = proccess_frame(std::move(payload));
    show_frame(view, fps);
}

static void svo_without_gui(sl::Camera *camera)
{
    grab_frame(camera);
}

static void opencv_with_gui(sl::Camera *camera, cv::VideoWriter *writer, int fps)
{
    FramePayload payload = retrieve_frame(camera);
    ViewPayload view = proccess_frame(std::move(payload));
    record_frame(writer, view);
    show_frame(view, fps);
}

static void opencv_without_gui(sl::Camera *camera, cv::VideoWriter *writer)
{
    FramePayload payload = retrieve_frame(camera);
    ViewPayload view = proccess_frame(std::move(payload));
    record_frame(writer, view);
}

#endif