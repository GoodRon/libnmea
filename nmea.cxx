/*
 * Copyright (c) 2014, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#include <functional>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <math.h>

#include "nmea.h"
#include "gps.h"
#include "tokens.h"

using namespace std;

namespace nmea {

/**
 * @brief Максимальная длина NMEA строки (по стандарту не должна превышать 80 символов)
 */
const unsigned maxNmeaStringLength = 255;

/**
 * @brief Количество киллометров в одной морской миле
 */
const double kilometersInNauticalMile = 1.852;

uint8_t calculateCrc(const string& str) {
	size_t start = str.find_first_of("$") + 1;
	size_t end = str.find_first_of("*");
	string data = str.substr(start, end - start);

	uint8_t crc = 0;
	for (auto &c: data) {
		crc ^= static_cast<uint8_t>(c);
	}
	return crc;
}

double nmphToKph(double speed) {
	return speed * kilometersInNauticalMile;
}

/**
 * @brief Парсер RMC сообщения (рекомендованный минимальный набор данных)
 *
 * @param str строка NMEA
 * @param gpsData структура данных
 * @return bool
 */
static bool rmc(const string& str, GpsData& gpsData) {
	vector<string> tokens = tokenize(str, ",*");
	if (tokens.size() < 13) {
		return false;
	}
	istringstream stream;

	// UTC время
	int hh = 0, mm = 0, ss = 0;
	stream.str(tokens[1].substr(0,2));
	stream.seekg(0);
	stream >> hh;
	stream.str(tokens[1].substr(2,2));
	stream.seekg(0);
	stream >> mm;
	stream.str(tokens[1].substr(4,2));
	stream.seekg(0);
	stream >> ss;

	// Актуальность данных
	if (tokens[2] == "A") {
		gpsData.actuality = true;
	} else {
		gpsData.actuality = false;
	}

	// Широта
	stream.str(tokens[3]);
	stream.seekg(0);
	stream >> gpsData.latitude;

	// Широтное направление
	if (tokens[4] == "N") {
		gpsData.northLatitude = true;
	} else {
		gpsData.northLatitude = false;
	}

	// Долгота
	stream.str(tokens[5]);
	stream.seekg(0);
	stream >> gpsData.longitude;

	// Долготное направление
	if (tokens[6] == "E") {
		gpsData.eastLongitude = true;
	} else {
		gpsData.eastLongitude = false;
	}

	// Скорость
	stream.str(tokens[7]);
    stream.seekg(0);
    stream >> gpsData.speed;
    gpsData.speed = nmphToKph(gpsData.speed);

	// Путевой угол
	stream.str(tokens[8]);
	stream.seekg(0);
	stream >> gpsData.direction;

	// Время UTC
	int day = 0, month = 0, year = 0;
	stream.str(tokens[9].substr(0,2));
	stream.seekg(0);
	stream >> day;
	stream.str(tokens[9].substr(2,2));
	stream.seekg(0);
	stream >> month;
	stream.str(tokens[9].substr(4,4));
	stream.seekg(0);
	stream >> year;

	tm time;
	time.tm_sec = ss;
	time.tm_min = mm;
	time.tm_hour = hh;
	time.tm_mday = day;
	time.tm_mon = month - 1;
	time.tm_year = year + 100;
	gpsData.timestamp = mktime(&time);

	return true;
}

/**
 * @brief Парсер GGA сообщения (информация о фиксированном решении)
 *
 * @param str строка NMEA
 * @param gpsData структура данных
 * @return bool
 */
static bool gga(const string& str, GpsData& gpsData) {
	vector<string> tokens = tokenize(str, ",*");
	if (tokens.size() < 16) {
		return false;
	}
	istringstream stream;

	// Широта
	stream.str(tokens[2]);
	stream.seekg(0);
	stream >> gpsData.latitude;

	// Широтное направление
	if (tokens[3] == "N") {
		gpsData.northLatitude = true;
	} else {
		gpsData.northLatitude = false;
	}

	// Долгота
	stream.str(tokens[4]);
	stream.seekg(0);
	stream >> gpsData.longitude;

	// Долготное направление
	if (tokens[5] == "E") {
		gpsData.eastLongitude = true;
	} else {
		gpsData.eastLongitude = false;
	}

	// Видимые спутники
	stream.str(tokens[7]);
	stream.seekg(0);
	stream >> gpsData.satellites;

	// HDOP
	stream.str(tokens[8]);
	stream.seekg(0);
	stream >> gpsData.hdop;

	// Высота над уровнем моря
	stream.str(tokens[9]);
	stream.seekg(0);
	stream >> gpsData.altitude;

	return true;
}

/**
 * @brief Парсер GLL сообщения (данные широты и долготы)
 *
 * @param str строка NMEA
 * @param gpsData структура данных
 * @return bool
 */
