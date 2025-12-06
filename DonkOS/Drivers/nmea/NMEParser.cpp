#include "NMEAParser.h"
#include <cmath>
#include <cstring>

const char *ParseResult_ToString(const ParseResult &result) {
    if (result == ParseResult::Okay) {
        return "OK";
    }
    if (result == ParseResult::GPGAA_Invld_Age) {
        return "GPGAA_Invld_Age";
    }
    if (result == ParseResult::GPGAA_Invld_Alt) {
        return "GPGAA_Invld_Alt";
    }
    if (result == ParseResult::GPGAA_Invld_CRC) {
        return "GPGAA_Invld_CRC";
    }
    if (result == ParseResult::GPGAA_Invld_DOP) {
        return "GPGAA_Invld_DOP";
    }
    if (result == ParseResult::GPGAA_Invld_GeoidSep) {
        return "GPGAA_Invld_GeoidSep";
    }
    if (result == ParseResult::GPGAA_Invld_GPS_Signal) {
        return "GPGAA_Invld_GPS_Signal";
    }
    if (result == ParseResult::GPGAA_Invld_Lat) {
        return "GPGAA_Invld_Lat";
    }
    if (result == ParseResult::GPGAA_Invld_Long) {
        return "GPGAA_Invld_Long";
    }
    if (result == ParseResult::GPGAA_Invld_Lat_Orientation) {
        return "GPGAA_Invld_Lat_Orientation";
    }
    if (result == ParseResult::GPGAA_Invld_Long_Orientation) {
        return "GPGAA_Invld_Long_Orientation";
    }
    if (result == ParseResult::GPGAA_Invld_Ref) {
        return "GPGAA_Invld_Ref";
    }
    if (result == ParseResult::GPGAA_Invld_Sat_Count) {
        return "GPGAA_Invld_Sat_Count";
    }
    if (result == ParseResult::GPGAA_Invld_Time) {
        return "GPGAA_Invld_Time";
    }
    if (result == ParseResult::Invld_Message_Length) {
        return "Invld_Message_Length";
    }
    if (result == ParseResult::Invld_Message_Type) {
        return "Invld_Message_Type";
    }

    return "Unknown";
}

ParseResult NMEAParser::Parse(char *message, NMEAParsedMessage *buffer, const int32_t buffer_len) {
    auto status = ParseResult::Okay;
    const auto full_size = static_cast<int32_t>(strlen(message));

    int32_t msg_len = 0;
    int32_t msg_start = 0;
    int32_t buffer_index = 0;

    for (int32_t i = 0; i <= full_size; i++) {
        if (message[i] == '\n' || message[i] == '\0') {
            if (msg_len > 0) {
                if (buffer_index < buffer_len) {
                    auto result = parseSingle(&message[msg_start], msg_len, &buffer[buffer_index]);
                    if (result == ParseResult::Okay) {
                        msg_len = 0;
                        buffer_index++;
                        msg_start = i + 1;
                    } else {
                        status = result;
                        break;
                    }
                } else {
                    status = ParseResult::ParseBufferOverflow;
                    break;
                }
            }
        } else {
            msg_len++;
        }
    }

    return status;
}

ParseResult NMEAParser::parseSingle(char *message, const int32_t message_len, NMEAParsedMessage *buffer) {
    if (message_len >= NMEA_MIN_LEN && message_len <= NMEA_MAX_LEN) {
        // split to sub-strings and then parse the message
        for (auto i = 0; i < message_len; i++) {
            char ch = message[i];
            if (ch == ',' || ch == '*' || ch == '\r') {
                message[i] = '\0';
            }
        }

        return doParseLine(&message[1], buffer);
    }

    return ParseResult::Invld_Message_Length;
}


