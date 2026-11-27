#include "nmea.h"
#include <cstdlib>

bool hasValidChecksum(const std::string& line)
{
    // Needs a '$' at the start and "*XX" at the end
    if (line.size() < 4 || line[0] != '$')
        return false;
    std::size_t star = line.find('*');
    if (star == std::string::npos || star + 3 > line.size())
        return false;

    // XOR every character between '$' and '*'
    unsigned char sum = 0;
    for (std::size_t i = 1; i < star; ++i)
        sum ^= static_cast<unsigned char>(line[i]);

    // Compare with the two hex digits the GPS sent
    char* end = nullptr;
    std::string sent = line.substr(star + 1, 2);
    long expected = std::strtol(sent.c_str(), &end, 16);
    if (end != sent.c_str() + 2)
        return false;
    return sum == expected;
}
