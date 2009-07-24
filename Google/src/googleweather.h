/* 
 * Copyright (C) 2009 Matteo Bertozzi.
 *
 * This file is part of THLibrary.
 * 
 * THLibrary is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * THLibrary is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with THLibrary.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _THGOOGLEWEATHER_H_
#define _THGOOGLEWEATHER_H_

#include "googleservice.h"
class THGoogleWeatherPrivate;

class THGoogleWeather : public THGoogleService {
    Q_OBJECT

    public:
        enum TemperatureUnit { Fahrenheit, Celsius };
        enum DayOfWeek { 
            Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday  
        };

    public:
        THGoogleWeather (QObject *parent = 0);
        ~THGoogleWeather();

        QString city (void) const;

        QString currentCondition (void) const;
        QString currentHumidity (void) const;
        QString currentWind (void) const;
        qreal currentTemperature (TemperatureUnit unit = Fahrenheit) const;

        QString dayOfWeek (DayOfWeek day) const;

        bool hasForecast (DayOfWeek dayOfWeek) const;
        QString forecastCondition (DayOfWeek dayOfWeek) const;
        qreal forecastLowTemperature (DayOfWeek dayOfWeek, 
                                      TemperatureUnit unit = Fahrenheit) const;
        qreal forecastHighTemperature (DayOfWeek dayOfWeek,
                                       TemperatureUnit unit = Fahrenheit) const;

        bool hasSundayForecast (void) const;
        QString sundayCondition (void) const;
        qreal sundayLowTemperature (TemperatureUnit unit = Fahrenheit) const;
        qreal sundayHighTemperature (TemperatureUnit unit = Fahrenheit) const;

        bool hasMondayForecast (void) const;
        QString mondayCondition (void) const;
        qreal mondayLowTemperature (TemperatureUnit unit = Fahrenheit) const;
        qreal mondayHighTemperature (TemperatureUnit unit = Fahrenheit) const;

        bool hasTuesdayForecast (void) const;
        QString tuesdayCondition (void) const;
        qreal tuesdayLowTemperature (TemperatureUnit unit = Fahrenheit) const;
        qreal tuesdayHighTemperature (TemperatureUnit unit = Fahrenheit) const;

        bool hasWednesdayForecast (void) const;
        QString wednesdayCondition (void) const;
        qreal wednesdayLowTemperature (TemperatureUnit unit = Fahrenheit) const;
        qreal wednesdayHighTemperature (TemperatureUnit unit = Fahrenheit) const;

        bool hasThursdayForecast (void) const;
        QString thursdayCondition (void) const;
        qreal thursdayLowTemperature (TemperatureUnit unit = Fahrenheit) const;
        qreal thursdayHighTemperature (TemperatureUnit unit = Fahrenheit) const;

        bool hasFridayForecast (void) const;
        QString fridayCondition (void) const;
        qreal fridayLowTemperature (TemperatureUnit unit = Fahrenheit) const;
        qreal fridayHighTemperature (TemperatureUnit unit = Fahrenheit) const;

        bool hasSaturdayForecast (void) const;
        QString saturdayCondition (void) const;
        qreal saturdayLowTemperature (TemperatureUnit unit = Fahrenheit) const;
        qreal saturdayHighTemperature (TemperatureUnit unit = Fahrenheit) const;

    public Q_SLOTS:
        void forecast (const QString& city);

    protected:
        void parseResponse (const QByteArray& data);

    private:
        THGoogleWeatherPrivate *d;
};

#endif /* !_THGOOGLEWEATHER_H_ */

