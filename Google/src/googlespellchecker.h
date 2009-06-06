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

#ifndef _GOOGLESPELLCHECKER_H_
#define _GOOGLESPELLCHECKER_H_

#include "googleservice.h"
class THGoogleSpellCheckerPrivate;

class THGoogleSpellChecker : public THGoogleService {
    Q_OBJECT

    public:
        THGoogleSpellChecker (QObject *parent = 0);
        ~THGoogleSpellChecker();

        QString query (void) const;
        void setQuery (const QString& query);

        QString language (void) const;
        void setLanguage (const QString& langId);

        QStringList keys (void) const;
        QStringList suggestions (const QString& key) const;

    Q_SIGNALS:
        void suggestion (const QString& original, 
                         const QStringList& suggestions);

    public Q_SLOTS:
        void spellCheck (const QString& query = QString());

    protected:
        void parseResponse (const QByteArray& data);

    private:
        THGoogleSpellCheckerPrivate *d;
};

#endif /* !_GOOGLESPELLCHECKER_H_ */

