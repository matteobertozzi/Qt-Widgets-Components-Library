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

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QNetworkRequest>

#include "googleentry.h"

// ===========================================================================
//  Abstract Content
// ===========================================================================
class THGoogleContentPrivate {
    public:
        QString content;
        QString type;
        QString lang;
};

THGoogleContent::THGoogleContent (QObject *parent)
    : QObject(parent), d(new THGoogleContentPrivate)
{
}

THGoogleContent::~THGoogleContent() {
    delete d;
}

void THGoogleContent::readXml (QXmlStreamReader *xmlReader) {
    if (xmlReader->attributes().hasAttribute("type"))
        d->type = xmlReader->attributes().value("type").toString().simplified();

    // TODO: Handle Language

    QString contentTag = xmlReader->name().toString();
    while (!xmlReader->atEnd()) {
        xmlReader->readNext();

        QStringRef name = xmlReader->name();
        if (xmlReader->isEndElement() && name == contentTag)
            break;

        // TODO: READ ALL DATA
    }
}

void THGoogleContent::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->type.isEmpty())
        xmlWriter->writeAttribute("type", d->type);

    // TODO: Handle Language

    xmlWriter->writeCharacters(d->content);
}

bool THGoogleContent::isEmpty (void) const {
    return(d->content.isEmpty());
}

QString THGoogleContent::lang (void) const {
    return(d->lang);
}

void THGoogleContent::setLang (const QString& lang) {
    d->lang = lang;
}

QString THGoogleContent::type (void) const {
    return(d->type);
}

void THGoogleContent::setType (const QString& type) {
    d->type = type;
}

QString THGoogleContent::content (void) const {
    return(d->content);
}

void THGoogleContent::setContent (const QString& content, const QString& type) {
    d->content = content;
    d->type = type;
}

void THGoogleContent::setHtml (const QString& html) {
    d->content = html;
    d->type = "text/html";
}

void THGoogleContent::setPlainText (const QString& text) {
    d->content = text;
    d->type = "text";
}

// ===========================================================================
//  Link
// ===========================================================================
class THGoogleLinkPrivate {
    public:
        QString titleLang;
        QString hrefLang;
        quint32 length;
        QString title;
        QString href;
        QString type;
        QString etag;
        QString rel;
};

THGoogleLink::THGoogleLink (QObject *parent)
    : QObject(parent), d(new THGoogleLinkPrivate)
{
    d->length = 0;
}

THGoogleLink::~THGoogleLink() {
    delete d;
}

void THGoogleLink::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();

    if (attributes.hasAttribute("type"))
        d->type = attributes.value("type").toString().simplified();

    if (attributes.hasAttribute("href"))
        d->href = attributes.value("href").toString().simplified();
}

void THGoogleLink::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    if (!d->type.isEmpty())
        xmlWriter->writeAttribute("type", d->type);

    if (!d->href.isEmpty())
        xmlWriter->writeAttribute("href", d->href);
}

bool THGoogleLink::hasRel (void) const {
    return(!d->rel.isEmpty());
}

QString THGoogleLink::rel (void) const {
    return(d->rel);
}

void THGoogleLink::setRel (const QString& rel) {
    d->rel = rel;
}

bool THGoogleLink::hasType (void) const {
    return(!d->type.isEmpty());
}

QString THGoogleLink::type (void) const {
    return(d->type);
}

void THGoogleLink::setType (const QString& type) {
    d->type = type;
}

bool THGoogleLink::hasHref (void) const {
    return(!d->href.isEmpty());
}

QString THGoogleLink::href (void) const {
    return(d->href);
}

void THGoogleLink::setHref (const QString& href) {
    d->href = href;
}

bool THGoogleLink::hasHrefLang (void) const {
    return(!d->hrefLang.isEmpty());
}

QString THGoogleLink::hrefLang (void) const {
    return(d->hrefLang);
}

void THGoogleLink::setHrefLang (const QString& hrefLang) {
    d->hrefLang = hrefLang;
}

quint32 THGoogleLink::length (void) const {
    return(d->length);
}

void THGoogleLink::setLength (quint32 length) {
    d->length = length;
}

bool THGoogleLink::hasEtag (void) const {
    return(!d->etag.isEmpty());
}

QString THGoogleLink::etag (void) const {
    return(d->etag);
}

void THGoogleLink::setEtag (const QString& etag) {
    d->etag = etag;
}

QString THGoogleLink::title (void) const {
    return(d->title);
}

void THGoogleLink::setTitle (const QString& title) {
    d->title = title;
}