ParseResult NMEAParser::doParseLine(char *line, NMEAParsedMessage *buffer) {
    const auto len = static_cast<int32_t>(strnlen(line, NMEA_MSG_TYPE_LEN + 1));
    auto result = ParseResult::Invld_Message_Type;

    if (len == NMEA_MSG_TYPE_LEN) {
        if (strncmp(line, "GPGGA", len) == 0) {
            buffer->messageType = NMEAMessageType::GPGGA_GLOBAL_POSITIONING_FIXED_DATA;
            auto [status, msg] = parseGPGGA(&line[len + 1]);
            result = status;
            buffer->gpgga_message = msg;
        } else if (strncmp(line, "GPGLL", len) == 0) {
            buffer->messageType = NMEAMessageType::GPGLL_GEOGRAPHIC_POSITION_LAT_LONG;
            result = parseGPGLL(&line[len + 1]);
        } else if (strncmp(line, "GPGSA", len) == 0) {
            buffer->messageType = NMEAMessageType::GPGSA_ACTIVE_SATELLITES;
            result = parseGPGSA(&line[len + 1]);
        } else if (strncmp(line, "GPGSV", len) == 0) {
            buffer->messageType = NMEAMessageType::GPGSV_SATELLITES_IN_VIEW;
            result = parseGPGSV(&line[len + 1]);
        } else if (strncmp(line, "GPRMC", len) == 0) {
            buffer->messageType = NMEAMessageType::GPRMC_RECOMMENDED_MIN_SPECIFIC_DATA;
            result = parseGPRMC(&line[len + 1]);
        } else if (strncmp(line, "GPVTG", len) == 0) {
            buffer->messageType = NMEAMessageType::GPVTG_CURSE_OVER_GND_AND_GND_SPEED;
            result = parseGPVTG(&line[len + 1]);
        }
    }

    return result;
}


bool parseUnits(const char *payload, char &output) {
    if (payload[0] == 'M') {
        output = payload[0];
        return true;
    }
    return false;
}

bool parseLatAngular(const char *payload, NMEAAngularDimension &output) {
    const char degree[]{payload[0], payload[1], '\0'};
    output = {(strtol(degree, nullptr, 10)), strtof(&payload[2], nullptr)};
    return true;
}

bool parseLongAngular(const char *payload, NMEAAngularDimension &output) {
    const char degree[]{payload[0], payload[1], payload[2], '\0'};
    output = {(strtol(degree, nullptr, 10)), strtof(&payload[3], nullptr)};
    return true;
}


bool parseTime(const char *payload, NMEATime &output) {
    NMEATime nmeaTime{};
    char buffer[3] = {};

    //parse hours
    buffer[0] = payload[0];
    buffer[1] = payload[1];
    auto hours = strtol(buffer, nullptr, 10);

    //parse minutes
    buffer[0] = payload[2];
    buffer[1] = payload[3];
    auto minutes = strtol(buffer, nullptr, 10);

    //parse seconds
    buffer[0] = payload[4];
    buffer[1] = payload[5];
    auto seconds = strtol(buffer, nullptr, 10);

    // ignore milliseconds

    nmeaTime.hours = static_cast<int8_t>(hours);
    nmeaTime.minutes = static_cast<int8_t>(minutes);
    nmeaTime.seconds = static_cast<int8_t>(seconds);

    output = nmeaTime;
    return true;
}


bool parseHexInteger(const char *payload, int32_t &output) {
    output = strtol(payload, nullptr, 16);
    return true;
}

bool parseInteger(const char *payload, int32_t &output) {
    output = strtol(payload, nullptr, 10);
    return true;
}

bool parseFloat(const char *payload, float &output) {
    output = strtof(payload, nullptr);
    return true;
}

bool parseQualityInfo(const char *payload, NMEAPositionFix &output) {
    // Parse Quality Information
    auto qual = strtol(payload, nullptr, 10);
    if (qual == 6 || (qual >= 0 && qual <= 2)) {
        output = static_cast<NMEAPositionFix>(qual);
        return true;
    }

    return false;
}

bool parseLongitudeOrientation(const char *payload, NMEALongitudeOrientation &output) {
    bool success = true;
    if (payload[0] == 'E') {
        output = NMEALongitudeOrientation::East;
    } else if (payload[0] == 'W') {
        output = NMEALongitudeOrientation::West;
    } else {
        success = false;
    }

    return success;
}

