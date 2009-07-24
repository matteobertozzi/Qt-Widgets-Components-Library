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

#ifndef _GOOGLEENTRY_H_
#define _GOOGLEENTRY_H_

#include <QDateTime>
#include <QObject>
class QXmlStreamReader;
class QXmlStreamWriter;

class THGoogleContentPrivate;
class THGoogleContent : public QObject {
    Q_OBJECT

    public:
        THGoogleContent (QObject *parent = 0);
        virtual ~THGoogleContent();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        bool isEmpty (void) const;

        QString lang (void) const;
        void setLang (const QString& lang);

        QString type (void) const;
        void setType (const QString& type);

        QString content (void) const;
        void setContent (const QString& content, const QString& type);

        void setHtml (const QString& html);
        void setPlainText (const QString& text);

    private:
        THGoogleContentPrivate *d;
};

class THGoogleLinkPrivate;
class THGoogleLink : public QObject {
    Q_OBJECT

    public:
        THGoogleLink (QObject *parent = 0);
        ~THGoogleLink();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        bool hasRel (void) const;
        QString rel (void) const;
        void setRel (const QString& rel);

        bool hasType (void) const;
        QString type (void) const;
        void setType (const QString& type);

        bool hasHref (void) const;
        QString href (void) const;
        void setHref (const QString& href);

        bool hasHrefLang (void) const;
        QString hrefLang (void) const;
        void setHrefLang (const QString& hrefLang);

        quint32 length (void) const;
        void setLength (quint32 length);

        bool hasEtag (void) const;
        QString etag (void) const;
        void setEtag (const QString& etag);

        QString title (void) const;
        void setTitle (const QString& title);

        QString titleLang (void) const;
        void setTitleLang (const QString& titleLang);    

    private:
        THGoogleLinkPrivate *d;
};

class THGoogleRelValueInfoPrivate;
class THGoogleRelValueInfo : public QObject {
    Q_OBJECT

    public:
        THGoogleRelValueInfo (QObject *parent = 0);
        ~THGoogleRelValueInfo();

        // Methods
        virtual void readXml (QXmlStreamReader *xmlReader);
        virtual void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        bool hasRel (void) const;
        QString rel (void) const;
        void setRel (const QString& rel);

        QString value (void) const;
        void setValue (const QString& value);

        bool hasLabel (void) const;
        QString label (void) const;
        void setLabel (const QString& label);

    private:
        THGoogleRelValueInfoPrivate *d;
};

class THGoogleCategoryPrivate;
class THGoogleCategory : public QObject {
    Q_OBJECT

    public:
        THGoogleCategory (QObject *parent = 0);
        ~THGoogleCategory();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        bool hasLabel (void) const;
        QString label (void) const;
        void setLabel (const QString& label);

        QString term (void) const;
        void setTerm (const QString& term);

        QString scheme (void) const;
        void setScheme (const QString& scheme);

    private:
        THGoogleCategoryPrivate *d;
};

class THGooglePersonPrivate;
class THGooglePerson : public QObject {
    Q_OBJECT

    public:
        THGooglePerson (QObject *parent = 0);
        ~THGooglePerson();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        QString uri (void) const;
        void setUri (const QString& uri);

        QString name (void) const;
        void setName (const QString& name);

        QString email (void) const;
        void setEmail (const QString& email);

    private:
        THGooglePersonPrivate *d;
};

class THGoogleExtendedPropertyPrivate;
class THGoogleExtendedProperty : public QObject {
    Q_OBJECT

    public:
        THGoogleExtendedProperty (QObject *parent = 0);
        ~THGoogleExtendedProperty();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        QString name (void) const;
        void setName (const QString& name);

        bool hasRealm (void) const;
        QString realm (void) const;
        void setRealm (const QString& realm);

        bool hasValue (void) const;
        QString value (void) const;
        void setValue (const QString& value);

    private:
        THGoogleExtendedPropertyPrivate *d;
};

class THGoogleEntryAdapter {
    public:
        virtual bool readXmlElement (QXmlStreamReader *xmlReader) = 0;
        virtual void writeXmlElement (QXmlStreamWriter *xmlWriter) const = 0;
};

class THGoogleEntryPrivate;
class THGoogleEntry : public QObject {
    Q_OBJECT

    public:
        THGoogleEntry (QObject *parent = 0);
        ~THGoogleEntry();

        // Methods      
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter) const;

        virtual void clear (void);

        // Properties
        void addAdapter (THGoogleEntryAdapter *adaptor);
        void addExtendedProperty (THGoogleExtendedProperty *property);

        void addLink (THGoogleLink *link);
        void addLink (const QString& rel,
                      const QString& type,
                      const QString& href);

        QString id (void) const;
        void setId (const QString& id);

        QString etag (void) const;
        void setEtag (const QString& etag);

        QString title (void) const;
        void setTitle (const QString& title);

        bool isDeleted (void) const;
        void setDeleted (bool deleted);

        QDateTime updated (void) const;
        QDateTime published (void) const;

        THGooglePerson *author (void) const;
        THGoogleContent *summary (void) const;
        THGoogleContent *content (void) const;
        THGoogleCategory *category (void) const;

        THGoogleLink *link (const QString& rel, const QString& type) const;

        QList<THGoogleLink *> links (void) const;
        QList<THGoogleLink *> links (const QString& rel, const QString& type) const;

        bool hasExtendedProperties (void) const;
        QList<THGoogleExtendedProperty *> extendedProperties (void) const;

        QByteArray toXml (void) const;

    private:
        THGoogleEntryPrivate *d;
};

#include "googleservice.h"
class THGoogleEntryServicePrivate;

class THGooglEntryService : public THGoogleService {
    Q_OBJECT

    public:
        THGooglEntryService (QObject *parent = 0);
        ~THGooglEntryService();

        QString authToken (void) const;
        void setAuthToken (const QString& authToken);

        QString gdataVersion (void) const;
        void setGDataVersion (const QString& gDataVersion);

    protected:
        QNetworkReply *queryEntry (const QUrl& url);
        QNetworkReply *insertEntry (const QUrl& url, const QByteArray& xml);
        QNetworkReply *updateEntry (const QUrl& url,
                                    const QByteArray& etag, 
                                    const QByteArray& xml);
        QNetworkReply *deleteEntry (const QUrl& url, 
                                    const QByteArray& etag,
                                    const QByteArray& xml);

        QNetworkReply *updateEntry (const QNetworkRequest& req,
                                    const QByteArray& data);

    private:
        THGoogleEntryServicePrivate *d;
};

#endif /* !_GOOGLEENTRY_H_ */

