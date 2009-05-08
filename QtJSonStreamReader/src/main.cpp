#include <QCoreApplication>
#include <QVariant>
#include <QDebug>
#include <QFile>

#include "jsonstreamreader.h"

int main (int argc, char **argv) {
	QCoreApplication app(argc, argv);

	QFile file("test.json");
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << file.errorString();
		return(1);
	}

	THJsonStreamReader reader(&file);
	while (!reader.atEnd()) {
		switch (reader.readNext()) {
			case THJsonStreamReader::NoToken:
				qDebug() << "No Token";
				break;
			case THJsonStreamReader::Invalid:
				qDebug() << "Invalid Token";
				break;
			case THJsonStreamReader::PropertyNumerical:
				qDebug() << " - Property Numerical" << reader.name() << reader.value();
				break;
			case THJsonStreamReader::PropertyString:
				qDebug() << " - Property String" << reader.name() << reader.value();
				break;
			case THJsonStreamReader::PropertyFalse:
				qDebug() << " - Property False" << reader.name() << reader.value();
				break;
			case THJsonStreamReader::PropertyTrue:
				qDebug() << " - Property True" << reader.name() << reader.value();
				break;
			case THJsonStreamReader::PropertyNull:
				qDebug() << " - Property Null" << reader.name();
				break;
			case THJsonStreamReader::Object:
				qDebug() << "Object" << reader.name();
				break;
			case THJsonStreamReader::ObjectEnd:
				qDebug() << "Object End";
				break;
			case THJsonStreamReader::Array:
				qDebug() << "Array" << reader.name();
				break;
			case THJsonStreamReader::ArrayEnd:
				qDebug() << "Array End";
				break;
		}
	}

	file.close();

	return(0);
}

