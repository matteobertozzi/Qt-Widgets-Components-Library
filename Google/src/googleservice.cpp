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
QNetworkReply *THGoogleService::get (const QUrl& url) {
    d->resetResponseInfo();
    return(d->networkManager.get(QNetworkRequest(url)));
}

QNetworkReply *THGoogleService::get (const QNetworkRequest& request) {
    d->resetResponseInfo();
    return(d->networkManager.get(request));
}

QNetworkReply *THGoogleService::post (const QUrl& url, QIODevice *data) {
    d->resetResponseInfo();
    return(d->networkManager.post(QNetworkRequest(url), data));
}

QNetworkReply *THGoogleService::post (const QUrl& url, 
                                      const QByteArray& data)
{
    d->resetResponseInfo();
    return(d->networkManager.post(QNetworkRequest(url), data));
}

QNetworkReply *THGoogleService::post (const QNetworkRequest& request, 
                                      QIODevice *data)
{
    d->resetResponseInfo();
    return(d->networkManager.post(request, data));
}


QNetworkReply *THGoogleService::post (const QNetworkRequest& request, 
                                      const QByteArray& data)
{
    d->resetResponseInfo();
    return(d->networkManager.post(request, data));
}

// ===========================================================================
//  PUBLIC Properties
// ===========================================================================
int THGoogleService::responseStatus (void) const {
    return(d->responseStatus);
}

bool THGoogleService::responseStatusIsError (void) const {
    return(d->responseStatus != 200 && d->responseStatus != 201);
}

QString THGoogleService::errorString (void) const {
    return(d->errorString);
}

bool THGoogleService::hasApiKey (void) const {
    return(!d->apiKey.isEmpty());
}

QString THGoogleService::apiKey (void) const {
    return(d->apiKey);
}

void THGoogleService::setApiKey (const QString& apiKey) {
    d->apiKey = apiKey;
}


bool THGoogleService::hasHostLanguage (void) const {
    return(!d->hostLanguage.isEmpty());
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
#include <QDebug>
void THGoogleService::responseReceived (QNetworkReply *reply) {
    d->responseStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error()) {
        d->errorString = reply->errorString();
        qDebug() << reply->readAll();
        emit finished(true);
    } else {
        parseResponse(reply);
    }
}

// ===========================================================================
//  PROTECTED Methods
// ===========================================================================
void THGoogleService::parseResponse (QNetworkReply *reply) {
    parseResponse(reply->readAll());
}

void THGoogleService::parseResponse (const QByteArray& data) {
    Q_UNUSED(data)
    qWarning("Parse Response of this Service is Not Implemented");
    qWarning(data.constData());
    emit finished(false);
}


