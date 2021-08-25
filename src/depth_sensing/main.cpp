#include "arg_dparser.hpp"
#include "utils.hpp"
#include <thread>
#include <chrono>

bool exit_app = false;
void poll_exit();
void show_distance(bool with_gui, std::string shorthand);

float distance;

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

    std::string depth_mode_s = parser.get_depth_mode();
    std::string sensing_mode_s = parser.get_sensing_mode();
    std::string m_unit_s = parser.get_measurement_unit();

    bool with_gui = parser.get_gui_option();
    sl::UNIT m_unit = string2unit(m_unit_s);
    sl::SENSING_MODE sensing_mode = string2sensing(sensing_mode_s);
    sl::DEPTH_MODE depth_mode = string2depth(depth_mode_s);
    std::string unit_sh = unit_shorthand(m_unit_s);

    std::cout << "Measurement unit: " << m_unit_s << std::endl;
    std::cout << "Sensing mode: " << sensing_mode_s << std::endl;
    std::cout << "Depth mode: " << depth_mode_s << std::endl;
    std::cout << "GUI Enable: " << with_gui << std::endl << std::endl;

    std::cout << "Initializing resources..." << std::endl;

    std::unique_ptr<sl::Camera> zed_camera;

    try
    {
        zed_camera = get_camera(depth_mode, m_unit);
    }
    catch (const sl::ERROR_CODE &err)
    {
        std::cerr << "Runtime error: " << err << std::endl;
        return 1;
    }

    std::cout << "Starting depth measurement." << std::endl;

    sl::RuntimeParameters rt_params;
    rt_params.sensing_mode = sensing_mode;
    sl::Mat depth_map, view;

    std::thread poll(poll_exit);
    std::thread distance_viewer(show_distance, with_gui, m_unit_s);

    while (exit_app == false)
    {
        if (zed_camera->grab(rt_params) == sl::ERROR_CODE::SUCCESS)
        {
            zed_camera->retrieveMeasure(depth_map, sl::MEASURE::DEPTH);
            distance = compute_distance(depth_map);

            if (with_gui)
            {
                zed_camera->retrieveImage(view, sl::VIEW::DEPTH);
                display_depth_map(view, distance, unit_sh);
            }
        }
    }

    poll.join();
    distance_viewer.join();
}

void poll_exit()
{
    std::string input;
    char next_char = {0};
    std::cout << "Press Q to exit application: ";

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

void show_distance(bool with_gui, std::string unit)
{
    std::string shorthand = unit_shorthand(unit);
    while (exit_app == false && !with_gui)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << '\r'
                  << "Distance " << shorthand << ": " << std::setw(5) << distance << 
                  " -> (Q to exit): " << std::flush;
    }
    std::cout << std::endl;
}