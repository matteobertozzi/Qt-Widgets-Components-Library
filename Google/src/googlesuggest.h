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

#ifndef _THGOOGLESUGGEST_H_
#define _THGOOGLESUGGEST_H_

#include "googleservice.h"
class THGoogleSuggestPrivate;

class THGoogleSuggest : public THGoogleService {
	Q_OBJECT

	public:
		enum Error { NoError, HttpError };

	public:
		THGoogleSuggest (QObject *parent = 0);
		~THGoogleSuggest();

		QString query (void) const;
        void setQuery (const QString& query);

		QStringList suggestions (void) const;

	public Q_SLOTS:
		void suggest (const QString& query = QString());

	protected:
        void parseResponse (const QByteArray& data);

	private:
		THGoogleSuggestPrivate *d;
};

#endif /* !_THGOOGLESUGGEST_H_ */

