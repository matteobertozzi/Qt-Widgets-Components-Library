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
#include <QBuffer>
#include <QStack>

#include "jsonstreamreader.h"

/* ============================================================================
 *  PRIVATE Class
 */
class THJsonStreamReaderPrivate {
	public:
		enum TokenType {
			NoToken,
			CurlyOpen, CurlyClose,
			SquaredOpen, SquaredClose,
			Colon, Comma,
			String, Number, True, False, Null
		};

	public:
		QStack<THJsonStreamReader::TokenType> parentToken;
		THJsonStreamReader::TokenType lastToken;
		QIODevice *device;
		bool useBuffer;
		QVariant value;
		QString name;

	public:
		THJsonStreamReaderPrivate();

		bool isWhitespace (char c);

		bool readNextIfIs (char k, char *pc = NULL);
		bool readNextIfIs (const char *chrs, int size, char *pc = NULL);

		TokenType readNext (void);

		QString parseString (void);
		QVariant parseNumber (void);

		THJsonStreamReader::TokenType parseValue (void);
};

THJsonStreamReaderPrivate::THJsonStreamReaderPrivate() {
	device = NULL;
	useBuffer = false;
	lastToken = THJsonStreamReader::NoToken;
}

bool THJsonStreamReaderPrivate::isWhitespace (char c) {
	return(c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

bool THJsonStreamReaderPrivate::readNextIfIs (char k, char *pc) {
	char c;

	if (!device->getChar(&c))
		return(false);

	if (c != k) {
		device->ungetChar(k);
		return(false);
	}

	if (pc != NULL) *pc = c;
	return(true);
}

bool THJsonStreamReaderPrivate::readNextIfIs (const char *chrs, int size, char *pc)
{
	char c;

	if (!device->getChar(&c))
		return(false);

	for (int i = 0; i < size; ++i) {
		if (c == chrs[i]) {
			if (pc != NULL) *pc = c;
			return(true);
		}
	}

	device->ungetChar(c);
	return(false);
}

THJsonStreamReaderPrivate::TokenType THJsonStreamReaderPrivate::readNext (void) 
{
	char c;

	/* Skip Whitespaces */
	do {
		if (!device->getChar(&c))
			return(NoToken);
	} while (isWhitespace(c));

	/* Match Token */
	switch (c) {
		case '{':
			return(CurlyOpen);
		case '}':
			return(CurlyClose);
		case '[':
			return(SquaredOpen);
		case ']':
			return(SquaredClose);
		case ',':
			return(Comma);
		case '"':
			device->ungetChar(c);
			return(String);
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		case '-':
			device->ungetChar(c);
			return(Number);
		case ':':
			return(Colon);
		case 't':
			if (!readNextIfIs('r')) return(NoToken);
			if (!readNextIfIs('u')) return(NoToken);
			if (!readNextIfIs('e')) return(NoToken);
			return(True);
		case 'f':
			if (!readNextIfIs('a')) return(NoToken);
			if (!readNextIfIs('l')) return(NoToken);
			if (!readNextIfIs('s')) return(NoToken);
			if (!readNextIfIs('e')) return(NoToken);
			return(False);
		case 'n':
			if (!readNextIfIs('u')) return(NoToken);
			if (!readNextIfIs('l')) return(NoToken);
			if (!readNextIfIs('l')) return(NoToken);
			return(Null);
	}

	return(NoToken);
}

QString THJsonStreamReaderPrivate::parseString (void) {
	bool completed = false;
	QString s;
	char c;

	/* Skip Whitespaces */
	do {
		if (!device->getChar(&c))
			return(QString());
	} while (isWhitespace(c));

	do {
		if (!device->getChar(&c))
			break;

		if (c == '"') {
			completed = true;
			break;
		} else if (c == '\\') {
			if (!device->getChar(&c))
				break;

			if (c == 'u') {
				quint32 code;
				if (device->read((char *)&code, 4) != 4)
					break;
				s += QChar((uint)code);	
			} else {
				switch (c) {
					case '"':
						s += '"';
						break;
					case '\\':
						s += '\\';
						break;
					case '/':
						s += '/';
						break;
					case 'b':
						s += '\b';
						break;
					case 'f':
						s += '\f';
						break;
					case 'n':
						s += '\n';
						break;
					case 'r':
						s += '\r';
						break;
					case 't':
						s += 't';
						break;
				}
			}
		} else {
			s += c;
		}
	} while (!completed);

	return(completed ? s : QString());
}

QVariant THJsonStreamReaderPrivate::parseNumber (void) {
	char numberChrs[] = "0123456789+-.eE";
	QString s;
	char c;

	while (readNextIfIs(numberChrs, sizeof(numberChrs) / sizeof(char), &c))
		s += c;

	if (s.contains('.')) {
		double value;
		bool ok;
		value = s.toDouble(&ok);
		if (ok) return(value);
	} else {
		qlonglong value;
		bool ok;
		value = s.toLongLong(&ok);
		if (ok) return(value);		
	}

	return(QVariant());
}

THJsonStreamReader::TokenType THJsonStreamReaderPrivate::parseValue (void) {
	switch (readNext()) {
		case String:
			value = parseString();
			return(THJsonStreamReader::PropertyString);
		case Number:
			value = parseNumber();
			return(THJsonStreamReader::PropertyNumerical);
		case True:
			value = true;
			return(THJsonStreamReader::PropertyTrue);
		case False:
			value = false;
			return(THJsonStreamReader::PropertyFalse);
		case Null:
			value.clear();
			return(THJsonStreamReader::PropertyNull);
		case CurlyOpen:
			return(THJsonStreamReader::Object);
		case CurlyClose:
			return(THJsonStreamReader::ObjectEnd);
		case SquaredOpen:
			return(THJsonStreamReader::Array);
		case SquaredClose:
			return(THJsonStreamReader::ArrayEnd);
		case NoToken:
			return(THJsonStreamReader::NoToken);
		default:
			break;
	}

	return(THJsonStreamReader::Invalid);
}

/* ============================================================================
 *  PUBLIC Constructors/Destructor
 */
THJsonStreamReader::THJsonStreamReader()
	: d(new THJsonStreamReaderPrivate)
{
}

THJsonStreamReader::THJsonStreamReader (QIODevice *device)
	: d(new THJsonStreamReaderPrivate)
{
	setDevice(device);
}

THJsonStreamReader::THJsonStreamReader (const QByteArray& data)
	: d(new THJsonStreamReaderPrivate)
{
	addData(data);
}

THJsonStreamReader::THJsonStreamReader (const QString& data)
	: d(new THJsonStreamReaderPrivate)
{
	addData(data);
}

THJsonStreamReader::THJsonStreamReader (const char *data)
	: d(new THJsonStreamReaderPrivate)
{
	addData(data);
}

THJsonStreamReader::~THJsonStreamReader() {
	clear();
	delete d;
}

/* ============================================================================
 *  PUBLIC Methods
 */
bool THJsonStreamReader::atEnd (void) const {
	return(	d->device == NULL || d->device->atEnd() || 
			((d->lastToken == ObjectEnd || d->lastToken == Array) && 
			d->parentToken.empty()));
}

THJsonStreamReader::TokenType THJsonStreamReader::readNext (void) {
	/* Clear Name and Values */
	d->name.clear();
	d->value.clear();

	/* Read Internal Token */
	THJsonStreamReaderPrivate::TokenType internalToken = d->readNext();
	if (internalToken == THJsonStreamReaderPrivate::CurlyOpen) {
		d->lastToken = Object;
	} else if (internalToken == THJsonStreamReaderPrivate::CurlyClose) {
		d->lastToken = ObjectEnd;
	} else if (internalToken == THJsonStreamReaderPrivate::SquaredOpen) {
		d->lastToken = Array;
	} else if (internalToken == THJsonStreamReaderPrivate::SquaredClose) {
		d->lastToken = ArrayEnd;
	} else if (d->parentToken.top() == Array) {
		d->lastToken = d->parseValue();
	} else if (d->parentToken.top() == Object) {
		d->name = d->parseString();
		if (d->readNext() != THJsonStreamReaderPrivate::Colon)
			return(Invalid);
		d->lastToken = d->parseValue();
	} else {
		d->lastToken = NoToken;
	}

	/* Evaluate Open/Close Token */
	if (d->lastToken == Array || d->lastToken == Object)
		d->parentToken.push(d->lastToken);
	else if (d->lastToken == ArrayEnd || d->lastToken == ObjectEnd)
		d->parentToken.pop();

	return(d->lastToken);
}

/* ============================================================================
 *  PUBLIC Methods/Properties (Data Related)
 */
void THJsonStreamReader::clear (void) {
	if (d->device == NULL)
		return;

	if (d->useBuffer)
		delete d->device;

	d->useBuffer = false;
	d->device = NULL;
}

QIODevice *THJsonStreamReader::device (void) const {
	return(d->useBuffer ? NULL : d->device);
}

void THJsonStreamReader::setDevice (QIODevice *device) {
	if (d->useBuffer) delete d->device;

	d->useBuffer = false;
	d->device = device;
}

void THJsonStreamReader::addData (const QByteArray& data) {
	if (d->useBuffer) delete d->device;

	QBuffer *buffer = new QBuffer;
	buffer->setData(data);
	d->useBuffer = true;
	d->device = buffer;
}

void THJsonStreamReader::addData (const QString& data) {
	if (d->useBuffer) delete d->device;

	QBuffer *buffer = new QBuffer;
	buffer->setData(data.toLatin1());
	d->useBuffer = true;
	d->device = buffer;
}

void THJsonStreamReader::addData (const char *data) {
	if (d->useBuffer) delete d->device;

	QBuffer *buffer = new QBuffer;
	buffer->setData(data);
	d->useBuffer = true;
	d->device = buffer;
}

/* ============================================================================
 *  PUBLIC Properties
 */
QVariant THJsonStreamReader::value (void) const {
	return(d->value);
}

QStringRef THJsonStreamReader::name (void) const {
	return(QStringRef(&(d->name)));
}

THJsonStreamReader::TokenType THJsonStreamReader::tokenType (void) const {
	return(d->lastToken);
}

THJsonStreamReader::TokenType THJsonStreamReader::parentTokenType (void) const {
	return(d->parentToken.empty() ? NoToken : d->parentToken.top());
}

