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
#include <QStringList>
#include <QUrl>

#include "googlesuggest.h"

/* ============================================================================
 *  PRIVATE Class
 */
class THGoogleSuggestPrivate {
	public:
		QStringList suggestions;
		QString query;
};

/* ============================================================================
 *  PUBLIC Constructors/Destructor
 */
THGoogleSuggest::THGoogleSuggest (QObject *parent) 
	: THGoogleService(parent), d(new THGoogleSuggestPrivate)
{
}

THGoogleSuggest::~THGoogleSuggest() {
	delete d;
}

/* ============================================================================
 *  PUBLIC Properties
 */
QString THGoogleSuggest::query (void) const {
	return(d->query);
}

void THGoogleSuggest::setQuery (const QString& query) {
    d->query = query;
}

QStringList THGoogleSuggest::suggestions (void) const {
	return(d->suggestions);
}

/* ============================================================================
 *  PUBLIC Slots
 */
void THGoogleSuggest::suggest (const QString& query) {
    if (!query.isEmpty())
    	d->query = query;

	QUrl queryUrl("http://www.google.com/complete/search");
	queryUrl.addQueryItem("output", "toolbar");
	queryUrl.addQueryItem("q", d->query);

	get(queryUrl);
}

/* ============================================================================
 *  PRIVATE Slots
 */
void THGoogleSuggest::parseResponse (const QByteArray& data) {
    QXmlStreamReader xmlReader(data);
    while (!xmlReader.atEnd()) {
        xmlReader.readNext();

        if (!xmlReader.isStartElement())
            continue;

        if (xmlReader.name() == "suggestion") {
            QStringRef item = xmlReader.attributes().value("data");
            d->suggestions << item.toString();
        }
    }

    emit finished(true);
}

