#include <iostream>
#include <utils.hpp>
#include <arg_sparser.hpp>
#include <chrono>

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

    sl::Mat image, depth;
    sl::SensorsData data;
    bool sensor_ok = true;

    int n_frames = 0;
    int frame_drop_count = 0;
    int imu_count = 0;
    int bar_count = 0;
    int mag_count = 0;
    int depth_count = 0;

    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Checking " << filename << " status..." << std::endl;
    while (std::chrono::duration_cast<std::chrono::seconds>(end - start).count() < 15)
    {
        auto err = zed_camera->grab();

        if (err == sl::ERROR_CODE::SUCCESS)
        {
            auto err_frame = zed_camera->retrieveImage(image, sl::VIEW::SIDE_BY_SIDE);
            if (err_frame != sl::ERROR_CODE::SUCCESS)
                frame_drop_count++;

            auto measure_err = zed_camera->getSensorsData(data, sl::TIME_REFERENCE::IMAGE);
            if (sensor_ok == true)
            {
                if (measure_err != sl::ERROR_CODE::SUCCESS)
                    sensor_ok = false;

                float norm = data.imu.pose.getTranslation().norm();
                if (norm > std::abs(0.05))
                    imu_count++;

                float pressure = data.barometer.pressure;
                if (pressure > std::abs(0.05))
                    bar_count++;

                float mag_data = data.magnetometer.magnetic_field_calibrated.norm();
                if (mag_data > std::abs(0.05))
                    mag_count++;
            }

            auto depth_err = zed_camera->retrieveMeasure(depth);
            if (depth_err == sl::ERROR_CODE::SUCCESS)
                depth_count++;
        }
        else if (err == sl::ERROR_CODE::END_OF_SVOFILE_REACHED)
        {
            break;
        }
        else
        {
            frame_drop_count++;
        }

        n_frames++;
        end = std::chrono::high_resolution_clock::now();
    }

    std::cout << "Frames processed: " << n_frames << std::endl;
    if (sensor_ok)
    {
        std::cout << "Sensor status: OK" << std::endl;
        std::cout << "Invalid frame count: " << frame_drop_count << std::endl;
        std::cout << "IMU non zero measurements: " << imu_count << "/" << n_frames << std::endl;
        std::cout << "Barometer non zero measurements: " << bar_count << "/" << n_frames << std::endl;
        std::cout << "Magnetometer non zero measurements: " << mag_count << "/" << n_frames << std::endl;
    }
    else
        std::cout << "Sensor status: Unavailable" << std::endl;

    std::cout << "Depth successful computations: " << depth_count << "/" << n_frames << std::endl;
}