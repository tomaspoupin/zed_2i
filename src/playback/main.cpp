#include <iostream>
#include <utils.hpp>
#include <arg_pparser.hpp>

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

    std::string filename = parser.get_filename();
    std::unique_ptr<sl::Camera> zed_camera;

    try
    {
        zed_camera = open_svo_file(filename);
    }
    catch (const sl::ERROR_CODE &err)
    {
        std::cerr << "Could not open svo file: " << err << std::endl;
        return 1;
    }

    sl::Mat image;
    while (true)
    {
        auto err = zed_camera->grab();

        if (err == sl::ERROR_CODE::SUCCESS)
        {
            zed_camera->retrieveImage(image, sl::VIEW::SIDE_BY_SIDE);
            cv::Mat view = slMat2cvMat(image);
            cv::imshow("Record", view);
            cv::waitKey(1);
        }
        else
        {
            break;
        }
    }
}