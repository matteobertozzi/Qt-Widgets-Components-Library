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

#include <QXmlStreamReader>
#include <QHash>
#include <QUrl>

#include "googleweather.h"

/* ============================================================================
 *  PRIVATE Class and Structures
 */
typedef struct {
    QString condition;
    qreal high;
    qreal low;
} THGoogleWeatherForecast;

typedef struct {
    QString condition;
    QString humidity;
    QString wind;
    qreal temp;
} THGoogleWeatherConditions;

class THGoogleWeatherPrivate {
    public:
        QHash<THGoogleWeather::DayOfWeek, THGoogleWeatherForecast> forecast;
        THGoogleWeatherConditions current;
        QString city;

    public:
        void clear (void);
        qreal toCelsius (qreal fahrenheit) const;
        THGoogleWeather::DayOfWeek parseDayOfWeek (const QStringRef& day) const;        
};

void THGoogleWeatherPrivate::clear (void) {
    current.temp = 0.0f;
    current.wind.clear();
    current.humidity.clear();
    current.condition.clear();

    forecast.clear();
}

qreal THGoogleWeatherPrivate::toCelsius (qreal fahrenheit) const {
    return(((fahrenheit - 32) * 5 + 8) / 9);
}

THGoogleWeather::DayOfWeek THGoogleWeatherPrivate::parseDayOfWeek (const QStringRef& day) const
{
    if (day == "Sun") return(THGoogleWeather::Sunday);
    if (day == "Mon") return(THGoogleWeather::Monday);
    if (day == "Tue") return(THGoogleWeather::Tuesday);
    if (day == "Wed") return(THGoogleWeather::Wednesday);
    if (day == "Thu") return(THGoogleWeather::Thursday);
    if (day == "Fri") return(THGoogleWeather::Friday);
    return(THGoogleWeather::Saturday); 
}

/* ============================================================================
 *  PUBLIC Constructors/Destructor
 */
THGoogleWeather::THGoogleWeather (QObject *parent)
    : THGoogleService(parent), d(new THGoogleWeatherPrivate)
{
}

THGoogleWeather::~THGoogleWeather() {
}

/* ============================================================================
 *  PUBLIC Properties
 */
QString THGoogleWeather::city (void) const {
    return(d->city);
}

QString THGoogleWeather::dayOfWeek (DayOfWeek day) const {
    switch (day) {
        case Sunday:    return(tr("Sunday"));
        case Monday:    return(tr("Monday"));
        case Tuesday:   return(tr("Tuesday"));
        case Wednesday: return(tr("Wednesday"));
        case Thursday:  return(tr("Thursday"));
        case Friday:    return(tr("Friday"));
        case Saturday:  return(tr("Saturday"));
    }

    return(QString());
}

QString THGoogleWeather::currentCondition (void) const {
    return(d->current.condition);
}

QString THGoogleWeather::currentHumidity (void) const {
    return(d->current.humidity);
}

QString THGoogleWeather::currentWind (void) const {
    return(d->current.wind);
}

qreal THGoogleWeather::currentTemperature (TemperatureUnit unit) const {
    if (unit == Fahrenheit)
        return(d->current.temp);
    return(d->toCelsius(d->current.temp));
}

bool THGoogleWeather::hasForecast (DayOfWeek dayOfWeek) const {
    return(d->forecast.contains(dayOfWeek));
}

QString THGoogleWeather::forecastCondition (DayOfWeek dayOfWeek) const {
    if (!d->forecast.contains(dayOfWeek))
        return(QString());
    return(d->forecast.value(dayOfWeek).condition);
}

qreal THGoogleWeather::forecastLowTemperature (DayOfWeek dayOfWeek, 
                                               TemperatureUnit unit) const
{
    if (!d->forecast.contains(dayOfWeek))
        return(0.0f);

    qreal temp = d->forecast.value(dayOfWeek).low;
    return(unit == Fahrenheit ? temp : d->toCelsius(temp));
}

qreal THGoogleWeather::forecastHighTemperature (DayOfWeek dayOfWeek,
                                                TemperatureUnit unit) const
{
    if (!d->forecast.contains(dayOfWeek))
        return(0.0f);

    qreal temp = d->forecast.value(dayOfWeek).high;
    return(unit == Fahrenheit ? temp : d->toCelsius(temp));
}

bool THGoogleWeather::hasSundayForecast (void) const {
    return(d->forecast.contains(Sunday));
}

QString THGoogleWeather::sundayCondition (void) const {
    return(forecastCondition(Sunday));
}

qreal THGoogleWeather::sundayLowTemperature (TemperatureUnit unit) const {
    return(forecastLowTemperature(Sunday, unit));
}

qreal THGoogleWeather::sundayHighTemperature (TemperatureUnit unit) const {
    return(forecastHighTemperature(Sunday, unit));
}

bool THGoogleWeather::hasMondayForecast (void) const {
    return(d->forecast.contains(Monday));
}

QString THGoogleWeather::mondayCondition (void) const {
    return(forecastCondition(Monday));
}

qreal THGoogleWeather::mondayLowTemperature (TemperatureUnit unit) const {
    return(forecastLowTemperature(Monday, unit));
}

