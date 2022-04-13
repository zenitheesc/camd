#include <chrono>
#include <ctime>
#include <frameworkd/classes/daemon/daemon.hpp>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <string>

class CamdService : public RoutineService {
public:
    explicit CamdService(std::string serviceId)
        : RoutineService { serviceId }
    {
    }
    void destroy() override
    {
    }
};

auto main() -> int
{

    return 0;
}