QString THGoogleLink::titleLang (void) const {
    return(d->titleLang);
}

void THGoogleLink::setTitleLang (const QString& titleLang) {
    d->titleLang = titleLang;
}

// ===========================================================================
//  RelValue Info
// ===========================================================================
class THGoogleRelValueInfoPrivate {
    public:
        QString label;
        QString value;
        QString rel;
};

THGoogleRelValueInfo::THGoogleRelValueInfo (QObject *parent)
    : QObject(parent), d(new THGoogleRelValueInfoPrivate)
{
}

THGoogleRelValueInfo::~THGoogleRelValueInfo() {
    delete d;
}

void THGoogleRelValueInfo::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("label"))
        d->label = attributes.value("label").toString().simplified();

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();

    if (attributes.hasAttribute("value"))
        d->value = attributes.value("value").toString().simplified();
}

void THGoogleRelValueInfo::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->label.isEmpty())
        xmlWriter->writeAttribute("label", d->label);

    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    xmlWriter->writeAttribute("value", d->value);
}

bool THGoogleRelValueInfo::hasRel (void) const {
    return(!d->rel.isEmpty());
}

QString THGoogleRelValueInfo::rel (void) const {
    return(d->rel);
}

void THGoogleRelValueInfo::setRel (const QString& rel) {
    d->rel = rel;
}

QString THGoogleRelValueInfo::value (void) const {
    return(d->value);
}

void THGoogleRelValueInfo::setValue (const QString& value) {
    d->value = value;
}

bool THGoogleRelValueInfo::hasLabel (void) const {
    return(!d->label.isEmpty());
}

QString THGoogleRelValueInfo::label (void) const {
    return(d->label);
}

void THGoogleRelValueInfo::setLabel (const QString& label) {
    d->label = label;
}

// ===========================================================================
//  Category
// ===========================================================================
class THGoogleCategoryPrivate {
    public:
        QString scheme;
        QString label;
        QString term;
};

THGoogleCategory::THGoogleCategory (QObject *parent)
    : QObject(parent), d(new THGoogleCategoryPrivate)
{
}

THGoogleCategory::~THGoogleCategory() {
    delete d;
}

void THGoogleCategory::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("term"))
        d->term = attributes.value("term").toString().simplified();

    if (attributes.hasAttribute("scheme"))
        d->scheme = attributes.value("scheme").toString().simplified();
}

void THGoogleCategory::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->term.isEmpty())
        xmlWriter->writeAttribute("term", d->term);

    if (!d->scheme.isEmpty())
        xmlWriter->writeAttribute("scheme", d->scheme);
}

bool THGoogleCategory::hasLabel (void) const {
    return(!d->label.isEmpty());
}

QString THGoogleCategory::label (void) const {
    return(d->label);
}

void THGoogleCategory::setLabel (const QString& label) {
    d->label = label;
}

QString THGoogleCategory::term (void) const {
    return(d->term);
}

void THGoogleCategory::setTerm (const QString& term) {
    d->term = term;
}

QString THGoogleCategory::scheme (void) const {
    return(d->scheme);
}

void THGoogleCategory::setScheme (const QString& scheme) {
    d->scheme = scheme;
}

// ===========================================================================
//  Person
// ===========================================================================
class THGooglePersonPrivate {
    public:
        QString email;
        QString name;
        QString uri;
};

THGooglePerson::THGooglePerson (QObject *parent)
    : QObject(parent), d(new THGooglePersonPrivate)
{
}

THGooglePerson::~THGooglePerson() {
    delete d;
}

void THGooglePerson::readXml (QXmlStreamReader *xmlReader) {
    // TODO: Handle URI
    QString personTag = xmlReader->name().toString();
    while (!xmlReader->atEnd()) {
        xmlReader->readNext();

        if (xmlReader->isEndElement() && xmlReader->name() == personTag)
            break;

        if (!xmlReader->isStartElement())
            break;

        QStringRef name = xmlReader->name();
        if (name == "name")
            d->name = xmlReader->readElementText().simplified();
        else if (name == "email")
            d->email = xmlReader->readElementText().simplified();
    }
}

void THGooglePerson::writeXml (QXmlStreamWriter *xmlWriter) {
    // TODO: Handle URI
    xmlWriter->writeTextElement("name", d->name);
    xmlWriter->writeTextElement("email", d->email);
}

QString THGooglePerson::uri (void) const {
    return(d->uri);
}

void THGooglePerson::setUri (const QString& uri) {
    d->uri = uri;
}

QString THGooglePerson::name (void) const {
    return(d->name);
}