static bool gll(const string& str, GpsData& gpsData) {
	vector<string> tokens = tokenize(str, ",*");
	if (tokens.size() < 8) {
		return false;
	}
	istringstream stream;

	// Широта
	stream.str(tokens[1]);
	stream.seekg(0);
	stream >> gpsData.latitude;

	// Широтное направление
	if (tokens[2] == "N") {
		gpsData.northLatitude = true;
	} else {
		gpsData.northLatitude = false;
	}

	// Долгота
	stream.str(tokens[3]);
	stream.seekg(0);
	stream >> gpsData.longitude;

	// Долготное направление
	if (tokens[4] == "E") {
		gpsData.eastLongitude = true;
	} else {
		gpsData.eastLongitude = false;
	}

	// Актуальность данных
	if (tokens[6] == "A") {
		gpsData.actuality = true;
	} else {
		gpsData.actuality = false;
	}

	return true;
}

/**
 * @brief Парсер GSA сообщения (общая информация о спутниках)
 *
 * @param str строка NMEA
 * @param gpsData структура данных
 * @return bool
 */
static bool gsa(const string& str, GpsData& gpsData) {
	vector<string> tokens = tokenize(str, ",*");
	if (tokens.size() < 19) {
		return false;
	}
	istringstream stream;

	// HDOP
	stream.str(tokens[16]);
	stream.seekg(0);
	stream >> gpsData.hdop;

	// VDOP
	stream.str(tokens[17]);
	stream.seekg(0);
	stream >> gpsData.vdop;

	return true;
}

/**
 * @brief Парсер GSV сообщения (детальная информация о спутниках)
 *
 * @param str строка NMEA
 * @param gpsData структура данных
 * @return bool
 */
static bool gsv(const string& str, GpsData& gpsData) {
	vector<string> tokens = tokenize(str, ",*");
	if (tokens.size() < 8) {
		return false;
	}
	istringstream stream;

	// Видимые спутники
	stream.str(tokens[3]);
	stream.seekg(0);
	stream >> gpsData.satellites;

	return true;
}

/**
 * @brief Парсер VTG сообщения (вектор движения и скорости)
 *
 * @param str строка NMEA
 * @param gpsData структура данных
 * @return bool
 */
static bool vtg(const string& str, GpsData& gpsData) {
	vector<string> tokens = tokenize(str, ",*");
	if (tokens.size() < 10) {
		return false;
	}
	istringstream stream;

	// Скорость
	stream.str(tokens[7]);
	stream.seekg(0);
	stream >> gpsData.speed;
    gpsData.speed = nmphToKph(gpsData.speed);
	return true;
}

/**
 * @brief Описатель таблицы парсеров NMEA сообщений
 */
struct NmeaParsersTableRow {
	/**
	 * @brief Заголовок сообщения
	 */
	string header;

	/**
	 * @brief Тип сообщения
	 */
	NmeaType type;

	/**
	 * @brief Парсер
	 */
	function<bool (const string& data, GpsData& gpsData)> handler;
};

/**
 * @brief Таблица парсеров NMEA сообщений
 */
static NmeaParsersTableRow NmeaParsersTable[] = {
	{"RMC", ntRMC, rmc},
	{"GGA", ntGGA, gga},
	{"GLL", ntGLL, gll},
	{"GSA", ntGSA, gsa},
	{"GSV", ntGSV, gsv},
	{"VTG", ntVTG, vtg}
};

vector<string> split(const string& data) {
	static string residue;
	vector<string> splittedStrings;

	size_t start = 0, end;
	while (start < data.length()) {
		// TODO find '\n'
		end = data.find_first_of('\r', start);
		residue += data.substr(start, end - start);
		if (residue.length() > maxNmeaStringLength) {
			residue.clear();
			break;
		}
		if (end >= data.length()) {
			break;
		}
		splittedStrings.push_back(residue);
		residue.clear();
		start = end + 2;
	}
	return splittedStrings;
}

bool checkCrc(const string& nmea) {
	uint8_t crcCalculated = calculateCrc(nmea);
	size_t pos = nmea.find_first_of('*') + 1;
	if (pos >= nmea.length()) {
		return false;
	}
	string crcString = nmea.substr(pos);

	unsigned crcProcessed = 0;
	stringstream stream(crcString);
	stream >> std::hex >> crcProcessed;

	if (static_cast<uint8_t>(crcProcessed) != crcCalculated) {
		return false;
	}
	return true;
}

NmeaType parse(const string& nmea, GpsData& gpsData) {
	vector<string> sources = {"$GP", "$GL", "$GN", "$GA"};
	size_t pos = nmea.find_first_of(',');
	string token = nmea.substr(0, pos);
	string header;

	bool validity = false;
	for (auto &source: sources) {
		if (token.find(source) == 0) {
			validity = true;
			header = token.substr(source.length());
			break;
		}
	}
	if (!validity) {
		return ntERR;
	}

	for (auto &parser: NmeaParsersTable) {
		if (parser.header == header) {
			if (parser.handler(nmea, gpsData)) {
				return parser.type;
			}
			break;
		}
	}
	return ntERR;
}

double convertDegreesFromNmeaToNormal(double value) {
	int deg = static_cast<int>(value / 100.0);
	double min = fmod(value, 100.0) / 60.0;
	return static_cast<double>(deg) + min;
}

double convertDegreesFromNormalToNmea(double value) {
	int deg = static_cast<int>(value) * 100;
	double min = fmod(value, 1.0) * 60.0;
	return static_cast<double>(deg) + min;
}

}
