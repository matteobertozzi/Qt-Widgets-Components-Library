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

#ifndef _JSONSTREAMREADER_H_
#define _JSONSTREAMREADER_H_

#include <QIODevice>

class THJsonStreamReaderPrivate;

class THJsonStreamReader {
	public:
		enum TokenType { 
			NoToken,
			Invalid,
			PropertyNumerical,
			PropertyString,
			PropertyFalse,
			PropertyTrue,
			PropertyNull,
			Object,
			ObjectEnd,
			Array,
			ArrayEnd
		};

	public:
		THJsonStreamReader();
		THJsonStreamReader (QIODevice *device);
		THJsonStreamReader (const QByteArray& data);
		THJsonStreamReader (const QString& data);
		THJsonStreamReader (const char *data);
		~THJsonStreamReader();

		bool atEnd (void) const;
		TokenType readNext (void);

		QVariant value (void) const;
		QStringRef name (void) const;
        bool isProperty (void) const;

		TokenType tokenType (void) const;
		TokenType parentTokenType (void) const;

		void clear (void);

		QIODevice *device (void) const;
		void setDevice (QIODevice *device);

		void addData (const QByteArray& data);
		void addData (const QString& data);
		void addData (const char *data);

	private:
		THJsonStreamReaderPrivate *d;
};

#endif /* !_JSONSTREAMREADER_H_ */

