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

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "googleservice.h"

// ===========================================================================
//  PRIVATE Class
// ===========================================================================
class THGoogleServicePrivate {
    public:
        QNetworkAccessManager networkManager;
        QString errorString;
        int responseStatus;

        QString hostLanguage;
        QString apiKey;

    public:
        void resetResponseInfo (void);
};

void THGoogleServicePrivate::resetResponseInfo (void) {
    errorString.clear();
    responseStatus = 0;
}

// ===========================================================================
//  PUBLIC Constructors/Destructors
// ===========================================================================
THGoogleService::THGoogleService (QObject *parent)
    : QObject(parent), d(new THGoogleServicePrivate)
{
    connect(&(d->networkManager), SIGNAL(finished(QNetworkReply *)),
            this, SLOT(responseReceived(QNetworkReply *)));
}

THGoogleService::~THGoogleService() {
    delete d;
}

// ===========================================================================
//  PUBLIC Methods
// ===========================================================================
void THGoogleService::get (const QUrl& url) {
    d->resetResponseInfo();
    d->networkManager.get(QNetworkRequest(url));
}

void THGoogleService::get (const QNetworkRequest& request) {
    d->resetResponseInfo();
    d->networkManager.get(request);
}

void THGoogleService::post (const QUrl& url, QIODevice *data) {
    d->resetResponseInfo();
    d->networkManager.post(QNetworkRequest(url), data);
}

void THGoogleService::post (const QUrl& url, const QByteArray& data) {
    d->resetResponseInfo();
    d->networkManager.post(QNetworkRequest(url), data);
}

void THGoogleService::post (const QNetworkRequest& request, QIODevice *data) {
    d->resetResponseInfo();
    d->networkManager.post(request, data);
}


void THGoogleService::post (const QNetworkRequest& request, 
                            const QByteArray& data)
{
    d->resetResponseInfo();
    d->networkManager.post(request, data);
}

// ===========================================================================
//  PUBLIC Properties
// ===========================================================================
int THGoogleService::responseStatus (void) const {
    return(d->responseStatus);
}

QString THGoogleService::errorString (void) const {
    return(d->errorString);
}

QString THGoogleService::apiKey (void) const {
    return(d->apiKey);
}

void THGoogleService::setApiKey (const QString& apiKey) {
    d->apiKey = apiKey;
}


QString THGoogleService::hostLanguage (void) const {
    return(d->hostLanguage);
}

void THGoogleService::setHostLanguage (const QString& hl) {
    d->hostLanguage = hl;
}

// ===========================================================================
//  PROTECTED Properties
// ===========================================================================
void THGoogleService::setErrorString (const QString& errorString) {
    d->errorString = errorString;
}

void THGoogleService::setResponseStatus (int responseStatus) {
    d->responseStatus = responseStatus;
}

// ===========================================================================
//  PROTECTED Slots
// ===========================================================================
void THGoogleService::responseReceived (QNetworkReply *reply) {
    d->responseStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error()) {
        d->errorString = reply->errorString();
        emit finished(false);
    } else {
        parseResponse(reply->readAll());
    }
}

// ===========================================================================
//  PROTECTED Methods
// ===========================================================================
void THGoogleService::parseResponse (const QByteArray& data) {
    Q_UNUSED(data)
    qWarning("Parse Response of this Service is Not Implemented");
}