void THGooglePerson::setName (const QString& name) {
    d->name = name;
}

QString THGooglePerson::email (void) const {
    return(d->email);
}

void THGooglePerson::setEmail (const QString& email) {
    d->email = email;
}


// ===========================================================================
//  Extended Property
// ===========================================================================
class THGoogleExtendedPropertyPrivate {
    public:
        QString value;
        QString realm;
        QString name;
};

THGoogleExtendedProperty::THGoogleExtendedProperty (QObject *parent)
    : QObject(parent), d(new THGoogleExtendedPropertyPrivate)
{
}

THGoogleExtendedProperty::~THGoogleExtendedProperty() {
    delete d;
}

void THGoogleExtendedProperty::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();
    if (attributes.hasAttribute("name"))
        d->name = attributes.value("name").toString().simplified();

    if (attributes.hasAttribute("value"))
        d->value = attributes.value("value").toString().simplified();
    else
        d->value = xmlReader->readElementText().simplified();
}

void THGoogleExtendedProperty::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->name.isEmpty())
        xmlWriter->writeAttribute("name", d->name);

    if (!d->value.isEmpty())
        xmlWriter->writeAttribute("value", d->value);
}

QString THGoogleExtendedProperty::name (void) const {
    return(d->name);
}

void THGoogleExtendedProperty::setName (const QString& name) {
    d->name = name;
}


bool THGoogleExtendedProperty::hasRealm (void) const {
    return(!d->realm.isEmpty());
}

QString THGoogleExtendedProperty::realm (void) const {
    return(d->realm);
}

void THGoogleExtendedProperty::setRealm (const QString& realm) {
    d->realm = realm;
}


bool THGoogleExtendedProperty::hasValue (void) const {
    return(!d->value.isEmpty());
}

QString THGoogleExtendedProperty::value (void) const {
    return(d->value);
}

void THGoogleExtendedProperty::setValue (const QString& value) {
    d->value = value;
}

// ===========================================================================
//  Entry
// ===========================================================================
class THGoogleEntryPrivate {
    public:
        QList<THGoogleEntryAdapter *> adapters;
        
        QList<THGoogleExtendedProperty *> extendedProperties;
        QList<THGoogleLink *> links;
        THGoogleCategory category;
        THGoogleContent content;
        THGoogleContent summary;
        THGooglePerson author;
        QDateTime published;
        QDateTime updated;
        bool isDeleted;
        QString title;
        QString etag;
        QString id;
};

THGoogleEntry::THGoogleEntry (QObject *parent)
    : QObject(parent), d(new THGoogleEntryPrivate)
{
    d->isDeleted = false;
}

THGoogleEntry::~THGoogleEntry() {
    delete d;
}

void THGoogleEntry::clear (void) {
    d->extendedProperties.clear();
    d->isDeleted = false;
    d->links.clear();
    d->title.clear();
    d->etag.clear();
    d->id.clear();
}

void THGoogleEntry::readXml (QXmlStreamReader *xmlReader) {
    if (xmlReader->attributes().hasAttribute("gd:etag"))
        d->etag = xmlReader->attributes().value("gd:etag").toString().simplified();

    while (!xmlReader->atEnd()) {
        xmlReader->readNext();

        QStringRef name = xmlReader->name();
        if (xmlReader->isEndElement() && name == "entry")
            break;

        if (!xmlReader->isStartElement())
            continue;

        QStringRef qualifiedName = xmlReader->qualifiedName();
        if (name == "link") {
            THGoogleLink *link = new THGoogleLink;
            link->readXml(xmlReader);
            d->links.append(link);
        } else if (name == "id") {
            d->id = xmlReader->readElementText().simplified();
        } else if (name == "title") {
            d->title = xmlReader->readElementText().simplified();
        } else if (name == "summary") {
            d->summary.readXml(xmlReader);
        } else if (name == "content") {
            d->content.readXml(xmlReader);
        } else if (name == "author") {
            d->author.readXml(xmlReader);
        } else if (name == "published") {
            QString dateString = xmlReader->readElementText().simplified();
            d->published = QDateTime::fromString(dateString, Qt::ISODate);
        } else if (name == "updated") {
            QString dateString = xmlReader->readElementText().simplified();
            d->updated = QDateTime::fromString(dateString, Qt::ISODate);
        } else if (name == "category") {
            d->category.readXml(xmlReader);
        } else if (qualifiedName == "gd:extendedProperty") {
            THGoogleExtendedProperty *extProperty = new THGoogleExtendedProperty(this);
            extProperty->readXml(xmlReader);
            d->extendedProperties.append(extProperty);
        } else if (qualifiedName == "gd:deleted") {
            d->isDeleted = true;
        } else {
            foreach (THGoogleEntryAdapter *adapter, d->adapters) {
                if (adapter->readXmlElement(xmlReader))
                    break;
            }
        }
    }
}

