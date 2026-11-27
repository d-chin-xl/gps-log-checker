#pragma once
#include <optional>
#include <string>

// One GGA sentence: where the receiver was, and how good the fix was
struct GgaFix
{
    std::string time;      // UTC time as hhmmss
    double latitude;       // degrees, negative = south
    double longitude;      // degrees, negative = west
    int fixQuality;        // 0 = no fix, 1 = GPS fix, 2 = DGPS fix
    int satellites;        // satellites used
};

enum class LineStatus
{
    Valid,         // checksum OK and the GPS had a fix
    NoFix,         // checksum OK but the GPS had no position
    BadChecksum,   // line was corrupted on the way
    Malformed      // not a GGA sentence we can read
};

// True if the two hex digits after '*' match the XOR of everything between '$' and '*'
bool hasValidChecksum(const std::string& line);

// Converts NMEA "ddmm.mmmm" plus a hemisphere letter into signed degrees
double toDegrees(const std::string& value, char hemisphere);

// Reads a $GPGGA line. Empty if it is not a readable GGA sentence.
std::optional<GgaFix> parseGga(const std::string& line);

// Decides which of the four groups a line belongs in
LineStatus checkLine(const std::string& line);
