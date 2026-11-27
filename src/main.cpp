#include "nmea.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: gps_log_checker <log file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cerr << "Could not open " << argv[1] << "\n";
        return 1;
    }

    int valid = 0, noFix = 0, badChecksum = 0, malformed = 0;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line))
    {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r')   // logs saved on Windows end in \r\n
            line.pop_back();
        if (line.empty())
            continue;

        switch (checkLine(line))
        {
        case LineStatus::Valid:
        {
            GgaFix fix = *parseGga(line);
            std::cout << "Line " << lineNumber << ": OK      " << std::fixed << std::setprecision(5)
                      << fix.latitude << ", " << fix.longitude << " (" << fix.satellites << " satellites)\n";
            ++valid;
            break;
        }
        case LineStatus::NoFix:
            std::cout << "Line " << lineNumber << ": NO FIX  GPS had no position\n";
            ++noFix;
            break;
        case LineStatus::BadChecksum:
            std::cout << "Line " << lineNumber << ": BAD     checksum does not match\n";
            ++badChecksum;
            break;
        case LineStatus::Malformed:
            std::cout << "Line " << lineNumber << ": BAD     not a readable GGA sentence\n";
            ++malformed;
            break;
        }
    }

    std::cout << "\nSummary: " << valid << " valid, " << noFix << " no fix, "
              << badChecksum << " bad checksum, " << malformed << " malformed\n";

    // Exit code 1 if anything was corrupted, so scripts and CI can tell
    return (badChecksum + malformed) > 0 ? 1 : 0;
}
