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

#include <QCoreApplication>
#include <QHttp>

#include "googleauth.h"

/* ============================================================================
 *  PRIVATE Class
 */
class THGoogleAuthPrivate {
	public:
		THGoogleAuth::Service service;
		THGoogleAuth::Error error;
		QString errorString;
		QString mail;
		QString auth;
		QString lsid;
		QString sid;

		QHttp *http;

	public:
		QString serviceName (THGoogleAuth::Service service);
		THGoogleAuth::Error authError (const QString& error);
		QString errorMessage (void) const;
		QString source (void) const;
};

/*
 * http://code.google.com/apis/gdata/faq.html#clientlogin
 */
QString THGoogleAuthPrivate::serviceName (THGoogleAuth::Service service) {
	switch (service) {
		case THGoogleAuth::GoogleAppsProvisioning: 	return("apps");
		case THGoogleAuth::GoogleBase: 				return("gbase");
		case THGoogleAuth::Blogger: 				return("blogger");
		case THGoogleAuth::BookSearch: 				return("print");
		case THGoogleAuth::Calendar: 				return("cl");
		case THGoogleAuth::GoogleCodeSearch:		return("codesearch");
		case THGoogleAuth::Contacts: 				return("cp");
		case THGoogleAuth::DocumentsList: 			return("writely");
		case THGoogleAuth::Finance: 				return("finance");
		case THGoogleAuth::GmailAtomFeed: 			return("mail");
		case THGoogleAuth::Health: 					return("health");
		case THGoogleAuth::Notebook: 				return("notebook");
		case THGoogleAuth::PicasaWebAlbums: 		return("lh2");
		case THGoogleAuth::Spreadsheets: 			return("wise");
		case THGoogleAuth::WebmasterTools: 			return("sitemaps");
		case THGoogleAuth::YouTube: 				return("youtube");
	}

	return("xapi");
}

THGoogleAuth::Error THGoogleAuthPrivate::authError (const QString& error) {
	if (error == "BadAuthentication") 	return(THGoogleAuth::BadAuthentication);
	if (error == "NotVerified") 		return(THGoogleAuth::NotVerified);
	if (error == "TermsNotAgreed") 		return(THGoogleAuth::TermsNotAgreed);
	if (error == "CaptchaRequired") 	return(THGoogleAuth::CaptchaRequired);
	if (error == "AccountDeleted") 		return(THGoogleAuth::AccountDeleted);
	if (error == "AccountDisabled") 	return(THGoogleAuth::AccountDisabled);
	if (error == "ServiceDisabled") 	return(THGoogleAuth::ServiceDisabled);
	if (error == "ServiceUnavailable") 	return(THGoogleAuth::ServiceUnavailable);
	return(THGoogleAuth::Unknown);
}

QString THGoogleAuthPrivate::errorMessage (void) const {
	if (error == THGoogleAuth::NoError)
		return(QString());

	if (error == THGoogleAuth::BadAuthentication) {
		return(QObject::tr(	"The login request used a username or password "
							"that is not recognized."));
	} else if (error == THGoogleAuth::NotVerified) {
		return(QObject::tr(	"The account email address has not been verified. "
							"The user will need to access their Google account "
							"directly to resolve the issue before logging in "
							"using a non-Google application."));
	} else if (error == THGoogleAuth::TermsNotAgreed) {
		return(QObject::tr(	"The user has not agreed to terms. "
							"The user will need to access their Google account "
							"directly to resolve the issue before logging in "
							"using a non-Google application."));
	} else if (error == THGoogleAuth::CaptchaRequired) {
		return(QObject::tr("A CAPTCHA is required."));
	} else if (error == THGoogleAuth::AccountDeleted) {
		return(QObject::tr("The user account has been deleted."));
	} else if (error == THGoogleAuth::AccountDisabled) {
		return(QObject::tr("The user account has been disabled."));
	} else if (error == THGoogleAuth::ServiceDisabled) {
		return(QObject::tr(	"The user's access to the specified service has been" 								" disabled. (The user account may still be valid.)"));
	} else if (error == THGoogleAuth::ServiceUnavailable) {
		return(QObject::tr("The service is not available; try again later."));
	} else if (error == THGoogleAuth::Unknown) {
		return(QObject::tr(	"The error is unknown or unspecified; the request "
							"contained invalid input or was malformed."));
	}

	return(errorString);
}