qreal THGoogleWeather::mondayHighTemperature (TemperatureUnit unit) const {
    return(forecastHighTemperature(Monday, unit));
}

bool THGoogleWeather::hasTuesdayForecast (void) const {
    return(d->forecast.contains(Tuesday));
}

QString THGoogleWeather::tuesdayCondition (void) const {
    return(forecastCondition(Tuesday));
}

qreal THGoogleWeather::tuesdayLowTemperature (TemperatureUnit unit) const {
    return(forecastLowTemperature(Tuesday, unit));
}

qreal THGoogleWeather::tuesdayHighTemperature (TemperatureUnit unit) const {
    return(forecastHighTemperature(Tuesday, unit));
}

bool THGoogleWeather::hasWednesdayForecast (void) const {
    return(d->forecast.contains(Wednesday));
}

QString THGoogleWeather::wednesdayCondition (void) const {
    return(forecastCondition(Wednesday));
}

qreal THGoogleWeather::wednesdayLowTemperature (TemperatureUnit unit) const {
    return(forecastLowTemperature(Wednesday, unit));
}

qreal THGoogleWeather::wednesdayHighTemperature (TemperatureUnit unit) const {
    return(forecastHighTemperature(Wednesday, unit));
}

bool THGoogleWeather::hasThursdayForecast (void) const {
    return(d->forecast.contains(Thursday));
}

QString THGoogleWeather::thursdayCondition (void) const {
    return(forecastCondition(Thursday));
}

qreal THGoogleWeather::thursdayLowTemperature (TemperatureUnit unit) const {
    return(forecastLowTemperature(Thursday, unit));
}

qreal THGoogleWeather::thursdayHighTemperature (TemperatureUnit unit) const {
    return(forecastHighTemperature(Thursday, unit));
}

bool THGoogleWeather::hasFridayForecast (void) const {
    return(d->forecast.contains(Friday));
}

QString THGoogleWeather::fridayCondition (void) const {
    return(forecastCondition(Friday));
}

qreal THGoogleWeather::fridayLowTemperature (TemperatureUnit unit) const {
    return(forecastLowTemperature(Friday, unit));
}

qreal THGoogleWeather::fridayHighTemperature (TemperatureUnit unit) const {
    return(forecastHighTemperature(Friday, unit));
}

bool THGoogleWeather::hasSaturdayForecast (void) const {
    return(d->forecast.contains(Saturday));
}

QString THGoogleWeather::saturdayCondition (void) const {
    return(forecastCondition(Saturday));
}

qreal THGoogleWeather::saturdayLowTemperature (TemperatureUnit unit) const {
    return(forecastLowTemperature(Saturday, unit));
}

qreal THGoogleWeather::saturdayHighTemperature (TemperatureUnit unit) const {
    return(forecastHighTemperature(Saturday, unit));
}

void THGoogleWeather::forecast (const QString& city) {
    d->clear();

	QUrl queryUrl("http://www.google.com/ig/api");
	queryUrl.addQueryItem("hl", "en");
	queryUrl.addQueryItem("weather", city);

	get(queryUrl);
}

/* ============================================================================
 *  PROTECTED Methods
 */
void THGoogleWeather::parseResponse (const QByteArray& data) {
    QXmlStreamReader xmlReader(data);
    while (!xmlReader.atEnd()) {
        xmlReader.readNext();

        if (!xmlReader.isStartElement())
            continue;

        if (xmlReader.name() == "city") {
            d->city = xmlReader.attributes().value("data").toString();
        } else if (xmlReader.name() == "current_conditions") {
            while (!xmlReader.atEnd()) {
                xmlReader.readNext();

                if (!xmlReader.isStartElement()) {
                    if (xmlReader.name() == "current_conditions")
                        break;
                    continue;
                }

                if (xmlReader.name() == "condition")
                    d->current.condition = xmlReader.attributes().value("data").toString();
                else if (xmlReader.name() == "temp_f")
                    d->current.temp = xmlReader.attributes().value("data").toString().toFloat();
                else if (xmlReader.name() == "humidity")
                    d->current.humidity = xmlReader.attributes().value("data").toString();
                else if (xmlReader.name() == "wind_condition")
                    d->current.wind = xmlReader.attributes().value("data").toString();
            }
        } else if (xmlReader.name() == "forecast_conditions") {
            THGoogleWeatherForecast forecast;
            DayOfWeek dayOfWeek;

            while (!xmlReader.atEnd()) {
                xmlReader.readNext();

                if (!xmlReader.isStartElement()) {
                    if (xmlReader.name() == "forecast_conditions")
                        break;
                    continue;
                }

                if (xmlReader.name() == "day_of_week")
                    dayOfWeek = d->parseDayOfWeek(xmlReader.attributes().value("data"));
                else if (xmlReader.name() == "low")
                    forecast.low = xmlReader.attributes().value("data").toString().toFloat();
                else if (xmlReader.name() == "high")
                    forecast.high = xmlReader.attributes().value("data").toString().toFloat();
                else if (xmlReader.name() == "condition")
                    forecast.condition = xmlReader.attributes().value("data").toString();
            }

            d->forecast.insert(dayOfWeek, forecast);
        }
    }

    emit finished(false);
}

