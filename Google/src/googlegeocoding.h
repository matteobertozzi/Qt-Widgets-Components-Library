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
#ifndef _GOOGLEGEOCODING_H_
#define _GOOGLEGEOCODING_H_

#include "googleservice.h"

#include <QObject>
#include <QPointF>
class QNetworkReply;
class QXmlStreamReader;
class THGoogleGeocodingPrivate;
class THGoogleGeoPlacemarkPrivate;

class THGoogleGeoPlacemark {
    public:
        THGoogleGeoPlacemark (QXmlStreamReader *xmlReader);

        qreal east (void) const;
        qreal west (void) const;
        qreal north (void) const;
        qreal south (void) const;
        QString address (void) const;
        QPointF coordinates (void) const;
        QString countryName (void) const;
        QString countryCode (void) const;
        QString localityName (void) const;
        QString postalCodeNumber (void) const;
        QString thoroughfareName (void) const;
        QString administrativeAreaName (void) const;
        QString subAdministrativeAreaName (void) const;

    private:
        THGoogleGeoPlacemarkPrivate *d;
};

class THGoogleGeocoding : public THGoogleService {
    Q_OBJECT

    public:
        THGoogleGeocoding (QObject *parent = 0);
        ~THGoogleGeocoding();

        QString query (void) const;
        void setQuery (const QString& query);

        bool sensor (void) const;
        void setSensor (bool sensor);

        QList<THGoogleGeoPlacemark> placemarks (void) const;

    public Q_SLOTS:
        void request (const QString& query = QString());

    protected:
        void parseResponse (const QByteArray& data);

    private:
        THGoogleGeocodingPrivate *d;
};

#endif /* !_GOOGLEGEOCODING_H_ */