QString THGoogleAuthPrivate::source (void) const {
	/* Short string identifying your application, for logging purposes. 
	 * This string should take the form: 
	 *         "companyName-applicationName-versionID". 
	 */
	return(	QCoreApplication::organizationName() + "-" + 
			QCoreApplication::applicationName() + "-" + 
			QCoreApplication::applicationVersion());
}

/* ============================================================================
 *  PUBLIC Constructors/Destructor
 */
THGoogleAuth::THGoogleAuth (QObject *parent)
	: QObject(parent), d(new THGoogleAuthPrivate)
{
	d->http = NULL;
	d->error = THGoogleAuth::NoError;
}

THGoogleAuth::THGoogleAuth (const QString& mail, QObject *parent)
	: QObject(parent), d(new THGoogleAuthPrivate)
{
	d->http = NULL;
	d->mail = mail;
	d->error = THGoogleAuth::NoError;
}

THGoogleAuth::~THGoogleAuth() {
	if (d->http != NULL)
		d->http->deleteLater();
	delete d;
}

/* ============================================================================
 *  PUBLIC Properties
 */
const QString& THGoogleAuth::mail (void) const {
	return(d->mail);
}

void THGoogleAuth::setMail (const QString& mail) {
	d->mail = mail;
}

THGoogleAuth::Service THGoogleAuth::service (void) const {
	return(d->service);
}

void THGoogleAuth::setService (THGoogleAuth::Service service) {
	d->service = service;
}

/* ============================================================================
 *  PUBLIC Properties (Auth Related)
 */
THGoogleAuth::Error THGoogleAuth::error (void) const {
	return(d->error);
}

QString THGoogleAuth::errorString (void) const {
	return(d->errorMessage());
}

const QString& THGoogleAuth::sid (void) const {
	return(d->sid);
}

const QString& THGoogleAuth::lsid (void) const {
	return(d->lsid);
}

const QString& THGoogleAuth::auth (void) const {
	return(d->auth);
}

/* ============================================================================
 *  PUBLIC Slots
 */
void THGoogleAuth::login (	const QString& password,
							const QString& loginToken,
							const QString& loginCaptcha)
{
	if (d->http != NULL)
		return;

	d->error = THGoogleAuth::NoError;
	d->errorString.clear();

	QHttpRequestHeader header("POST", "/accounts/ClientLogin");
	header.setContentType("application/x-www-form-urlencoded");

	QByteArray query;
	query += "accountType=HOSTED_OR_GOOGLE";
	query += "&Email=" + d->mail;
	query += "&Passwd=" + password;
	query += "&source=" + d->source();
	query += "&service=" + d->serviceName(d->service);
	if (!loginToken.isEmpty()) query += "&logintoken=" + loginToken;
	if (!loginCaptcha.isEmpty()) query += "&logintoken=" + loginCaptcha;
	
	d->http = new QHttp("google.com", QHttp::ConnectionModeHttps);
	connect(d->http, SIGNAL(done(bool)), this, SLOT(authDone(bool)));
	d->http->request(header, query);
}

void THGoogleAuth::login (	Service service,
							const QString& password,
							const QString& loginToken,
							const QString& loginCaptcha)
{
	d->service = service;
	login(password, loginToken, loginCaptcha);
}

/* ============================================================================
 *  PROTECTED Slots
 */
void THGoogleAuth::authDone (bool error) {
	QString captchaToken;
	QString captchaUrl;

	if (error) {
		d->error = HttpError;
		d->errorString = d->http->errorString();
	} else {
		QList<QByteArray> response = d->http->readAll().split('\n');
		for (int i = 0; i < response.size(); ++i) {
			const QByteArray& responseLine = response.at(i);

			if (responseLine.startsWith("SID="))
				d->sid = responseLine.mid(4);
			else if (responseLine.startsWith("LSID="))
				d->lsid = responseLine.mid(5);
			else if (responseLine.startsWith("Auth="))
				d->auth = responseLine.mid(5);
			else if (responseLine.startsWith("Error="))
				d->error = d->authError(responseLine.mid(6));
			else if (responseLine.startsWith("CaptchaToken="))
				captchaToken = responseLine.mid(13);
			else if (responseLine.startsWith("CaptchaUrl="))
				captchaUrl = responseLine.mid(11);
		}
	}

	/* Destroy Http */
	d->http->deleteLater();
	d->http = NULL;

	/* Emit Auth Event */
	emit authenticated(d->error != NoError);
	if (d->error == CaptchaRequired)
		emit captchaRequired(captchaToken, captchaUrl);
}

