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

#ifndef _THGOOGLEAUTH_H_
#define _THGOOGLEAUTH_H_

#include <QObject>

class THGoogleAuthPrivate;

class THGoogleAuth : public QObject {
	Q_OBJECT

	Q_ENUMS(Service Error)

	Q_PROPERTY(QString sid READ sid)
	Q_PROPERTY(QString lsid READ lsid)
	Q_PROPERTY(QString auth READ auth)

	Q_PROPERTY(Error error READ error)
	Q_PROPERTY(QString errorString READ errorString)

	Q_PROPERTY(QString mail READ mail WRITE setMail)
	Q_PROPERTY(Service service READ service WRITE setService)

	public:
		enum Service {
			GoogleAppsProvisioning,
			GoogleBase,
			Blogger,
			BookSearch,
			Calendar,
			GoogleCodeSearch,
			Contacts,
			DocumentsList,
			Finance,
			GmailAtomFeed,
			Health,
			Notebook,
			PicasaWebAlbums,
			Spreadsheets,
			WebmasterTools,
			YouTube
		};

		enum Error {
			NoError,
			HttpError,
			BadAuthentication,
			NotVerified,
			TermsNotAgreed,
			CaptchaRequired,
			Unknown,
			AccountDeleted,
			AccountDisabled,
			ServiceDisabled,
			ServiceUnavailable			
		};

	public:
		THGoogleAuth (QObject *parent = 0);
		THGoogleAuth (const QString& mail, QObject *parent = 0);
		~THGoogleAuth();		

		// User Information
		const QString& mail (void) const;
		void setMail (const QString& mail);

		Service service (void) const;
		void setService (Service service);

		// Auth Properties
		Error error (void) const;
		QString errorString (void) const;

		const QString& sid (void) const;
		const QString& lsid (void) const;
		const QString& auth (void) const;

	signals:
		void authenticated (bool error = false);
		void captchaRequired (	const QString& captchaToken,
								const QString& captchaUrl);

	public Q_SLOTS:
		void login (const QString& password,
					const QString& loginToken = QString(),
					const QString& loginCaptcha = QString());
		void login (Service service, 
					const QString& password,
					const QString& loginToken = QString(),
					const QString& loginCaptcha = QString());

	protected Q_SLOTS:
		void authDone (bool error);

	private:
		THGoogleAuthPrivate *d;
};

#endif /* !_THGOOGLEAUTH_H_ */

