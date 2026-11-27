#include "nmea.h"
#include <catch2/catch_test_macros.hpp>

// A real GGA sentence with a correct checksum (*47)
static const std::string good = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";

TEST_CASE("A correct checksum is accepted")
{
    REQUIRE(hasValidChecksum(good));
}

TEST_CASE("A wrong checksum is rejected")
{
    REQUIRE_FALSE(hasValidChecksum("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*00"));
}

TEST_CASE("One changed character breaks the checksum")
{
    // Same line as 'good' but latitude 4807.038 -> 4807.039
    REQUIRE_FALSE(hasValidChecksum("$GPGGA,123519,4807.039,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"));
}

TEST_CASE("Lines without $ or * are rejected")
{
    REQUIRE_FALSE(hasValidChecksum("GPGGA,123519*47"));
    REQUIRE_FALSE(hasValidChecksum("$GPGGA,123519"));
    REQUIRE_FALSE(hasValidChecksum(""));
}