bool parseLatitudeOrientation(const char *payload, NMEALatitudeOrientation &output) {
    bool success = true;
    if (payload[0] == 'N') {
        output = NMEALatitudeOrientation::North;
    } else if (payload[0] == 'S') {
        output = NMEALatitudeOrientation::South;
    } else {
        success = false;
    }

    return success;
}


std::pair<ParseResult, NMEAGPGGAMessage> NMEAParser::parseGPGGA(char *payload) {
    NMEAGPGGAMessage message{};

    auto status = ParseResult::Okay;
    doParse<ParseResult::GPGAA_Invld_Time>(payload, message.time_utc, GPGAA_TIME_LEN, &parseTime, status);
    doParse<ParseResult::GPGAA_Invld_Lat>(payload, message.latitude, GPGAA_LAT_LEN, &parseLatAngular, status);
    doParse<ParseResult::GPGAA_Invld_Lat_Orientation>(payload, message.latitude_orientation, 1,
                                                      &parseLatitudeOrientation, status);
    doParse<ParseResult::GPGAA_Invld_Long>(payload, message.longitude, NMEA_GPGAA_LONG_LEN, &parseLongAngular, status);
    doParse<ParseResult::GPGAA_Invld_Long_Orientation>(payload, message.longitude_orientation, 1,
                                                       &parseLongitudeOrientation, status);
    doParse<ParseResult::GPGAA_Invld_GPS_Signal>(payload, message.position_fix, 1, &parseQualityInfo, status);
    doParse<ParseResult::GPGAA_Invld_Sat_Count>(payload, message.numberOfSatellites, NMEA_GPGAA_SAT_CNT_LEN,
                                                &parseInteger, status);
    doParse<ParseResult::GPGAA_Invld_DOP>(payload, message.dilution_of_precision, NMEA_GPGAA_DOP_LEN, &parseFloat,
                                          status);
    doParse<ParseResult::GPGAA_Invld_Alt>(payload, message.altitude, NMEA_GPGAA_ALT_LEN, &parseFloat, status);
    doParse<ParseResult::GPGAA_Invld_Unit>(payload, message.units_alt, 1, &parseUnits, status);
    doParse<ParseResult::GPGAA_Invld_GeoidSep>(payload, message.geoid_separation, 5, &parseFloat, status);
    doParse<ParseResult::GPGAA_Invld_Unit>(payload, message.units_sep, 1, &parseUnits, status);
    doParse<ParseResult::GPGAA_Invld_Age>(payload, message.ageOfDifferentialCorrection, 4, &parseInteger, status);
    doParse<ParseResult::GPGAA_Invld_Ref>(payload, message.ref_station_id, 4, &parseInteger, status);
    doParse<ParseResult::GPGAA_Invld_CRC>(payload, message.checksum, 4, &parseHexInteger, status);

    return {status, message};
}


ParseResult NMEAParser::parseGPGLL(char *payload) {
    return ParseResult::Okay;
}

ParseResult NMEAParser::parseGPGSA(char *payload) {
    return ParseResult::Okay;
}

ParseResult NMEAParser::parseGPGSV(char *payload) {
    return ParseResult::Okay;
}

ParseResult NMEAParser::parseGPRMC(char *payload) {
    return ParseResult::Okay;
}

ParseResult NMEAParser::parseGPVTG(char *payload) {
    return ParseResult::Okay;
}

template<ParseResult ERROR_STATUS, typename TYPE>
void NMEAParser::doParse(char * &data, TYPE &output, const int32_t max_length,
                         bool (*parseFunction)(const char *, TYPE &),
                         ParseResult &status, const int32_t min_length) {
    if (status == ParseResult::Okay) {
        status = ERROR_STATUS;
        int32_t len = strnlen(data, max_length + 1);
        if (len >= min_length && len <= max_length) {
            // in case len == 0 and was allowed to be by setting the min length, we skip the parsing function (nothing to do then)
            if (len > 0) {
                parseFunction(data, output);
            }
            // advance ptr for following conversions
            data += len + 1;
            status = ParseResult::Okay;
        }
    }
}
