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

#include <QNetworkAccessManager>
#include <QXmlStreamReader>
#include <QNetworkReply>
#include <QStringList>
#include <QDebug>

#include "googlegeocoding.h"

// ===========================================================================
//  GEOPlacemark
// ===========================================================================
#define GEO_LATLONG_BOX_NORTH       0
#define GEO_LATLONG_BOX_SOUTH       1
#define GEO_LATLONG_BOX_EAST        2
#define GEO_LATLONG_BOX_WEST        3

class THGoogleGeoPlacemarkPrivate {
    public:
        QString subAdministrativeAreaName;
        QString administrativeAreaName;
        QString thoroughfareName;
        QString postalCodeNumber;
        QString localityName;
        QString countryCode;
        QString countryName;
        QPointF coordinates;
        qreal latLongBox[4];
        QString address;

    public:
        void parsePlacemark (QXmlStreamReader *xmlReader);
};

void THGoogleGeoPlacemarkPrivate::parsePlacemark (QXmlStreamReader *xmlReader) {
    if (xmlReader->name() == "address") {
        address = xmlReader->readElementText();
    } else if (xmlReader->name() == "ContryNameCode") {
        countryCode = xmlReader->readElementText();
    } else if (xmlReader->name() == "ContryName") {
        countryName = xmlReader->readElementText();
    } else if (xmlReader->name() == "AdministrativeAreaName") {
        administrativeAreaName = xmlReader->readElementText();
    } else if (xmlReader->name() == "SubAdministrativeAreaName") {
        subAdministrativeAreaName = xmlReader->readElementText();
    } else if (xmlReader->name() == "LocalityName") {
        localityName = xmlReader->readElementText();
    } else if (xmlReader->name() == "ThoroughfareName") {
        thoroughfareName = xmlReader->readElementText();
    } else if (xmlReader->name() == "PostalCodeNumber") {
        postalCodeNumber = xmlReader->readElementText();
    } else if (xmlReader->name() == "LatLonBox") {
        if (xmlReader->attributes().hasAttribute("north")) {
            QStringRef north = xmlReader->attributes().value("north");
            latLongBox[GEO_LATLONG_BOX_NORTH] = north.toString().toDouble();
        }

        if (xmlReader->attributes().hasAttribute("south")) {
            QStringRef south = xmlReader->attributes().value("south");
            latLongBox[GEO_LATLONG_BOX_SOUTH] = south.toString().toDouble();
        }

        if (xmlReader->attributes().hasAttribute("west")) {
            QStringRef west = xmlReader->attributes().value("west");
            latLongBox[GEO_LATLONG_BOX_WEST] = west.toString().toDouble();
        }

        if (xmlReader->attributes().hasAttribute("east")) {
            QStringRef east = xmlReader->attributes().value("east");
            latLongBox[GEO_LATLONG_BOX_EAST] = east.toString().toDouble();
        }
    } else if (xmlReader->name() == "coordinates") {
        QStringList coord = xmlReader->readElementText().split(',',
                                                    QString::SkipEmptyParts);
        if (coord.size() >= 2) {
            coordinates.setX(coord[0].toDouble());
            coordinates.setY(coord[1].toDouble());
        }
    }
}

THGoogleGeoPlacemark::THGoogleGeoPlacemark (QXmlStreamReader *xmlReader) 
    : d(new THGoogleGeoPlacemarkPrivate)
{
    while (!xmlReader->atEnd()) {
        xmlReader->readNext();

        if (xmlReader->isEndElement() && xmlReader->name() == "Placemark")
            break;

        if (xmlReader->isStartElement())
            d->parsePlacemark(xmlReader);
    }
}


qreal THGoogleGeoPlacemark::east (void) const {
    return(d->latLongBox[GEO_LATLONG_BOX_EAST]);
}

qreal THGoogleGeoPlacemark::west (void) const {
    return(d->latLongBox[GEO_LATLONG_BOX_WEST]);
}

qreal THGoogleGeoPlacemark::north (void) const {
    return(d->latLongBox[GEO_LATLONG_BOX_NORTH]);
}

qreal THGoogleGeoPlacemark::south (void) const {
    return(d->latLongBox[GEO_LATLONG_BOX_SOUTH]);
}

