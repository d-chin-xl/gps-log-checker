#include "nmea.h"
#include <catch2/catch_approx.hpp>
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

TEST_CASE("Degrees and minutes convert to decimal degrees")
{
    REQUIRE(toDegrees("4807.038", 'N') == Catch::Approx(48.1173));
    REQUIRE(toDegrees("01131.000", 'E') == Catch::Approx(11.516667));
    REQUIRE(toDegrees("0117.520", 'S') == Catch::Approx(-1.292));
}

TEST_CASE("A GGA sentence is parsed into its fields")
{
    auto fix = parseGga(good);
    REQUIRE(fix.has_value());
    REQUIRE(fix->time == "123519");
    REQUIRE(fix->latitude == Catch::Approx(48.1173));
    REQUIRE(fix->longitude == Catch::Approx(11.516667));
    REQUIRE(fix->fixQuality == 1);
    REQUIRE(fix->satellites == 8);
}

TEST_CASE("Lines are sorted into the right group")
{
    REQUIRE(checkLine(good) == LineStatus::Valid);
    REQUIRE(checkLine("$GPGGA,123522,,,,,0,00,,,M,,M,,*63") == LineStatus::NoFix);
    REQUIRE(checkLine("$GPGGA,123524,0117.521,N,10351.331,E,1,09,0.8,15.3,M,4.5,M,,*00") == LineStatus::BadChecksum);
    REQUIRE(checkLine("GPGGA garbage line from a loose connector") == LineStatus::Malformed);
}
