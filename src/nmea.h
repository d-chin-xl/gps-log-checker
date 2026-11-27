#pragma once
#include <string>

// True if the two hex digits after '*' match the XOR of everything between '$' and '*'
bool hasValidChecksum(const std::string& line);
