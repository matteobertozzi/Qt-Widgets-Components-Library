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

#ifndef _GOOGLETRANSLATOR_H_
#define _GOOGLETRANSLATOR_H_

#include "googleservice.h"
class THGoogleTranslatorPrivate;

class THGoogleTranslator : public THGoogleService {
    Q_OBJECT

    public:
        THGoogleTranslator (QObject *parent = 0);
        ~THGoogleTranslator();

        QString query (void) const;
        void setQuery (const QString& query);

        QString sourceLanguage (void) const;
        void setSourceLanguage (const QString& langId);

        QString destinationLanguage (void) const;
        void setDestinationLanguage (const QString& langId);

        QString translatedText (void) const;
        QString detectedSourceLanguage (void) const;

    public Q_SLOTS:
        void translate (const QString& query = QString());

    protected:
        void parseResponse (const QByteArray& data);

    private:
        THGoogleTranslatorPrivate *d;
};

#endif /* !_GOOGLETRANSLATOR_H_ */

