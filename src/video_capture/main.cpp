#include <utils.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
    ArgParser parser;

    try
    {
        parser.parse(argc, argv);
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << "Could not parse arguments: " << e.what() << std::endl;
    }

    std::cout << "With GUI: " << parser.get_gui_option() << std::endl;
    std::cout << "With SVO: " << parser.get_fileformat_option() << std::endl;
    std::cout << "Resolution: " << parser.get_resolution_value() << std::endl;
    std::cout << "Framerate: " << parser.get_fps_value() << std::endl;
}