void THGoogleEntry::writeXml (QXmlStreamWriter *xmlWriter) const {
    // Entry
    xmlWriter->writeStartElement("entry");
    xmlWriter->writeAttribute("xmlns", "http://www.w3.org/2005/Atom");
    xmlWriter->writeAttribute("xmlns:gd", "http://schemas.google.com/g/2005");
    xmlWriter->writeAttribute("xmlns:openSearch", "http://a9.com/-/spec/opensearch/1.1/");
    xmlWriter->writeAttribute("xmlns:gContact", "http://schemas.google.com/contact/2008");
    xmlWriter->writeAttribute("xmlns:batch", "http://schemas.google.com/gdata/batch");

    if (!d->etag.isEmpty())
        xmlWriter->writeAttribute("gd:etag", d->etag);

    // Id
    if (!d->id.isEmpty())
        xmlWriter->writeTextElement("id", d->id);

    if (!d->updated.isNull())
        xmlWriter->writeTextElement("updated", d->updated.toString(Qt::ISODate));

    // Category
    xmlWriter->writeEmptyElement("category");
    d->category.writeXml(xmlWriter);

    // Title
    if (!d->title.isEmpty())
        xmlWriter->writeTextElement("title", d->title);

    // Content
    if (!d->content.isEmpty()) {
        xmlWriter->writeStartElement("content");
        d->content.writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    // Summary
    if (!d->summary.isEmpty()) {
        xmlWriter->writeStartElement("summary");
        d->summary.writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    // Extended Properties
    foreach (THGoogleExtendedProperty *extProperty, d->extendedProperties) {
        xmlWriter->writeStartElement("gd:extendedProperty");
        extProperty->writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    foreach (THGoogleEntryAdapter *adapter, d->adapters)
        adapter->writeXmlElement(xmlWriter);

    if (d->isDeleted)
        xmlWriter->writeEmptyElement("gd:deleted");

    xmlWriter->writeEndElement();
}


void THGoogleEntry::addAdapter (THGoogleEntryAdapter *adapter) {
    d->adapters.append(adapter);
}

void THGoogleEntry::addLink (THGoogleLink *link) {
    d->links.append(link);
}

void THGoogleEntry::addLink (const QString& rel,
                             const QString& type,
                             const QString& href)
{
    THGoogleLink *link = new THGoogleLink(this);
    link->setRel(rel);
    link->setType(type);
    link->setHref(href);
    d->links.append(link);
}

void THGoogleEntry::addExtendedProperty (THGoogleExtendedProperty *property) {
    property->setParent(this);
    d->extendedProperties.append(property);
}

QString THGoogleEntry::id (void) const {
    return(d->id);
}

void THGoogleEntry::setId (const QString& id) {
    d->id = id;
}

QString THGoogleEntry::etag (void) const {
    return(d->etag);
}

void THGoogleEntry::setEtag (const QString& etag) {
    d->etag = etag;
}

QString THGoogleEntry::title (void) const {
    return(d->title);
}

void THGoogleEntry::setTitle (const QString& title) {
    d->title = title;
}


bool THGoogleEntry::isDeleted (void) const {
    return(d->isDeleted);
}

void THGoogleEntry::setDeleted (bool deleted) {
    d->isDeleted = deleted;
}

QDateTime THGoogleEntry::updated (void) const {
    return(d->updated);
}

QDateTime THGoogleEntry::published (void) const {
    return(d->published);
}

THGooglePerson *THGoogleEntry::author (void) const {
    return(&(d->author));
}

THGoogleContent *THGoogleEntry::summary (void) const {
    return(&(d->summary));
}

THGoogleContent *THGoogleEntry::content (void) const {
    return(&(d->content));
}

THGoogleCategory *THGoogleEntry::category (void) const {
    return(&(d->category));
}

THGoogleLink *THGoogleEntry::link (const QString& rel, 
                                   const QString& type) const
{
    QList<THGoogleLink *> results;
    foreach (THGoogleLink *link, d->links) {
        if (link->rel() == rel && link->type() == type)
            return(link);
    }

    return(NULL);
}

QList<THGoogleLink *> THGoogleEntry::links (void) const {
    return(d->links);
}

QList<THGoogleLink *> THGoogleEntry::links (const QString& rel, 
                                            const QString& type) const
{
    QList<THGoogleLink *> results;
    foreach (THGoogleLink *link, d->links) {
        if (link->rel() == rel && link->type() == type)
            results.append(link);
    }
    return(results);
}

bool THGoogleEntry::hasExtendedProperties (void) const {
    return(!d->extendedProperties.isEmpty());
}

QList<THGoogleExtendedProperty *> THGoogleEntry::extendedProperties (void) const {
    return(d->extendedProperties);
}

QByteArray THGoogleEntry::toXml (void) const {
    QByteArray xml;
    QXmlStreamWriter xmlWriter(&xml);
    xmlWriter.setAutoFormatting(true);
    writeXml(&xmlWriter);
    return(xml.trimmed());
}

// ===========================================================================
//  Entry Service
// ===========================================================================
class THGoogleEntryServicePrivate {
    public:
        QString gdataVersion;
        QString authToken;

    public:
        QByteArray authHeaderValue (void) const;
        QByteArray dataVersionHeaderValue (void) const;
};

QByteArray THGoogleEntryServicePrivate::authHeaderValue (void) const {
    return(QString("GoogleLogin auth=%1").arg(authToken).toLatin1());
}

QByteArray THGoogleEntryServicePrivate::dataVersionHeaderValue (void) const {
    return(gdataVersion.toLatin1());
}

// ===========================================================================
//  PUBLIC Constructors/Destructors
// ===========================================================================
THGooglEntryService::THGooglEntryService (QObject *parent)
    : THGoogleService(parent), d(new THGoogleEntryServicePrivate)
{
    d->gdataVersion = "2";
}

THGooglEntryService::~THGooglEntryService() {
    delete d;
}

// ===========================================================================
//  PUBLIC Properties
// ===========================================================================
QString THGooglEntryService::authToken (void) const {
    return(d->authToken);
}

void THGooglEntryService::setAuthToken (const QString& authToken) {
    d->authToken = authToken;
}

QString THGooglEntryService::gdataVersion (void) const {
    return(d->gdataVersion);
}

void THGooglEntryService::setGDataVersion (const QString& gdataVersion) {
    d->gdataVersion = gdataVersion;
}

// ===========================================================================
//  PROTECTED Methods
// ===========================================================================
QNetworkReply *THGooglEntryService::queryEntry (const QUrl& url) {
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", d->authHeaderValue());
    request.setRawHeader("GData-Version", d->dataVersionHeaderValue());
    request.setHeader(QNetworkRequest::ContentTypeHeader, 
                      "application/x-www-form-urlencoded");

    return(get(request));
}

QNetworkReply *THGooglEntryService::insertEntry (const QUrl& url,
                                                 const QByteArray& xml)
{
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", d->authHeaderValue());
    request.setRawHeader("GData-Version", d->dataVersionHeaderValue());
    request.setHeader(QNetworkRequest::ContentTypeHeader, 
                      "application/atom+xml");

    return(post(request, xml));
}

QNetworkReply *THGooglEntryService::updateEntry (const QUrl& url, 
                                                 const QByteArray& etag,
                                                 const QByteArray& xml)
{
    QNetworkRequest request(url);
    request.setRawHeader("X-HTTP-Method-Override", "PUT");
    request.setRawHeader("Authorization", d->authHeaderValue());
    request.setRawHeader("GData-Version", d->dataVersionHeaderValue());
    request.setRawHeader("If-Match", etag);
    request.setHeader(QNetworkRequest::ContentTypeHeader, 
                      "application/atom+xml");

    return(post(request, xml));
}

QNetworkReply *THGooglEntryService::updateEntry (const QNetworkRequest& req,
                                                 const QByteArray& data)
{
    QNetworkRequest request(req);
    request.setRawHeader("X-HTTP-Method-Override", "PUT");
    request.setRawHeader("Authorization", d->authHeaderValue());
    request.setRawHeader("GData-Version", d->dataVersionHeaderValue());

    return(post(request, data));
}

QNetworkReply *THGooglEntryService::deleteEntry (const QUrl& url, 
                                                 const QByteArray& etag,
                                                 const QByteArray& xml)
{
    QNetworkRequest request(url);
    request.setRawHeader("X-HTTP-Method-Override", "DELETE");
    request.setRawHeader("Authorization", d->authHeaderValue());
    request.setRawHeader("GData-Version", d->dataVersionHeaderValue());
    request.setRawHeader("If-Match", etag);
    request.setHeader(QNetworkRequest::ContentTypeHeader, 
                      "application/atom+xml");

    return(post(request, xml));
}