QString THGoogleGeoPlacemark::address (void) const {
    return(d->address);
}

QPointF THGoogleGeoPlacemark::coordinates (void) const {
    return(d->coordinates);
}

QString THGoogleGeoPlacemark::countryName (void) const {
    return(d->countryName);
}

QString THGoogleGeoPlacemark::countryCode (void) const {
    return(d->countryCode);
}

QString THGoogleGeoPlacemark::localityName (void) const {
    return(d->localityName);
}

QString THGoogleGeoPlacemark::postalCodeNumber (void) const {
    return(d->postalCodeNumber);
}

QString THGoogleGeoPlacemark::thoroughfareName (void) const {
    return(d->thoroughfareName);
}

QString THGoogleGeoPlacemark::administrativeAreaName (void) const {
    return(d->administrativeAreaName);
}

QString THGoogleGeoPlacemark::subAdministrativeAreaName (void) const {
    return(d->subAdministrativeAreaName);
}

// ===========================================================================
//  GeoCoding
// ===========================================================================
class THGoogleGeocodingPrivate {
    public:
        QList<THGoogleGeoPlacemark> geoPlacemarks;
        QString query;
        bool sensor;

    public:
        QString statusCodeToMessage (int statusCode);
};

QString THGoogleGeocodingPrivate::statusCodeToMessage (int statusCode) {
    switch (statusCode) {
        case 500: return(QObject::tr("Server Error"));
        case 601: return(QObject::tr("Missing Query"));
        case 602: return(QObject::tr("Unknown Address"));
        case 603: return(QObject::tr("Unavailable Address"));
        case 610: return(QObject::tr("Bad Key"));
        case 620: return(QObject::tr("Too Many Queries"));
    }

    return(QString());
}

THGoogleGeocoding::THGoogleGeocoding (QObject *parent)
    : THGoogleService(parent), d(new THGoogleGeocodingPrivate)
{
    d->sensor = false;
}

THGoogleGeocoding::~THGoogleGeocoding() {
    delete d;
}

QString THGoogleGeocoding::query (void) const {
    return(d->query);
}

void THGoogleGeocoding::setQuery (const QString& query) {
    d->query = query;
}


bool THGoogleGeocoding::sensor (void) const {
    return(d->sensor);
}

void THGoogleGeocoding::setSensor (bool sensor) {
    d->sensor = sensor;
}

QList<THGoogleGeoPlacemark> THGoogleGeocoding::placemarks (void) const {
    return(d->geoPlacemarks);
}

void THGoogleGeocoding::request (const QString& query) {
    if (!query.isEmpty())
        d->query = query;

    QUrl url("http://maps.google.com/maps/geo");
    url.addQueryItem("q", d->query);
    url.addQueryItem("sensor", d->sensor ? "true" : "false");
    url.addQueryItem("output", "xml");
    url.addQueryItem("oe", "utf8");

    if (hasApiKey())
        url.addQueryItem("key", apiKey());

    //url.addQueryItem("ll", QString("%1,%2").arg(d->latitude).arg(d->longitude));
    //url.addQueryItem("spn", ...
    //url.addQueryItem("gl", ...

    get(url);
}

void THGoogleGeocoding::parseResponse (const QByteArray& data) {
    QXmlStreamReader xmlReader(data);
    while (!xmlReader.atEnd()) {
        xmlReader.readNext();

        if (!xmlReader.isStartElement())
            continue;

        if (xmlReader.name() == "Status") {
            while (!xmlReader.atEnd()) {
                xmlReader.readNext();

                QStringRef name = xmlReader.name();
                if (xmlReader.isEndElement() && name == "Status")
                    break;

                if (xmlReader.isStartElement() && name == "Code") {
                    int statusCode = xmlReader.readElementText().toInt();
                    setResponseStatus(statusCode);
                    setErrorString(d->statusCodeToMessage(statusCode));
                    break;
                }
            }
        } else if (xmlReader.name() == "Placemark") {
            d->geoPlacemarks.append(THGoogleGeoPlacemark(&xmlReader));
        }
    }
    qDebug() << "PARSE FINISHED";
    emit finished(responseStatus() != 200);
}

