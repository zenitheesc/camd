#include <chrono>
#include <ctime>
#include <frameworkd/classes/daemon/daemon.hpp>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <string>

class CamdService : public RoutineService {
    static cv::Mat frame;
    static cv::VideoCapture cap;
    const int camTurningOnDelay = 6000; // a camera liga depois de 60 segundos
    const std::string absoluteFilePath = "./";
    const int camPhotoDelay = 15000; // uma foto a cada 15 segundos

    const DBusHandler::Path takePhotoPath {
        "zfkd.dbus.camd",
        "/zfkd/dbus/camd",
        "zfkd.dbus.camd",
        "takePhoto"
    };

public:
    explicit CamdService(std::string serviceId)
        : RoutineService { serviceId }
    {
    }

    void setup() override
    {
        cap.open(0, cv::CAP_ANY);

        if (!cap.isOpened()) {
            std::cerr << "ERRO! Câmera inacessível\n";
        }

        DBusHandler::registerMethod(takePhotoPath, [&](nlohmann::json messageWrapper) {
            nlohmann::json response;

            try {
                takePhoto(messageWrapper["filePath"]);
                response["error"] = false;
            } catch (...) {
                response["error"] = true;
            }

            return response;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(camTurningOnDelay));
    }

    void routine() override
    {
        std::string fileName = getFileName();
        try {

            takePhoto(absoluteFilePath + "/" + fileName + ".jpg");
            std::this_thread::sleep_for(std::chrono::milliseconds(camPhotoDelay));
        } catch (...) {
            std::cerr << "ERROR! could not save the picture \n";
        }
    }

    static auto getFileName() -> std::string
    {
        std::time_t now = std::time(nullptr);
        std::string timeStamp(std::ctime(&now));

        std::replace(timeStamp.begin(), timeStamp.end(), ' ', '-');
        timeStamp.pop_back();

        return timeStamp;
    }

    static void takePhoto(std::string filePath)
    {
        cap.read(frame);

        if (frame.empty()) {
            std::cerr << getFileName() + ": ERROR! empty frame \n";
        }

        cv::imwrite(filePath, frame);
    }

    void destroy() override
    {
    }
};

cv::Mat CamdService::frame;
cv::VideoCapture CamdService::cap;

auto main() -> int
{
    Daemon camd("camd");
    CamdService camService("camd");
    camd.deploy(camService);
    camd.run();

    return 0;
}
