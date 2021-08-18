#include <utils.hpp>
#include <pipeline.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    ArgParser parser;

    try
    {
        parser.parse(argc, argv);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Could not parse arguments: " << e.what() << std::endl;
        return 1;
    }

    std::string resolution = parser.get_resolution_value();
    std::string fps = parser.get_fps_value();
    bool with_svo = parser.get_fileformat_option();
    bool with_gui = parser.get_gui_option();

    std::unique_ptr<sl::Camera> zed_camera;
    try
    {
        zed_camera = get_camera(
            get_resolution(resolution),
            std::stoi(fps));
    }
    catch(const sl::ERROR_CODE& err)
    {
        std::cerr << "Runtime error: " << err << std::endl;
        return 1;
    }

    VQueue video_queue;

    if (with_svo)
    {
        enable_recording(zed_camera.get());
    }
}
