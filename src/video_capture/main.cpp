#include <utils.hpp>
#include <tasks.hpp>
#include <iostream>
#include <iomanip>
#include <arg_parser.hpp>

void poll_exit();
void show_fps(bool with_gui);
bool exit_app = false;
int fps_view = 0;

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

    std::string s_resolution = parser.get_resolution_value();
    std::string s_fps = parser.get_fps_value();

    int fps = std::stoi(s_fps);
    sl::RESOLUTION resolution = get_resolution(s_resolution);
    cv::Size resolution_size = resolution_to_cvsize(resolution);

    std::unique_ptr<sl::Camera> zed_camera = nullptr;

    std::cout << "Resolution: " << s_resolution << std::endl;
    std::cout << "FPS: " << s_fps << std::endl;

    std::cout << "Initializing resources..." << std::endl;

    try
    {
        zed_camera = get_camera(
            resolution,
            fps);
    }
    catch (const sl::ERROR_CODE &err)
    {
        std::cerr << "Runtime error: " << err << std::endl;
        return 1;
    }

    std::string filename = get_filename();

    try
    {
        enable_recording(zed_camera.get(), filename);
    }
    catch (const sl::ERROR_CODE &err)
    {
        std::cerr << "Could not enable recording: " << err << std::endl;
        return 1;
    }

    std::cout << "Recording started." << std::endl;
    std::cout << "Writing to: " << filename << std::endl;

    std::thread poll(poll_exit);

    sl::RuntimeParameters params;
    params.enable_depth = false;
    while (exit_app == false)
    {
        record_step(zed_camera.get(), params);
    }

    poll.join();

    std::cout << "Quitting Application." << std::endl;
}

void poll_exit()
{
    std::string input;
    char next_char = {0};
    std::cout << "Press Q to finish recording: ";

    while (true)
    {
        std::getline(std::cin, input);

        if (input.length() == 1)
            next_char = input[0];

        if (next_char == 'Q' || next_char == 'q')
            break;

        std::this_thread::sleep_for(std::chrono::microseconds(20));
    }

    exit_app = true;
}