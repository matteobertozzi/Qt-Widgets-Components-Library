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

#include <QVariant>
#include <QUrl>

#include "jsonstreamreader.h"
#include "googledetectlanguage.h"

/* ============================================================================
 *  PRIVATE Class
 */
class THGoogleDetectLanguagePrivate {
    public:
        QString query;

        QString language;
        qreal confidence;
        bool isReliable;
};

/* ============================================================================
 *  PUBLIC Constructors/Destructors
 */
THGoogleDetectLanguage::THGoogleDetectLanguage (QObject *parent)
    : THGoogleService(parent), d(new THGoogleDetectLanguagePrivate)
{
}

THGoogleDetectLanguage::~THGoogleDetectLanguage() {
    delete d;
}

/* ============================================================================
 *  PUBLIC Properties
 */
QString THGoogleDetectLanguage::query (void) const {
    return(d->query);
}

void THGoogleDetectLanguage::setQuery (const QString& query) {
    d->query = query;
}

bool THGoogleDetectLanguage::isReliable (void) const {
    return(d->isReliable);
}

qreal THGoogleDetectLanguage::confidence (void) const {
    return(d->confidence);
}

QString THGoogleDetectLanguage::language (void) const {
    return(d->language);
}

/* ============================================================================
 *  PUBLIC Slots
 */
void THGoogleDetectLanguage::detectLanguage (const QString& query) {
    if (!query.isEmpty())
        d->query = query;

    QUrl url("http://ajax.googleapis.com/ajax/services/language/detect");
    url.addQueryItem("q", query);
    url.addQueryItem("v", "1.0");

    if (!hostLanguage().isEmpty())
        url.addQueryItem("hl", hostLanguage());

    if (!apiKey().isEmpty())
        url.addQueryItem("key", apiKey());

    get(url);
}

/* ============================================================================
 *  PROTECTED Methods
 */
void THGoogleDetectLanguage::parseResponse (const QByteArray& data) {   
    THJsonStreamReader::TokenType tokenType;
    THJsonStreamReader jsonReader(data);

    while (!jsonReader.atEnd()) {
        tokenType = jsonReader.readNext();

        if (!jsonReader.isProperty())
            continue;

        QStringRef propertyName = jsonReader.name();
        if (propertyName == "language") {
            d->language = jsonReader.value().toString();
        } else if (propertyName == "isReliable") {
            d->isReliable = jsonReader.value().toBool();
        } else if (propertyName == "confidence") {
            d->confidence = jsonReader.value().toDouble();
        } else if (propertyName == "responseStatus") {
            setResponseStatus(jsonReader.value().toInt());
        } else if (propertyName == "responseDetails" &&
                   tokenType != THJsonStreamReader::PropertyNull)
        {
            setErrorString(jsonReader.value().toString());
        }
    }

    emit finished(responseStatus() != 200);
}

