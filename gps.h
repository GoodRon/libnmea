/*
 * Copyright (c) 2014, Roman Meyta <theshrodingerscat@gmail.com>
 * All rights reserved
 */

#ifndef GPS_H
#define GPS_H

#include <cmath>
#include <ctime>

#define RADIAN (M_PI / 180.0)
#define NAUTICAL_MILE 1852

/**
 * @brief Структура навигационных данных
 */
struct GpsData {
	/**
	 * @brief Конструктор
	 */
	GpsData() : actuality(false), latitude(0.0), northLatitude(true), longitude(0.0),
		eastLongitude(true), altitude(0.0), speed(0.0), direction(0.0), hdop(99.0), vdop(99.0),
		satellites(0), timestamp(0) {}

	/**
	 * @brief Очистить структуру
	 *
	 * @return void
	 */
	void reset() {
		actuality = false;
		latitude = 0.0;
		northLatitude = true;
		longitude = 0.0;
		eastLongitude = true;
		altitude = 0.0;
		speed = 0.0;
		direction = 0.0;
		hdop = 0.0;
		vdop = 0.0;
		satellites = 0;
		timestamp = 0;
	}

	/**
	 * @brief Признак актуальности
	 */
	bool actuality;

	/**
	 * @brief Широта в формате NMEA
	 * Формат данных GGMM.MM — 2 цифры градусов (GG), 2 цифры целых минут (MM),
	 * точка и дробная часть минут переменной длины (.MM). Лидирующие нули не опускаются
	 */
	double latitude;

	/**
	 * @brief Признак северной широты
	 */
	bool northLatitude;

	/**
	 * @brief Долгота в формате NMEA
	 * Формат данных GGMM.MM — 2 цифры градусов (GG), 2 цифры целых минут (MM),
	 * точка и дробная часть минут переменной длины (.MM). Лидирующие нули не опускаются
	 */
	double longitude;

	/**
	 * @brief Признак восточной долготы
	 */
	bool eastLongitude;

	/**
	 * @brief Высота над уровнем моря
	 */
	double altitude;

	/**
     * @brief Скорость км/ч
	 */
	double speed;

	/**
	 * @brief Угол направления
	 */
	double direction;

	/**
	 * @brief Снижение точности в горизонтальной плоскости
	 */
	double hdop;

	/**
	 * @brief Снижение точности в вертикальной плоскости
	 */
	double vdop;

	/**
	 * @brief Количество видимых спутников
	 */
	unsigned satellites;

	/**
	 * @brief UTC время
	 */
	time_t timestamp;
};

#endif  // GPS_H
