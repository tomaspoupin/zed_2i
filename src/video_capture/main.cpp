#include <utils.hpp>
#include <tasks.hpp>
#include <iostream>
#include <iomanip>
#include <arg_parser.hpp>

void poll_exit();
void show_fps(bool with_gui);
bool exit_app = false;
int fps_view = 0;
Checkpoint t;

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
    bool with_svo = parser.get_fileformat_option();
    bool with_gui = parser.get_gui_option();

    int fps = std::stoi(s_fps);
    sl::RESOLUTION resolution = get_resolution(s_resolution);
    cv::Size resolution_size = resolution_to_cvsize(resolution);

    std::unique_ptr<sl::Camera> zed_camera = nullptr;
    std::unique_ptr<cv::VideoWriter> writer = nullptr;

    std::cout << "Resolution: " << s_resolution << std::endl;
    std::cout << "FPS: " << s_fps << std::endl;
    std::cout << "With GUI: " << with_gui << std::endl;
    std::cout << "With SVO: " << with_svo << std::endl
              << std::endl;

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

    try
    {
        if (with_svo)
        {
            enable_recording(zed_camera.get());
        }
        else
        {
            writer = get_video_writer(fps, resolution_size);
        }
    }
    catch (const sl::ERROR_CODE &err)
    {
        std::cerr << "Could not enable recording: " << err << std::endl;
    }
    catch (const std::runtime_error &err)
    {
        std::cerr << "Could not initialize video writer: " << err.what() << std::endl;
    }

    std::cout << "Recording started." << std::endl;

    std::thread poll(poll_exit);
    std::thread fps_viewer(show_fps, with_gui);

    while (exit_app == false)
    {
        if (with_gui && !with_svo)
            opencv_with_gui(zed_camera.get(), writer.get(), fps_view);
        else if (with_gui && with_svo)
            svo_with_gui(zed_camera.get(), fps_view);
        else if (!with_gui && !with_svo)
            opencv_without_gui(zed_camera.get(), writer.get());
        else
            svo_without_gui(zed_camera.get());
        auto temp = t;
        t = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_double = t - temp;
        fps_view = 1000.0 / ms_double.count();
    }

    poll.join();
    fps_viewer.join();

    std::cout << "Quitting Application." << std::endl;

    if (writer != nullptr)
        writer->release();
}

void poll_exit()
{
    std::string input;
    char next_char = {0};
    std::cout << "Press Q to exit application." << std::endl;

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

void show_fps(bool with_gui)
{
    while (exit_app == false && !with_gui)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << '\r'
                  << "Current Throughput (Hz): " << std::setw(2) << fps_view << " -> (Q to exit): " << std::flush;
    }
    std::cout << std::endl;
}