#ifndef __VID_UTILS__
#define __VID_UTILS__

#include <sl/Camera.hpp>
static std::unique_ptr<sl::Camera> open_svo_file(const std::string& filename)
{
    sl::String input_path(filename.c_str());
    sl::InitParameters params;
    params.input.setFromSVOFile(input_path);
    
    auto zed_camera = std::make_unique<sl::Camera>();
    auto err = zed_camera->open(params);

    if (err != sl::ERROR_CODE::SUCCESS)
    {
        throw err;
    }

    return zed_camera;
}

#endif