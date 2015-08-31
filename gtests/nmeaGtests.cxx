/*
 * Copyright (c) 2015, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <string>

#include <gtest/gtest.h>

#include "nmea.h"
#include "tokens.h"
#include "gps.h"

using namespace std;
using namespace nmea;

const string nmeaRmc = "$GPRMC,091724.00,A,5630.7930,N,08459.3424,E,06.404,075.5,260214,,,A*69";
const string nmeaGgaGsaGsa = "$GPGGA,091724.00,5630.7930,N,08459.3424,E,1,13,0.7,117.1,M,,M,,*74\r\n"
        "$GPGSA,A,3,32,23,25,20,31,14,04,,,,,,1.3,0.7,1.0*30\r\n"
        "$GPGSA,A,3,65,75,66,72,74,76,,,,,,,1.3,0.7,1.0*30\r\n";

TEST(libnmea_tests, nmphToKph) {
    ASSERT_EQ(nmphToKph(1), 1.852);
}

TEST(libnmea_tests, crc) {
    ASSERT_EQ(calculateCrc(nmeaRmc), 105);
    ASSERT_TRUE(checkCrc(nmeaRmc));
}

TEST(libnmea_tests, split) {
    vector<string> parts = {"$GPGGA,091724.00,5630.7930,N,08459.3424,E,1,13,0.7,117.1,M,,M,,*74\r\n",
                            "$GPGSA,A,3,32,23,25,20,31,14,04,,,,,,1.3,0.7,1.0*30\r\n",
                            "$GPGSA,A,3,65,75,66,72,74,76,,,,,,,1.3,0.7,1.0*30\r\n"};
    auto tokens = split(nmeaGgaGsaGsa);
    ASSERT_EQ(tokens.size(), 3);
    for (size_t i = 0; i < tokens.size(); ++i) {
        ASSERT_TRUE(i < parts.size());
        if (i >= parts.size()) {
            break;
        }
        ASSERT_TRUE(rtrimCopy(tokens[i]) == rtrimCopy(parts[i]));
    }
}

TEST(libnmea_tests, parceRmc) {
    GpsData gpsData;
    gpsData.actuality = true;
    gpsData.latitude = 5630.793;
    gpsData.northLatitude = true;
    gpsData.longitude = 8459.3424;
    gpsData.eastLongitude = true;
    gpsData.altitude = 0;
    gpsData.speed = nmphToKph(6.404);
    gpsData.direction = 75.5;
    gpsData.hdop = 99.0;
    gpsData.vdop = 99.0;
    gpsData.satellites = 0;
    gpsData.timestamp = 1393381044;

    GpsData parsedData;
    ASSERT_EQ(parse(nmeaRmc, parsedData), ntRMC);
    ASSERT_EQ(parsedData.actuality, gpsData.actuality);
    ASSERT_EQ(parsedData.latitude, gpsData.latitude);
    ASSERT_EQ(parsedData.northLatitude, gpsData.northLatitude);
    ASSERT_EQ(parsedData.longitude, gpsData.longitude);
    ASSERT_EQ(parsedData.eastLongitude, gpsData.eastLongitude);
    ASSERT_EQ(parsedData.altitude, gpsData.altitude);
    ASSERT_EQ(parsedData.speed, gpsData.speed);
    ASSERT_EQ(parsedData.direction, gpsData.direction);
    ASSERT_EQ(parsedData.hdop, gpsData.hdop);
    ASSERT_EQ(parsedData.vdop, gpsData.vdop);
    ASSERT_EQ(parsedData.satellites, gpsData.satellites);
    ASSERT_EQ(parsedData.timestamp, gpsData.timestamp);
}

TEST(libnmea_tests, convertDegrees) {
    const double nmeaAngle = 2356.12;
    auto result = convertDegreesFromNmeaToNormal(2356.12);
    ASSERT_EQ(nmeaAngle, convertDegreesFromNormalToNmea(result));
}
