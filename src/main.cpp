#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: gps_log_checker <log file>\n";
        return 1;
    }
    std::cout << "Checking " << argv[1] << "\n";
    return 0;
}