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
#include <QDebug>
#include <QUrl>

#include "jsonstreamreader.h"
#include "googletranslator.h"

/* ============================================================================
 *  PRIVATE Class
 */
class THGoogleTranslatorPrivate {
    public:
        QString srcLang;
        QString dstLang;
        QString query;

        QString detectedSourceLanguage;
        QString translatedText;
};

/* ============================================================================
 *  PUBLIC Constructors/Destructors
 */
THGoogleTranslator::THGoogleTranslator (QObject *parent)
    : THGoogleService(parent), d(new THGoogleTranslatorPrivate)
{
}

THGoogleTranslator::~THGoogleTranslator() {
    delete d;
}

/* ============================================================================
 *  PUBLIC Properties
 */
QString THGoogleTranslator::query (void) const {
    return(d->query);
}

void THGoogleTranslator::setQuery (const QString& query) {
    d->query = query;
}

QString THGoogleTranslator::sourceLanguage (void) const {
    return(d->srcLang);
}

void THGoogleTranslator::setSourceLanguage (const QString& langId) {
    d->srcLang = langId;
}

QString THGoogleTranslator::destinationLanguage (void) const {
    return(d->dstLang);
}

void THGoogleTranslator::setDestinationLanguage (const QString& langId) {
    d->dstLang = langId;
}

QString THGoogleTranslator::translatedText (void) const {
    return(d->translatedText);
}

QString THGoogleTranslator::detectedSourceLanguage (void) const {
    return(d->detectedSourceLanguage);
}

/* ============================================================================
 *  PUBLIC Slots
 */
void THGoogleTranslator::translate (const QString& query) {
    if (!query.isEmpty())
        d->query = query;

    QUrl url("http://ajax.googleapis.com/ajax/services/language/translate");
    url.addQueryItem("q", query);
    url.addQueryItem("v", "1.0");
    url.addQueryItem("langpair", d->srcLang + '|' + d->dstLang);

    if (hasHostLanguage())
        url.addQueryItem("hl", hostLanguage());

    if (hasApiKey())
        url.addQueryItem("key", apiKey());

    get(url);
}

/* ============================================================================
 *  PROTECTED Methods
 */
void THGoogleTranslator::parseResponse (const QByteArray& data) {   
    THJsonStreamReader::TokenType tokenType;
    THJsonStreamReader jsonReader(data);

    while (!jsonReader.atEnd()) {
        tokenType = jsonReader.readNext();

        if (!jsonReader.isProperty())
            continue;

        QStringRef propertyName = jsonReader.name();
        if (propertyName == "translatedText") {
            d->translatedText = jsonReader.value().toString();
        } else if (propertyName == "detectedSourceLanguage") {
            d->detectedSourceLanguage = jsonReader.value().toString();
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

