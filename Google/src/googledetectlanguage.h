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

#ifndef _GOOGLEDETECTLANGUAGE_H_
#define _GOOGLEDETECTLANGUAGE_H_

#include "googleservice.h"
class THGoogleDetectLanguagePrivate;

class THGoogleDetectLanguage : public THGoogleService {
    Q_OBJECT

    public:
        THGoogleDetectLanguage (QObject *parent = 0);
        ~THGoogleDetectLanguage();

        QString query (void) const;
        void setQuery (const QString& query);

        bool isReliable (void) const;
        qreal confidence (void) const;
        QString language (void) const;

    public Q_SLOTS:
        void detectLanguage (const QString& query = QString());

    protected:
        void parseResponse (const QByteArray& data);

    private:
        THGoogleDetectLanguagePrivate *d;
};

#endif /* !_GOOGLEDETECTLANGUAGE_H_ */

