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
#include <QHash>
#include <QUrl>

#include "googlespellchecker.h"

/* ============================================================================
 *  PRIVATE Class
 */
class THGoogleSpellCheckerPrivate {
    public:
        QHash<QString, QStringList> spellSuggestions;
        QString language;
        QString query;
};

/* ============================================================================
 *  PUBLIC Constructors/Destructors
 */
THGoogleSpellChecker::THGoogleSpellChecker (QObject *parent)
    : THGoogleService(parent), d(new THGoogleSpellCheckerPrivate)
{
}

THGoogleSpellChecker::~THGoogleSpellChecker() {
    delete d;
}

/* ============================================================================
 *  PUBLIC Properties
 */
QString THGoogleSpellChecker::query (void) const {
    return(d->query);
}

void THGoogleSpellChecker::setQuery (const QString& query) {
    d->query = query;
}

QString THGoogleSpellChecker::language (void) const {
    return(d->language);
}

void  THGoogleSpellChecker::setLanguage (const QString& langId) {
    d->language = langId;
}

QStringList THGoogleSpellChecker::keys (void) const {
    return(d->spellSuggestions.keys());
}

QStringList THGoogleSpellChecker::suggestions (const QString& key) const {
    if (d->spellSuggestions.contains(key))
        return(d->spellSuggestions.value(key));
    return(QStringList());
}

/* ============================================================================
 *  PUBLIC Slots
 */
void THGoogleSpellChecker::spellCheck (const QString& query) {
    if (!query.isEmpty())
        d->query = query;

    QUrl url("https://www.google.com/tbproxy/spell?");
    url.addQueryItem("lang", d->language.isEmpty() ? "en" : d->language);

    if (!hostLanguage().isEmpty())
        url.addQueryItem("hl", hostLanguage());

    if (!apiKey().isEmpty())
        url.addQueryItem("key", apiKey());

    QByteArray data;
    data  = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
    data += "<spellrequest ";
    data += QString("textalreadyclipped=\"%1\" ").arg(0);
    data += QString("ignoredups=\"%1\" ").arg(0);
    data += QString("ignoredigits=\"%1\" ").arg(1);
    data += QString("ignoreallcaps=\"%1\">").arg(1);
    data += QString("<text>%1</text>").arg(d->query);
    data += "</spellrequest>";

    d->spellSuggestions.clear();
    post(url, data);
}

/* ============================================================================
 *  PROTECTED Methods
 */
void THGoogleSpellChecker::parseResponse (const QByteArray& data) {
    QXmlStreamReader xmlReader(data);

    while (!xmlReader.atEnd()) {
        xmlReader.readNext();

        if (!xmlReader.isStartElement())
            continue;

        if (xmlReader.name() == "c") {
            int offset = 0;
            int length = 0;

            QXmlStreamAttributes attributes = xmlReader.attributes();
            if (attributes.hasAttribute("o"))
                offset = attributes.value("o").toString().toInt();

            if (attributes.hasAttribute("l"))
                length = attributes.value("l").toString().toInt();

            QString original = d->query.mid(offset, length);
            QStringList suggestList = xmlReader.readElementText().split('\t');
            d->spellSuggestions.insert(original, suggestList);

            emit suggestion(original, suggestList);
        }
    }

    emit finished(true);
}

