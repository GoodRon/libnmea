/*
 * Copyright (c) 2014, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef NMEA_H
#define NMEA_H

#include <string>
#include <vector>

struct GpsData;

/**
 * @brief Пространство имен библиотеки для работы с NMEA строками
 */
namespace nmea {

/**
 * @brief Перечисление типов NMEA сообщений
 */
enum NmeaType {
	/**
	 * @brief Рекомендованный минимальный набор данных
	 */
	ntRMC,

	/**
	 * @brief Информация о фиксированном решении
	 */
	ntGGA,

	/**
	 * @brief Данные широты и долготы
	 */
	ntGLL,

	/**
	 * @brief Общая информация о спутниках
	 */
	ntGSA,

	/**
	 * @brief Детальная информация о спутниках
	 */
	ntGSV,

	/**
	 * @brief Вектор движения и скорости
	 */
	ntVTG,

	/**
	 * @brief Некорректное сообщение
	 */
	ntERR
};

/**
 * @brief Перевод скорости из милей/час в километры/час
 *
 * @param speed скорость миль/час
 * @return double скорость км/час
 */
double mphToKph(double speed);

/**
 * @brief Расчет CRC для NMEA строки (XOR-сумма всех байт в строке между «$» и «*»)
 *
 * @param str NMEA строка
 * @return uint8_t crc
 */
uint8_t calculateCrc(const std::string& str);

/**
 * @brief Проверка CRC NMEA строки
 *
 * @param str NMEA строка
 * @return bool
 */
bool checkCrc(const std::string& nmea);

/**
 * @brief Разделить поток данных от навигатора на отдельные NMEA строки
 * Обрывки NMEA строк запоминаются и, при последующих вызовах, восстанавливаются из частей
 *
 * @param data текстовые данные
 * @return vector<string>
 */
std::vector<std::string> split(const std::string& data);

/**
 * @brief Разбор NMEA строки. Распарсенные данные заполняют структуру GpsData
 *
 * @param nmea текстовые данные
 * @param gpsData структура данных
 * @return NmeaType тип разобранного сообщения
 */
NmeaType parse(const std::string& nmea, GpsData& gpsData);

/**
 * @brief Перевод градусов из формата NMEA (GGMM.MM — 2 цифры градусов (GG), 2 цифры целых минут
 * (MM), точка и дробная часть минут переменной длины (.MM)) в вещественное число (gg.mm - цифры
 * градусов (gg), точка и доли градусов (.mm))
 *
 * @param value значение в формате GGMM.MM
 * @return double значение в формате gg.mm
 */
double convertDegreesFromNmeaToNormal(double value);

/**
 * @brief Обратный перевод градусов из вещественного представления (gg.mm - цифры градусов (gg),
 * точка и доли градусов (.mm)) в формат NMEA (GGMM.MM — 2 цифры градусов (GG), 2 цифры целых
 * минут (MM), точка и дробная часть минут переменной длины (.MM))
 *
 * @param value
 * @return double
 */
double convertDegreesFromNormalToNmea(double value);

}

#endif // NMEA_H
