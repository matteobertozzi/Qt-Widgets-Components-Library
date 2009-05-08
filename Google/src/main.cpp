#include <QCoreApplication>
#include <QDebug>
#include <QHttp>
#include "googleauth.h"

int main (int argc, char **argv) {
	QCoreApplication app(argc, argv);

	QEventLoop q;
	THGoogleAuth gAuth("MYUSER@gmail.com");
	QObject::connect(&gAuth, SIGNAL(authenticated()), &q, SLOT(quit()));
	gAuth.login(THGoogleAuth::Contacts, "MYPASSWORD");
	q.exec();

	if (gAuth.error() == THGoogleAuth::NoError) {
		qDebug() << "SID" << gAuth.sid();
		qDebug() << "LSID" << gAuth.lsid();
		qDebug() << "AUTH" << gAuth.auth();
	} else {
		qDebug() << gAuth.errorString();
	}

	QEventLoop l;

	QHttpRequestHeader header("GET", "/m8/feeds/contacts/MYUSER%40gmail.com/full");
	header.setContentType("application/x-www-form-urlencoded");
	header.setValue("Authorization", "GoogleLogin auth=" + gAuth.auth());

	QHttp http("google.com");
	QObject::connect(&http, SIGNAL(done(bool)), &l, SLOT(quit()));
	http.request(header);

	l.exec();

	qDebug() << http.readAll();

	return(0);
}

