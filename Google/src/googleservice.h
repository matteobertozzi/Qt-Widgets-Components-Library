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

#ifndef _THGOOGLESERVICE_H_
#define _THGOOGLESERVICE_H_

#include <QObject>
class QNetworkRequest;
class QNetworkReply;
class QIODevice;
class QUrl;

class THGoogleServicePrivate;

class THGoogleService : public QObject {
    Q_OBJECT

    public:
        THGoogleService (QObject *parent = 0);
        ~THGoogleService();

        bool hasApiKey (void) const;
        QString apiKey (void) const;
        void setApiKey (const QString& apiKey);

        bool hasHostLanguage (void) const;
        QString hostLanguage (void) const;
        void setHostLanguage (const QString& hl);

        QNetworkReply *get (const QUrl& url);
        QNetworkReply *get (const QNetworkRequest& request);

        QNetworkReply *post (const QUrl& url, QIODevice *data);
        QNetworkReply *post (const QUrl& url, const QByteArray& data);
        QNetworkReply *post (const QNetworkRequest& request, QIODevice *data);
        QNetworkReply *post (const QNetworkRequest& request, 
                             const QByteArray& data);

        int responseStatus (void) const;
        QString errorString (void) const;
        bool responseStatusIsError (void) const;

    Q_SIGNALS:
        void finished (bool error);

    protected Q_SLOTS:
        virtual void responseReceived (QNetworkReply *reply);

    protected:
        virtual void parseResponse (QNetworkReply *reply);
        virtual void parseResponse (const QByteArray& data);

    protected:
        void setResponseStatus (int responseStatus);
        void setErrorString (const QString& errorString);

    private:
        THGoogleServicePrivate *d;
};

#endif /* !_THGOOGLESERVICE_H_ */

