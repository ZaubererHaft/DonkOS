#ifndef DONKOS_NMEAPARSER_H
#define DONKOS_NMEAPARSER_H

#include <cstdint>
#include <utility>

enum class NMEAMessageType {
    Unknown,
    GPGGA_GLOBAL_POSITIONING_FIXED_DATA,
    GPGLL_GEOGRAPHIC_POSITION_LAT_LONG,
    GPGSA_ACTIVE_SATELLITES,
    GPGSV_SATELLITES_IN_VIEW,
    GPRMC_RECOMMENDED_MIN_SPECIFIC_DATA,
    GPVTG_CURSE_OVER_GND_AND_GND_SPEED
};


struct NMEATime {
    int8_t hours;
    int8_t minutes;
    int8_t seconds;
};

enum class NMEAPositionFix {
    Invalid, GPS_FIX, DGPS_FIX, ESTIMATED = 6
};

enum class NMEALatitudeOrientation {
    Unknown, North, South
};

enum class NMEALongitudeOrientation {
    Unknown, East, West
};

class NMEAAngularDimension {
public:
    int32_t degree;
    float minute;
};

class NMEAMessage {
public:
    NMEAMessageType type;
};

class NMEAGPGSAMessage {
public:
};

class NMEAGPGSVMessage {
public:
};

class NMEAGPRMCVMessage {
public:
};

class NMEAGPVTGMessage {
public:
};

class NMEAGPGLLMessage {
public:
};

class NMEAGPGGAMessage {
public:
    NMEATime time_utc;
    NMEAAngularDimension latitude;
    NMEAAngularDimension longitude;
    NMEALatitudeOrientation latitude_orientation;
    NMEALongitudeOrientation longitude_orientation;
    NMEAPositionFix position_fix;
    int32_t numberOfSatellites;
    float dilution_of_precision;
    float altitude;
    char units_alt;
    float geoid_separation;
    char units_sep;
    int32_t ageOfDifferentialCorrection;
    int32_t ref_station_id;
    int32_t checksum;
};

class NMEAParsedMessage {
public:
    NMEAMessageType messageType;

    union {
        NMEAGPGGAMessage gpgga_message;
        NMEAGPGLLMessage gpgll_message;
        NMEAGPGSAMessage gpgsa_message;
        NMEAGPGSVMessage gpgsv_message;
        NMEAGPRMCVMessage gpgrcv_message;
        NMEAGPVTGMessage gpgtpvt_message;
    };
};

enum class ParseResult {
    Okay,
    Invld_Message_Length,
    Invld_Message_Type,
    GPGAA_Invld_Time,
    GPGAA_Invld_Lat,
    GPGAA_Invld_Lat_Orientation,
    GPGAA_Invld_Long,
    GPGAA_Invld_Long_Orientation,
    GPGAA_Invld_GPS_Signal,
    GPGAA_Invld_Sat_Count,
    GPGAA_Invld_DOP,
    GPGAA_Invld_Alt,
    GPGAA_Invld_Unit,
    GPGAA_Invld_GeoidSep,
    GPGAA_Invld_Age,
    GPGAA_Invld_Ref,
    GPGAA_Invld_CRC,
    ParseBufferOverflow
};

class NMEAParser {
public:
    ParseResult Parse(char *message, NMEAParsedMessage *buffer, int32_t buffer_len);

private:
    ParseResult parseSingle(char *message, int32_t message_len, NMEAParsedMessage *buffer);

    ParseResult doParseLine(char *line, NMEAParsedMessage *buffer);

    std::pair<ParseResult, NMEAGPGGAMessage> parseGPGGA(char *payload);

    ParseResult parseGPGLL(char *payload);

    ParseResult parseGPGSA(char *payload);

    ParseResult parseGPGSV(char *payload);

    ParseResult parseGPRMC(char *payload);

    ParseResult parseGPVTG(char *payload);

    template<ParseResult ERROR_STATUS, typename TYPE>
    void doParse(char * &data, TYPE &output, int32_t max_length, bool (*parseFunction)(const char *, TYPE &),
                 ParseResult &status, int32_t min_length = 0);

    static constexpr int32_t NMEA_MSG_TYPE_LEN = 5;
    static constexpr int32_t NMEA_MIN_LEN = 0; // ToDo what is the min length
    static constexpr int32_t NMEA_MAX_LEN = 82; // ToDo what is the max length
    static constexpr int32_t GPGAA_TIME_LEN = 10;
    static constexpr int32_t GPGAA_LAT_LEN = 11;
    static constexpr int32_t NMEA_GPGAA_LONG_LEN = 12;
    static constexpr int32_t NMEA_GPGAA_LAT_LEN = 9;
    static constexpr int32_t NMEA_GPGAA_SAT_CNT_LEN = 2;
    static constexpr int32_t NMEA_GPGAA_DOP_LEN = 5;
    static constexpr int32_t NMEA_GPGAA_ALT_LEN = 5;

};

#endif //DONKOS_NMEAPARSER_H
