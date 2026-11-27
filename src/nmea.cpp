#include "nmea.h"
#include <cstdlib>
#include <sstream>
#include <vector>

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

double toDegrees(const std::string& value, char hemisphere)
{
    // "4807.038" means 48 degrees and 7.038 minutes; 60 minutes = 1 degree
    double raw = std::atof(value.c_str());
    int degrees = static_cast<int>(raw / 100);
    double minutes = raw - degrees * 100;
    double result = degrees + minutes / 60.0;
    if (hemisphere == 'S' || hemisphere == 'W')
        result = -result;
    return result;
}

// Splits "a,b,,c" into {"a","b","","c"}, keeping empty fields
static std::vector<std::string> splitFields(const std::string& text)
{
    std::vector<std::string> fields;
    std::stringstream stream(text);
    std::string field;
    while (std::getline(stream, field, ','))
        fields.push_back(field);
    if (!text.empty() && text.back() == ',')
        fields.push_back("");
    return fields;
}

std::optional<GgaFix> parseGga(const std::string& line)
{
    if (line.rfind("$GPGGA,", 0) != 0)
        return std::nullopt;

    // Only look at the part before '*'
    std::string body = line.substr(1, line.find('*') - 1);
    std::vector<std::string> f = splitFields(body);
    if (f.size() < 8)
        return std::nullopt;

    GgaFix fix{};
    fix.time = f[1];
    fix.fixQuality = std::atoi(f[6].c_str());
    fix.satellites = std::atoi(f[7].c_str());
    if (fix.fixQuality > 0)
    {
        fix.latitude = toDegrees(f[2], f[3].empty() ? 'N' : f[3][0]);
        fix.longitude = toDegrees(f[4], f[5].empty() ? 'E' : f[5][0]);
    }
    return fix;
}

LineStatus checkLine(const std::string& line)
{
    if (line.empty() || line[0] != '$' || line.find('*') == std::string::npos)
        return LineStatus::Malformed;
    if (!hasValidChecksum(line))
        return LineStatus::BadChecksum;

    std::optional<GgaFix> fix = parseGga(line);
    if (!fix)
        return LineStatus::Malformed;
    return fix->fixQuality == 0 ? LineStatus::NoFix : LineStatus::Valid;
}
