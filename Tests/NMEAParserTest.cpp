#include <gtest/gtest.h>
#include "NMEAParser.h"

TEST(NMEParserTest, GPGGA_1) {
    NMEAParser parser{};
    char message[] = "$GPGGA,121014.sss,1234.567899,N,87654.321011,W,1,11,2.1,27.0,M,-34.2,M,,0000*5E";
    NMEAParsedMessage parsed_msg{};
    auto status = parser.Parse(message, &parsed_msg, 1);

    ASSERT_EQ(status, ParseResult::Okay);
    ASSERT_EQ(parsed_msg.messageType, NMEAMessageType::GPGGA_GLOBAL_POSITIONING_FIXED_DATA);
    auto result_message = parsed_msg.gpgga_message;

    ASSERT_EQ(result_message.time_utc.hours, 12);
    ASSERT_EQ(result_message.time_utc.minutes, 10);
    ASSERT_EQ(result_message.time_utc.seconds, 14);

    ASSERT_EQ(result_message.latitude.degree, 12);
    ASSERT_FLOAT_EQ(result_message.latitude.minute, 34.567899);

    ASSERT_EQ(result_message.longitude.degree, 876);
    ASSERT_FLOAT_EQ(result_message.longitude.minute, 54.321011);
}


TEST(NMEParserTest, GPGGA_2) {
    NMEAParser parser{};
    char message[] = "$GPGGA,074844.00,,,,,0,00,99.99,,,,,,*6D";
    NMEAParsedMessage parsed_msg{};
    auto status = parser.Parse(message, &parsed_msg, 1);

    ASSERT_EQ(status, ParseResult::Okay);
    ASSERT_EQ(parsed_msg.messageType, NMEAMessageType::GPGGA_GLOBAL_POSITIONING_FIXED_DATA);
    auto result_message = parsed_msg.gpgga_message;

    ASSERT_EQ(result_message.time_utc.hours, 7);
    ASSERT_EQ(result_message.time_utc.minutes, 48);
    ASSERT_EQ(result_message.time_utc.seconds, 44);

    ASSERT_EQ(result_message.latitude.degree, 0);
    ASSERT_FLOAT_EQ(result_message.latitude.minute, 0);

    ASSERT_EQ(result_message.longitude.degree, 0);
    ASSERT_FLOAT_EQ(result_message.longitude.minute, 0);

    ASSERT_EQ(result_message.longitude_orientation, NMEALongitudeOrientation::Unknown);
    ASSERT_EQ(result_message.latitude_orientation, NMEALatitudeOrientation::Unknown);

    ASSERT_EQ(result_message.numberOfSatellites, 0);
    ASSERT_FLOAT_EQ(result_message.dilution_of_precision, 99.99);
    ASSERT_FLOAT_EQ(result_message.altitude, 0);
    ASSERT_FLOAT_EQ(result_message.geoid_separation, 0);
    ASSERT_EQ(result_message.ageOfDifferentialCorrection, 0);

    ASSERT_EQ(result_message.units_alt, '\0');
    ASSERT_EQ(result_message.units_sep, '\0');

    ASSERT_EQ(result_message.ref_station_id, 0);
    ASSERT_EQ(result_message.checksum, 0x6D);
}


TEST(NMEParserTest, GPGGA_3) {
    NMEAParser parser{};

    char message[] = "$GPVTG,,,,,,,,,N*30\r\n"
            "$GPGGA,083246.00,,,,,0,00,99.99,,,,,,*6D";

    NMEAParsedMessage buffer[2];
    auto status = parser.Parse(message, buffer, 2);

    ASSERT_EQ(status, ParseResult::Okay);

    ASSERT_EQ(buffer[0].messageType, NMEAMessageType::GPVTG_CURSE_OVER_GND_AND_GND_SPEED);
    ASSERT_EQ(buffer[1].messageType, NMEAMessageType::GPGGA_GLOBAL_POSITIONING_FIXED_DATA);
}



TEST(NMEParserTest, GPGGA_4) {
    NMEAParser parser{};
    char message[] = "$GPGGA,091059.00,4818.00825,N,01136.85193,E,1,06,2.06,481.6,M,46.3,M,,*59";
    NMEAParsedMessage parsed_msg{};
    auto status = parser.Parse(message, &parsed_msg, 1);

    ASSERT_EQ(status, ParseResult::Okay);
    ASSERT_EQ(parsed_msg.messageType, NMEAMessageType::GPGGA_GLOBAL_POSITIONING_FIXED_DATA);
    auto result_message = parsed_msg.gpgga_message;

    ASSERT_EQ(result_message.time_utc.hours, 9);
    ASSERT_EQ(result_message.time_utc.minutes, 10);
    ASSERT_EQ(result_message.time_utc.seconds, 59);
}
