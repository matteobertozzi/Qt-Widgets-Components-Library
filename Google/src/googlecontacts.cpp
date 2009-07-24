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
#include <QNetworkReply>
#include <QEventLoop>
#include <QBuffer>
#include <QPixmap>
#include <QImage>
#include <QTimer>
#include <QHash>
#include <QUrl>

#include "googlecontacts.h"

// ===========================================================================
//  EMail
// ===========================================================================
class THGoogleEmailPrivate {
    public:
        QString displayName;
        QString address;
        QString label;
        QString quota;
        bool primary;
        QString rel;
};

THGoogleEmail::THGoogleEmail (QObject *parent)
    : QObject(parent), d(new THGoogleEmailPrivate)
{
    d->primary = false;
}

THGoogleEmail::~THGoogleEmail() {
    delete d;
}


QString THGoogleEmail::relHome (void) {
    return("http://schemas.google.com/g/2005#home");
}

QString THGoogleEmail::relWork (void) {
    return("http://schemas.google.com/g/2005#work");
}

QString THGoogleEmail::relOther (void) {
    return("http://schemas.google.com/g/2005#other");
}

void THGoogleEmail::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();

    if (attributes.hasAttribute("address"))
        d->address = attributes.value("address").toString().simplified();

    if (attributes.hasAttribute("label"))
        d->address = attributes.value("label").toString().simplified();

    if (attributes.hasAttribute("primary"))
        d->primary = attributes.value("primary").compare("true", Qt::CaseInsensitive);
}

void THGoogleEmail::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    if (!d->address.isEmpty())
        xmlWriter->writeAttribute("address", d->address);

    if (!d->label.isEmpty())
        xmlWriter->writeAttribute("label", d->label);

    xmlWriter->writeAttribute("primary", d->primary ? "true" : "false");
}

QString THGoogleEmail::label (void) const {
    return(d->label);
}

void THGoogleEmail::setLabel (const QString& label) {
    d->label = label;
}

QString THGoogleEmail::address (void) const {
    return(d->address);
}

void THGoogleEmail::setAddress (const QString& address) {
    d->address = address;
}

bool THGoogleEmail::isWorkAddress (void) const {
    return(d->rel == relWork());
}

bool THGoogleEmail::isHomeAddress (void) const {
    return(d->rel == relHome());
}

bool THGoogleEmail::isOtherAddress (void) const {
    return(d->rel == relOther());
}

QString THGoogleEmail::displayName (void) const {
    return(d->displayName);
}

void THGoogleEmail::setDisplayName (const QString& displayName) {
    d->displayName = displayName;
}

bool THGoogleEmail::primary (void) const {
    return(d->primary);
}

void THGoogleEmail::setPrimary (bool primary) {
    d->primary = primary;
}

QString THGoogleEmail::rel (void) const {
    return(d->rel);
}

void THGoogleEmail::setRel (const QString& rel) {
    d->rel = rel;
}

QString THGoogleEmail::quota (void) const {
    return(d->quota);
}

void THGoogleEmail::setQuota (const QString& quota) {
    d->quota = quota;
}

// ===========================================================================
//  Group Membership Info
// ===========================================================================
class THGoogleGroupMembershipInfoPrivate {
    public:
        QString href;
        bool deleted;
};

THGoogleGroupMembershipInfo::THGoogleGroupMembershipInfo (QObject *parent)
    : QObject(parent), d(new THGoogleGroupMembershipInfoPrivate)
{
}

THGoogleGroupMembershipInfo::~THGoogleGroupMembershipInfo() {
    delete d;
}

void THGoogleGroupMembershipInfo::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("href"))
        d->href = attributes.value("href").toString().simplified();

    if (attributes.hasAttribute("deleted"))
        d->deleted = attributes.value("deleted").compare("true", Qt::CaseInsensitive);
}

void THGoogleGroupMembershipInfo::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->href.isEmpty())
        xmlWriter->writeAttribute("href", d->href);

    xmlWriter->writeAttribute("deleted", d->deleted ? "true" : "false");
}

bool THGoogleGroupMembershipInfo::deleted (void) const {
    return(d->deleted);
}

void THGoogleGroupMembershipInfo::setDeleted (bool deleted) {
    d->deleted = deleted;
}

bool THGoogleGroupMembershipInfo::hasHref (void) const {
    return(!d->href.isEmpty());
}

QString THGoogleGroupMembershipInfo::href (void) const {
    return(d->href);
}

void THGoogleGroupMembershipInfo::setHref (const QString& href) {
    d->href = href;
}

// ===========================================================================
//  Instant Messaging
// ===========================================================================
class THGoogleImPrivate {
    public:
        QString protocol;
        QString address;
        QString label;
        bool primary;
        QString rel;
};

THGoogleIm::THGoogleIm (QObject *parent)
    : QObject(parent), d(new THGoogleImPrivate)
{
}

THGoogleIm::~THGoogleIm() {
    delete d;
}


QString THGoogleIm::relHome (void) {
    return("http://schemas.google.com/g/2005#home");
}

QString THGoogleIm::relWork (void) {
    return("http://schemas.google.com/g/2005#work");
}

QString THGoogleIm::relOther (void) {
    return("http://schemas.google.com/g/2005#other");
}

QString THGoogleIm::protocolQq (void) {
    return("http://schemas.google.com/g/2005#QQ");
}

QString THGoogleIm::protocolAim (void) {
    return("http://schemas.google.com/g/2005#AIM");
}

QString THGoogleIm::protocolMsn (void) {
    return("http://schemas.google.com/g/2005#MSN");
}

QString THGoogleIm::protocolIcq (void) {
    return("http://schemas.google.com/g/2005#ICQ");
}

QString THGoogleIm::protocolYahoo (void) {
    return("http://schemas.google.com/g/2005#YAHOO");
}

QString THGoogleIm::protocolSkype (void) {
    return("http://schemas.google.com/g/2005#SKYPE");
}

QString THGoogleIm::protocolJabber (void) {
    return("http://schemas.google.com/g/2005#JABBER");
}

QString THGoogleIm::protocolGoogleTalk (void) {
    return("http://schemas.google.com/g/2005#GOOGLE_TALK");
}


void THGoogleIm::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("address"))
        d->address = attributes.value("address").toString().simplified();

    if (attributes.hasAttribute("protocol"))
        d->protocol = attributes.value("protocol").toString().simplified();

    if (attributes.hasAttribute("label"))
        d->label = attributes.value("label").toString().simplified();

    if (attributes.hasAttribute("primary"))
        d->primary = attributes.value("primary").compare("true", Qt::CaseInsensitive);

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();
}

void THGoogleIm::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    if (!d->label.isEmpty())
        xmlWriter->writeAttribute("label", d->label);

    if (!d->protocol.isEmpty())
        xmlWriter->writeAttribute("protocol", d->protocol);

    if (!d->address.isEmpty())
        xmlWriter->writeAttribute("address", d->address);

    xmlWriter->writeAttribute("primary", d->primary ? "true" : "false");
}

bool THGoogleIm::hasAddress (void) const {
    return(!d->address.isEmpty());
}

QString THGoogleIm::address (void) const {
    return(d->address);
}

void THGoogleIm::setAddress (const QString& address) {
    d->address = address;
}

bool THGoogleIm::isWorkAddress (void) const {
    return(d->rel == relWork());
}

bool THGoogleIm::isHomeAddress (void) const {
    return(d->rel == relHome());
}

bool THGoogleIm::isOtherAddress (void) const {
    return(d->rel == relOther());
}

bool THGoogleIm::hasLabel (void) const {
    return(!d->label.isEmpty());
}

QString THGoogleIm::label (void) const {
    return(d->label);
}

void THGoogleIm::setLabel (const QString& label) {
    d->label = label;
}


bool THGoogleIm::primary (void) const {
    return(d->primary);
}

void THGoogleIm::setPrimary (bool primary) {
    d->primary = primary;
}


bool THGoogleIm::hasProtocol (void) const {
    return(!d->protocol.isEmpty());
}

QString THGoogleIm::protocol (void) const {
    return(d->protocol);
}

void THGoogleIm::setProtocol (const QString& protocol) {
    d->protocol = protocol;
}


bool THGoogleIm::hasRel (void) const {
    return(!d->rel.isEmpty());
}

QString THGoogleIm::rel (void) const {
    return(d->rel);
}

void THGoogleIm::setRel (const QString& rel) {
    d->rel = rel;
}

// ===========================================================================
//  Organization
// ===========================================================================
class THGoogleOrganizationPrivate {
    public:
        QString orgTitle;
        QString orgName;
        QString label;
        bool primary;
        QString rel;
};


THGoogleOrganization::THGoogleOrganization (QObject *parent)
    : QObject(parent), d(new THGoogleOrganizationPrivate)
{
}

THGoogleOrganization::~THGoogleOrganization() {
    delete d;
}

void THGoogleOrganization::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("label"))
        d->label = attributes.value("label").toString().simplified();

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();

    if (attributes.hasAttribute("primary"))
        d->primary = attributes.value("primary").compare("true", Qt::CaseInsensitive);

    while (!xmlReader->atEnd()) {
        xmlReader->readNext();

        QStringRef name = xmlReader->qualifiedName();
        if (xmlReader->isEndElement() && name == "gd:organization")        
            break;        

        if (!xmlReader->isStartElement())
            continue;

        if (name == "gd:orgName")
            d->orgName = xmlReader->readElementText().simplified();
        else if (name == "gd:orgTitle")
            d->orgTitle = xmlReader->readElementText().simplified();
    }
}

void THGoogleOrganization::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->label.isEmpty())
        xmlWriter->writeAttribute("label", d->label);

    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    xmlWriter->writeAttribute("primary", d->primary ? "true" : "false");

    xmlWriter->writeTextElement("gd:orgName", d->orgName);
    xmlWriter->writeTextElement("gd:orgTitle", d->orgTitle);
}

bool THGoogleOrganization::hasLabel (void) const {
    return(!d->label.isEmpty());
}

QString THGoogleOrganization::label (void) const {
    return(d->label);
}

void THGoogleOrganization::setLabel (const QString& label) {
    d->label = label;
}


bool THGoogleOrganization::hasOrgName (void) const {
    return(!d->orgName.isEmpty());
}

QString THGoogleOrganization::orgName (void) const {
    return(d->orgName);
}

void THGoogleOrganization::setOrgName (const QString& orgName) {
    d->orgName = orgName;
}


bool THGoogleOrganization::hasOrgTitle (void) const {
    return(!d->orgTitle.isEmpty());
}

QString THGoogleOrganization::orgTitle (void) const {
    return(d->orgTitle);
}

void THGoogleOrganization::setOrgTitle (const QString& orgTitle) {
    d->orgTitle = orgTitle;
}


bool THGoogleOrganization::primary (void) {
    return(d->primary);
}

void THGoogleOrganization::setPrimary (bool primary) {
    d->primary = primary;
}


bool THGoogleOrganization::hasRel (void) const {
    return(!d->rel.isEmpty());
}

QString THGoogleOrganization::rel (void) const {
    return(d->rel);
}

void THGoogleOrganization::setRel (const QString& rel) {
    d->rel = rel;
}

// ===========================================================================
//  Phone Number
// ===========================================================================
class THGooglePhoneNumberPrivate {
    public:
        QString phoneNumber;
        QString label;
        bool primary;
        QString uri;
        QString rel;
};

THGooglePhoneNumber::THGooglePhoneNumber (QObject *parent)
    : QObject(parent), d(new THGooglePhoneNumberPrivate)
{
    d->primary = false;
}

THGooglePhoneNumber::~THGooglePhoneNumber() {
    delete d;
}

void THGooglePhoneNumber::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("label"))
        d->label = attributes.value("label").toString().simplified();

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();

    if (attributes.hasAttribute("uri"))
        d->uri = attributes.value("uri").toString().simplified();

    if (attributes.hasAttribute("primary"))
        d->primary = attributes.value("primary").compare("true", Qt::CaseInsensitive);

    d->phoneNumber = xmlReader->readElementText().simplified();
}

void THGooglePhoneNumber::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->label.isEmpty())
        xmlWriter->writeAttribute("label", d->label);

    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    if (!d->uri.isEmpty())
        xmlWriter->writeAttribute("uri", d->uri);

    xmlWriter->writeAttribute("primary", d->primary ? "true" : "false");
    xmlWriter->writeCharacters(d->phoneNumber);
}

QString THGooglePhoneNumber::label (void) const {
    return(d->label);
}

void THGooglePhoneNumber::setLabel (const QString& label) {
    d->label = label;
}


QString THGooglePhoneNumber::phoneNumber (void) const {
    return(d->phoneNumber);
}

void THGooglePhoneNumber::setPhoneNumber (const QString& phoneNumber) {
    d->phoneNumber = phoneNumber;
}


bool THGooglePhoneNumber::primary (void) const {
    return(d->primary);
}

void THGooglePhoneNumber::setPrimary (bool primary) {
    d->primary = primary;
}


QString THGooglePhoneNumber::rel (void) const {
    return(d->rel);
}

void THGooglePhoneNumber::setRel (const QString& rel) {
    d->rel = rel;
}


QString THGooglePhoneNumber::uri (void) const {
    return(d->uri);
}

void THGooglePhoneNumber::setUri (const QString& uri) {
    d->uri = uri;
}

// ===========================================================================
//  Postal Address
// ===========================================================================
class THGooglePostalAddressPrivate {
    public:
        QString formattedAddress;
        QString postCode;
        QString country;
        QString address;
        QString region;
        QString city;

        QString label;
        bool primary;
        QString rel;
};

THGooglePostalAddress::THGooglePostalAddress (QObject *parent)
    : QObject(parent), d(new THGooglePostalAddressPrivate)
{
    d->primary = false;
}

THGooglePostalAddress::~THGooglePostalAddress() {
    delete d;
}

void THGooglePostalAddress::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("primary"))
        d->primary = attributes.value("primary").compare("true", Qt::CaseInsensitive);

    if (attributes.hasAttribute("label"))
        d->label = attributes.value("label").toString().simplified();

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();

    while (xmlReader->atEnd()) {
        xmlReader->readNext();

        QStringRef name = xmlReader->qualifiedName();
        if (xmlReader->isEndElement() && name == "gd:structuredPostalAddress")        
            break;        

        if (!xmlReader->isStartElement())
            continue;

        if (name == "gd:formattedAddress")
            d->formattedAddress = xmlReader->readElementText().simplified();
        else if (name == "gd:postCode")
            d->postCode = xmlReader->readElementText().simplified();
        else if (name == "gd:country")
            d->country = xmlReader->readElementText().simplified();
        else if (name == "gd:address")
            d->address = xmlReader->readElementText().simplified();
        else if (name == "gd:region")
            d->region = xmlReader->readElementText().simplified();
        else if (name == "gd:city")
            d->city = xmlReader->readElementText().simplified();
        else if (name == "gd:postCode")
            d->postCode = xmlReader->readElementText().simplified();
    }
}

void THGooglePostalAddress::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->label.isEmpty())
        xmlWriter->writeAttribute("label", d->label);

    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    xmlWriter->writeAttribute("primary", d->primary ? "true" : "false");

    if (!d->formattedAddress.isEmpty())
        xmlWriter->writeTextElement("gd:formattedAddress", d->formattedAddress);

    if (!d->postCode.isEmpty())
        xmlWriter->writeTextElement("gd:postCode", d->postCode);

    if (!d->country.isEmpty())
        xmlWriter->writeTextElement("gd:country", d->country);

    if (!d->address.isEmpty())
        xmlWriter->writeTextElement("gd:address", d->address);

    if (!d->region.isEmpty())
        xmlWriter->writeTextElement("gd:region", d->region);

    if (!d->city.isEmpty())
        xmlWriter->writeTextElement("gd:city", d->city);

    if (!d->postCode.isEmpty())
        xmlWriter->writeTextElement("gd:postCode", d->postCode);
}

QString THGooglePostalAddress::label (void) const {
    return(d->label);
}

void THGooglePostalAddress::setLabel (const QString& label) {
    d->label = label;
}

bool THGooglePostalAddress::primary (void) const {
    return(d->primary);
}

void THGooglePostalAddress::setPrimary (bool primary) {
    d->primary = primary;
}


QString THGooglePostalAddress::rel (void) const {
    return(d->rel);
}

void THGooglePostalAddress::setRel (const QString& rel) {
    d->rel = rel;
}

QString THGooglePostalAddress::city (void) const {
    return(d->city);
}

void THGooglePostalAddress::setCity (const QString& city) {
    d->city = city;
}

QString THGooglePostalAddress::region (void) const {
    return(d->region);
}

void THGooglePostalAddress::setRegion (const QString& region) {
    d->region = region;
}

QString THGooglePostalAddress::address (void) const {
    return(d->address);
}

void THGooglePostalAddress::setAddress (const QString& address) {
    d->address = address;
}

QString THGooglePostalAddress::country (void) const {
    return(d->country);
}

void THGooglePostalAddress::setCountry (const QString& country) {
    d->country = country;
}

QString THGooglePostalAddress::postCode (void) const {
    return(d->postCode);
}

void THGooglePostalAddress::setPostCode (const QString& postCode) {
    d->postCode = postCode;
}

QString THGooglePostalAddress::formattedAddress (void) const {
    return(d->formattedAddress);
}

void THGooglePostalAddress::setFormattedAddress (const QString& address) {
    d->formattedAddress = address;
}

// ===========================================================================
//  Contact WebSite
// ===========================================================================
class THGoogleContactWebSitePrivate {
    public:
        QString label;
        bool primary;
        QString href;
        QString rel;
};

THGoogleContactWebSite::THGoogleContactWebSite (QObject *parent)
    : QObject(parent), d(new THGoogleContactWebSitePrivate)
{
    d->primary = false;
}

THGoogleContactWebSite::~THGoogleContactWebSite() {
    delete d;
}

QString THGoogleContactWebSite::relHomePage (void) {
    return("home-page");
}

QString THGoogleContactWebSite::relProfile (void) {
    return("profile");
}

QString THGoogleContactWebSite::relOther (void) {
    return("other");
}

QString THGoogleContactWebSite::relBlog (void) {
    return("blog");
}

QString THGoogleContactWebSite::relHome (void) {
    return("home");
}

QString THGoogleContactWebSite::relWork (void) {
    return("work");
}

QString THGoogleContactWebSite::relFtp (void) {
    return("ftp");
}

void THGoogleContactWebSite::readXml (QXmlStreamReader *xmlReader) {
    QXmlStreamAttributes attributes = xmlReader->attributes();

    if (attributes.hasAttribute("label"))
        d->label = attributes.value("label").toString().simplified();

    if (attributes.hasAttribute("rel"))
        d->rel = attributes.value("rel").toString().simplified();

    if (attributes.hasAttribute("href"))
        d->href = attributes.value("href").toString().simplified();

    if (attributes.hasAttribute("primary"))
        d->primary = attributes.value("primary").compare("true", Qt::CaseInsensitive);
}

void THGoogleContactWebSite::writeXml (QXmlStreamWriter *xmlWriter) {
    if (!d->label.isEmpty())
        xmlWriter->writeAttribute("label", d->label);

    if (!d->rel.isEmpty())
        xmlWriter->writeAttribute("rel", d->rel);

    xmlWriter->writeAttribute("primary", d->primary ? "true" : "false");
    xmlWriter->writeAttribute("href", d->href);
}

bool THGoogleContactWebSite::hasRel (void) const {
    return(!d->rel.isEmpty());
}

QString THGoogleContactWebSite::rel (void) const {
    return(d->rel);
}
       
void THGoogleContactWebSite::setRel (const QString& rel) {
    d->rel = rel;
}

bool THGoogleContactWebSite::hasHref (void) const {
    return(!d->href.isEmpty());
}

QString THGoogleContactWebSite::href (void) const {
    return(d->href);
}

void THGoogleContactWebSite::setHref (const QString& href) {
    d->href = href;
}

bool THGoogleContactWebSite::hasLabel (void) const {
    return(!d->label.isEmpty());
}

QString THGoogleContactWebSite::label (void) const {
    return(d->label);
}

void THGoogleContactWebSite::setLabel (const QString& label) {
    d->label = label;
}

bool THGoogleContactWebSite::primary (void) const {
    return(d->primary);
}

void THGoogleContactWebSite::setPrimary (bool primary) {
    d->primary = primary;
}

// ===========================================================================
//  Contact External ID
// ===========================================================================
THGoogleContactExternalId::THGoogleContactExternalId (QObject *parent)
    : THGoogleRelValueInfo(parent)
{
}

THGoogleContactExternalId::~THGoogleContactExternalId() {
}

// ===========================================================================
//  Contact Group Entry
// ===========================================================================
class THGoogleContactGroupPrivate {
    public:
        QString systemGroupId;
};

THGoogleContactGroup::THGoogleContactGroup (QObject *parent)
    : THGoogleEntry(parent), d(new THGoogleContactGroupPrivate)
{
    category()->setScheme("http://schemas.google.com/g/2005#kind");
    category()->setTerm("http://schemas.google.com/contact/2008#group");
}

THGoogleContactGroup::~THGoogleContactGroup() {
    delete d;
}

QString THGoogleContactGroup::systemGroupIdCoworkers (void) {
    return("Coworkers");
}

QString THGoogleContactGroup::systemGroupIdContacts (void) {
    return("Contacts");
}

QString THGoogleContactGroup::systemGroupIdFriends (void) {
    return("Friends");
}

QString THGoogleContactGroup::systemGroupIdFamily (void) {
    return("Family");
}

QString THGoogleContactGroup::systemGroupId (void) const {
    return(d->systemGroupId);
}

void THGoogleContactGroup::setSystemGroupId (const QString& groupId) {
    d->systemGroupId = groupId;
}

bool THGoogleContactGroup::readXmlElement (QXmlStreamReader *xmlReader) {
    QStringRef name = xmlReader->qualifiedName();

    if (name == "gContact:systemGroup") {
        QXmlStreamAttributes attributes = xmlReader->attributes();
        if (attributes.hasAttribute("id"))
            d->systemGroupId = attributes.value("id").toString().simplified();
        return(true);
    }

    return(false);
}

void THGoogleContactGroup::writeXmlElement (QXmlStreamWriter *xmlWriter) const {
    if (!d->systemGroupId.isEmpty()) {
        xmlWriter->writeEmptyElement("gContact:systemGroup");
        xmlWriter->writeAttribute("id", d->systemGroupId);
    }
}

// ===========================================================================
//  Contact Entry
// ===========================================================================
class THGoogleContactPrivate {
    public:
        QList<THGoogleGroupMembershipInfo *> groupMembershipInfos;
        QList<THGoogleContactUserField> userDefinedFields;
        QList<THGoogleContactExternalId *> externalIds;
        QList<THGooglePostalAddress *> postalAddresses;
        QList<THGoogleOrganization *> organizations;
        QList<THGooglePhoneNumber *> phoneNumbers;
        QList<THGoogleContactWebSite *> webSites;
        QList<THGoogleEmail *> emailAddresses;
        QList<THGoogleIm *> imAddresses;

        THGoogleLink *editPhotoLink;
        THGoogleLink *photoLink;

        QString familyName;
        QString givenName;
        QString fullName;
};

THGoogleContact::THGoogleContact (QObject *parent)
    : THGoogleEntry(parent), d(new THGoogleContactPrivate)
{
    d->editPhotoLink = NULL;
    d->photoLink = NULL;
    addAdapter(this);

    // Setup Category
    category()->setScheme("http://schemas.google.com/g/2005#kind");
    category()->setTerm("http://schemas.google.com/contact/2008#contact");
}

THGoogleContact::~THGoogleContact() {
    delete d;
}

void THGoogleContact::clear (void) {
    THGoogleEntry::clear();

    d->groupMembershipInfos.clear();
    d->userDefinedFields.clear();
    d->postalAddresses.clear();
    d->emailAddresses.clear();
    d->organizations.clear();
    d->phoneNumbers.clear();
    d->externalIds.clear();
    d->imAddresses.clear();

    if (d->editPhotoLink != NULL) {
        d->editPhotoLink->deleteLater();
        d->editPhotoLink = NULL;
    }

    if (d->photoLink != NULL) {
        d->photoLink->deleteLater();
        d->photoLink = NULL;
    }

    d->familyName.clear();
    d->givenName.clear();
    d->fullName.clear();
}

bool THGoogleContact::readXmlElement (QXmlStreamReader *xmlReader) {
    QStringRef name = xmlReader->qualifiedName();

    if (name == "gd:im") {
        THGoogleIm *imAddress = new THGoogleIm(this);
        imAddress->readXml(xmlReader);
        d->imAddresses.append(imAddress);
        return(true);
    } else if (name == "gd:email") {
        THGoogleEmail *emailAddress = new THGoogleEmail(this);
        emailAddress->readXml(xmlReader);
        d->emailAddresses.append(emailAddress);
        return(true);
    } else if (name == "gd:phoneNumber") {
        THGooglePhoneNumber *phoneNumber = new THGooglePhoneNumber(this);
        phoneNumber->readXml(xmlReader);
        d->phoneNumbers.append(phoneNumber);
        return(true);
    } else if (name == "gd:organization") {
        THGoogleOrganization *organization = new THGoogleOrganization(this);
        organization->readXml(xmlReader);
        d->organizations.append(organization);
        return(true);
    } else if (name == "gd:structuredPostalAddress") {
        THGooglePostalAddress *postalAddress = new THGooglePostalAddress(this);
        postalAddress->readXml(xmlReader);
        d->postalAddresses.append(postalAddress);
        return(true);
    } else if (name == "gd:fullName") {
        d->fullName = xmlReader->readElementText().simplified();
        return(true);
    } else if (name == "gd:familyName") {
        d->familyName = xmlReader->readElementText().simplified();
        return(true);
    } else if (name == "gd:givenName") {
        d->givenName = xmlReader->readElementText().simplified();
        return(true);
    } else if (name == "gContact:groupMembershipInfo") {
        THGoogleGroupMembershipInfo *info = new THGoogleGroupMembershipInfo(this);
        info->readXml(xmlReader);
        d->groupMembershipInfos.append(info);
        return(true);
    } else if (name == "gContact:userDefinedField") {
        QXmlStreamAttributes attributes = xmlReader->attributes();
        if (!attributes.hasAttribute("key") || !attributes.hasAttribute("value"))
            return(false);

        QString key = attributes.value("key").toString();
        QString value = attributes.value("value").toString();
        d->userDefinedFields.append(THGoogleContactUserField(key, value));
        return(true);
    } else if (name == "gContact:website") {
        THGoogleContactWebSite *webSite = new THGoogleContactWebSite(this);
        webSite->readXml(xmlReader);
        d->webSites.append(webSite);
        return(true);
    } else if (name == "gContact:externalId") {
        THGoogleContactExternalId *externalId = new THGoogleContactExternalId(this);
        externalId->readXml(xmlReader);
        d->externalIds.append(externalId);
        return(true);
    }

    return(false);
}

void THGoogleContact::writeXmlElement (QXmlStreamWriter *xmlWriter) const {
    foreach (THGoogleEmail *email, d->emailAddresses) {
        xmlWriter->writeStartElement("gd:email");
        email->writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    foreach (THGoogleIm *im, d->imAddresses) {
        xmlWriter->writeStartElement("gd:im");
        im->writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    foreach (THGoogleGroupMembershipInfo *groupInfo, d->groupMembershipInfos) {
        xmlWriter->writeStartElement("gContact:groupMembershipInfo");
        groupInfo->writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    foreach (THGooglePostalAddress *postalAddress, d->postalAddresses) {
        xmlWriter->writeStartElement("gd:structuredPostalAddress");
        postalAddress->writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    foreach (THGoogleOrganization *organization, d->organizations) {
        xmlWriter->writeStartElement("gd:organization");
        organization->writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    foreach (THGooglePhoneNumber *phoneNumber, d->phoneNumbers) {
        xmlWriter->writeStartElement("gd:phoneNumber");
        phoneNumber->writeXml(xmlWriter);
        xmlWriter->writeEndElement();
    }

    foreach (THGoogleContactUserField field, d->userDefinedFields) {
        xmlWriter->writeStartElement("gContact:userDefinedField");
        xmlWriter->writeAttribute("key", field.first);
        xmlWriter->writeAttribute("value", field.second);
        xmlWriter->writeEndElement();
    }

    foreach (THGoogleContactWebSite *webSite, d->webSites) {
        xmlWriter->writeEmptyElement("gContact:webSite");
        webSite->writeXml(xmlWriter);
    } 

    foreach (THGoogleContactExternalId *externalId, d->externalIds) {
        xmlWriter->writeEmptyElement("gContact:externalId");        
        externalId->writeXml(xmlWriter);
    }

    xmlWriter->writeStartElement("gd:name");
    if (!d->fullName.isEmpty())
        xmlWriter->writeTextElement("gd:fullName", d->fullName);

    if (!d->givenName.isEmpty())
        xmlWriter->writeTextElement("gd:givenName", d->givenName);

    if (!d->familyName.isEmpty())
        xmlWriter->writeTextElement("gd:familyName", d->familyName);
    xmlWriter->writeEndElement();
}

void THGoogleContact::addImAddress (THGoogleIm *imAddress) {
    imAddress->setParent(this);
    d->imAddresses.append(imAddress);
}

void THGoogleContact::addEmailAddress (THGoogleEmail *emailAddress) {
    emailAddress->setParent(this);
    d->emailAddresses.append(emailAddress);
}

void THGoogleContact::addPhoneNumber (THGooglePhoneNumber *phoneNumber) {
    phoneNumber->setParent(this);
    d->phoneNumbers.append(phoneNumber);
}

void THGoogleContact::addOrganization (THGoogleOrganization *organization) {
    organization->setParent(this);
    d->organizations.append(organization);
}

void THGoogleContact::addPostalAddress (THGooglePostalAddress *postalAddress) {
    postalAddress->setParent(this);
    d->postalAddresses.append(postalAddress);
}

void THGoogleContact::addUserDefinedField (const QString& key, const QString& value) {
    d->userDefinedFields.append(THGoogleContactUserField(key, value));
}

void THGoogleContact::addGroupMembershipInfo (THGoogleGroupMembershipInfo *membershipInfo) {
    membershipInfo->setParent(this);
    d->groupMembershipInfos.append(membershipInfo);
}

QString THGoogleContact::fullName (void) const {
    if (d->fullName.isEmpty())
        return(title());
    return(d->fullName);
}

void THGoogleContact::setFullName (const QString& fullName) {
    d->fullName = fullName;
}

QString THGoogleContact::givenName (void) const {
    return(d->givenName);
}

void THGoogleContact::setGivenName (const QString& givenName) {
    d->givenName = givenName;
}

QString THGoogleContact::familyName (void) const {
    return(d->familyName);
}

void THGoogleContact::setFamilyName (const QString& familyName) {
    d->familyName = familyName;
}


QList<THGoogleIm *> THGoogleContact::imAddresses (void) const {
    return(d->imAddresses);
}

QList<THGoogleEmail *> THGoogleContact::emailAddresses (void) const {
    return(d->emailAddresses);
}

QList<THGooglePhoneNumber *> THGoogleContact::phoneNumbers (void) const {
    return(d->phoneNumbers);
}

QList<THGoogleOrganization *> THGoogleContact::organizations (void) const {
    return(d->organizations);
}

QList<THGooglePostalAddress *> THGoogleContact::postalAddresses (void) const {
    return(d->postalAddresses);
}

QList<THGoogleContactExternalId *> THGoogleContact::externalIds (void) const {
    return(d->externalIds);
}

QList<THGoogleContactUserField> THGoogleContact::userDefinedFields (void) const {
    return(d->userDefinedFields);
}

QList<THGoogleGroupMembershipInfo *> THGoogleContact::groupMembershipInfos (void) const {
    return(d->groupMembershipInfos);
}

bool THGoogleContact::hasImAddresses (void) const {
    return(!d->imAddresses.isEmpty());
}

bool THGoogleContact::hasPhoneNumbers (void) const {
    return(!d->phoneNumbers.isEmpty());
}

bool THGoogleContact::hasOrganizations (void) const {
    return(!d->organizations.isEmpty());
}

bool THGoogleContact::hasPostalAddresses (void) const {
    return(!d->postalAddresses.isEmpty());
}

bool THGoogleContact::hasEmailAddresses (void) const {
    return(!d->emailAddresses.isEmpty());
}

bool THGoogleContact::hasGroupMembershipInfos (void) const {
    return(!d->groupMembershipInfos.isEmpty());
}

bool THGoogleContact::hasPhoto (void) const {
    return(photoLink() != NULL && photoLink()->hasEtag());
}

bool THGoogleContact::hasPhotoLink (void) const {
    return(photoLink() != NULL);
}

THGoogleLink *THGoogleContact::photoLink (void) const {
    if (d->photoLink == NULL)
        d->photoLink = link(relPhoto(), "image/*");
    return(d->photoLink);
}

bool THGoogleContact::hasEditPhoto (void) const {
    return(photoLink() != NULL && photoLink()->hasEtag());
}

bool THGoogleContact::hasEditPhotoLink (void) const {
    return(editPhotoLink() != NULL);
}

THGoogleLink *THGoogleContact::editPhotoLink (void) const {
    if (d->editPhotoLink == NULL)
        d->editPhotoLink = link(relEditPhoto(), "image/*");
    return(d->editPhotoLink);
}

// STATIC Methods
QString THGoogleContact::relPhoto (void) {
    return("http://schemas.google.com/contacts/2008/rel#photo");
}

QString THGoogleContact::relEditPhoto (void) {
    return("http://schemas.google.com/contacts/2008/rel#edit-photo");
}

// ========================================================================
//  Contacts Service (Executer)
// ========================================================================

#include <QDebug>
class THGoogleContactService {
    public:
        enum RequestType { 
            InsertContact, UpdateContact, DeleteContact, RefreshContact, 
            UpdateContactPhoto, UpdateContactPhotoEdit,
            InsertGroup, UpdateGroup, DeleteGroup, RefreshGroup,
            RetrievingContacts, RetrievingGroups
        };

    public:
        THGoogleContactService (RequestType requestType);
        THGoogleContactService (THGoogleEntry *entity, 
                                RequestType requestType);
        ~THGoogleContactService();

        void run (THGoogleContactsService *service, QNetworkReply *reply);

    private:
        void parseInsertContactResponse (THGoogleContactsService *service);
        void parseUpdateContactResponse (THGoogleContactsService *service);
        void parseDeleteContactResponse (THGoogleContactsService *service);
        void parseRefreshContactResponse (THGoogleContactsService *service);
        void parseUpdateContactPhotoResponse (THGoogleContactsService *service);
        void parseUpdateContactPhotoEditResponse (THGoogleContactsService *service);

        void parseInsertGroupResponse (THGoogleContactsService *service);
        void parseUpdateGroupResponse (THGoogleContactsService *service);
        void parseDeleteGroupResponse (THGoogleContactsService *service);
        void parseRefreshGroupResponse (THGoogleContactsService *service);

        void parseRetrievingGroupsResponse (THGoogleContactsService *service);
        void parseRetrievingContactsResponse (THGoogleContactsService *service);

    private:
        THGoogleEntry *m_entry;
        RequestType m_requestType;
        QNetworkReply *m_reply;
};

THGoogleContactService::THGoogleContactService (RequestType requestType) {
    m_requestType = requestType;
    m_entry = NULL;
}

THGoogleContactService::THGoogleContactService (THGoogleEntry *entity,
                                                RequestType requestType)
{
    m_requestType = requestType;
    m_entry = entity;
}

THGoogleContactService::~THGoogleContactService() {
}

void THGoogleContactService::run (THGoogleContactsService *service, 
                                  QNetworkReply *reply)
{
    m_reply = reply;

    // Check if Response is Valid
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode != 200 && statusCode != 201) {
        service->setErrorString(reply->readAll());
        emit service->finished(false);
        return;
    }

    switch (m_requestType) {
        case InsertContact:
            parseInsertContactResponse(service);
            return;
        case UpdateContact:
            parseUpdateContactResponse(service);
            return;
        case DeleteContact:
            parseDeleteContactResponse(service);
            return;
        case RefreshContact:
            parseRefreshContactResponse(service);
            return;
        case UpdateContactPhoto:
            parseUpdateContactPhotoResponse(service);
            return;
        case UpdateContactPhotoEdit:
            parseUpdateContactPhotoEditResponse(service);
            return;
        case InsertGroup:
            parseInsertGroupResponse(service);
            return;
        case UpdateGroup:
            parseUpdateGroupResponse(service);
            return;
        case DeleteGroup:
            parseDeleteGroupResponse(service);
            return;
        case RefreshGroup:
            parseRefreshGroupResponse(service);
            return;
        case RetrievingGroups:
            parseRetrievingGroupsResponse(service);
            return;
        case RetrievingContacts:
            parseRetrievingContactsResponse(service);
            return;
    }

    emit service->finished(true);
}

void THGoogleContactService::parseInsertContactResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(false);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry") {
                m_entry->clear();
                m_entry->readXml(&xmlReader);
            }
        }

        service->refreshContact(static_cast<THGoogleContact *>(m_entry));
    }
}

void THGoogleContactService::parseUpdateContactResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry")
                m_entry->readXml(&xmlReader);
        }

        service->refreshContact(static_cast<THGoogleContact *>(m_entry));
    }
}

void THGoogleContactService::parseDeleteContactResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        emit service->finished(false);
    }
}

void THGoogleContactService::parseRefreshContactResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry") {
                m_entry->clear();
                m_entry->readXml(&xmlReader);
            }
        }

        emit service->finished(false);
    }
}

void THGoogleContactService::parseUpdateContactPhotoResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry")
                m_entry->readXml(&xmlReader);
        }

        service->refreshContact(static_cast<THGoogleContact *>(m_entry));
    }
}

void THGoogleContactService::parseUpdateContactPhotoEditResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry")
                m_entry->readXml(&xmlReader);
        }

        service->refreshContact(static_cast<THGoogleContact *>(m_entry));
    }
}

void THGoogleContactService::parseInsertGroupResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry") {
                m_entry->clear();
                m_entry->readXml(&xmlReader);
            }
        }

        service->refreshGroup(static_cast<THGoogleContactGroup *>(m_entry));
    }
}

void THGoogleContactService::parseUpdateGroupResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry")
                m_entry->readXml(&xmlReader);
        }

        service->refreshGroup(static_cast<THGoogleContactGroup *>(m_entry));
    }
}

void THGoogleContactService::parseDeleteGroupResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        emit service->finished(false);
    }
}

void THGoogleContactService::parseRefreshGroupResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry") {
                m_entry->clear();
                m_entry->readXml(&xmlReader);
            }
        }

        emit service->finished(false);
    }
}

void THGoogleContactService::parseRetrievingGroupsResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QList<THGoogleContactGroup *> groupList;

        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry") {
                THGoogleContactGroup *group = new THGoogleContactGroup;
                groupList.append(group);

                group->readXml(&xmlReader);
            }
        }

        emit service->finished(false);
        service->setGroupList(groupList);
    }
}

void THGoogleContactService::parseRetrievingContactsResponse (THGoogleContactsService *service) {
    QByteArray data = m_reply->readAll();
    if (data.trimmed().isEmpty()) {
        emit service->finished(true);
    } else {
        QList<THGoogleContact *> contactList;

        QXmlStreamReader xmlReader(data);
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();

            if (xmlReader.name() == "entry") {
                THGoogleContact *contact = new THGoogleContact;
                contactList.append(contact);

                contact->readXml(&xmlReader);
            }
        }

        emit service->finished(false);
        service->setContactList(contactList);
    }
}

// ========================================================================
//  Contacts Service
// ========================================================================
class THGoogleContactsServicePrivate {
    public:
        QHash<QNetworkReply *, THGoogleContactService *> requests;
        QList<THGoogleContactGroup *> groups;
        QList<THGoogleContact *> contacts;
};

THGoogleContactsService::THGoogleContactsService (QObject *parent)
    : THGooglEntryService(parent), d(new THGoogleContactsServicePrivate)
{
    setGDataVersion("3.0");
}

THGoogleContactsService::~THGoogleContactsService() {
}

void THGoogleContactsService::insertContact (THGoogleContact *contact) {
    QUrl url("http://www.google.com/m8/feeds/contacts/default/full");
    QNetworkReply *reply = insertEntry(url, contact->toXml());

    THGoogleContactService *service = new THGoogleContactService(contact,   
                                         THGoogleContactService::InsertContact);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::updateContact (THGoogleContact *contact) {
    THGoogleLink *editLink = contact->link("edit", "application/atom+xml");
    QNetworkReply *reply = updateEntry(QUrl::fromEncoded(editLink->href().toLatin1()),
                                       contact->etag().toLatin1(),
                                       contact->toXml());

    THGoogleContactService *service = new THGoogleContactService(contact,   
                                         THGoogleContactService::UpdateContact);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::deleteContact (THGoogleContact *contact) {
    QNetworkReply *reply = deleteEntry(QUrl::fromEncoded(contact->id().toLatin1()),
                                       contact->etag().toLatin1(),
                                       contact->toXml());

    THGoogleContactService *service = new THGoogleContactService(contact,   
                                           THGoogleContactService::DeleteContact);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::refreshContact (THGoogleContact *contact) {
    THGoogleLink *selfLink = contact->link("self", "application/atom+xml");
    refreshContact(contact, QUrl::fromEncoded(selfLink->href().toLatin1()));
}

void THGoogleContactsService::refreshContact (THGoogleContact *contact, const QUrl& url) {
    QNetworkReply *reply = queryEntry(url);
    THGoogleContactService *service = new THGoogleContactService(contact,   
                                       THGoogleContactService::RefreshContact);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::addContactPhoto (THGoogleContact *contact, 
                                               const QImage& image)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    addContactPhoto(contact, data);
}

void THGoogleContactsService::addContactPhoto (THGoogleContact *contact, 
                                               const QPixmap& pixmap)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");

    addContactPhoto(contact, data);
}

void THGoogleContactsService::addContactPhoto (THGoogleContact *contact, 
                                               const QByteArray& pngData)
{
    QUrl url = QUrl::fromEncoded(contact->photoLink()->href().toLatin1());

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
    request.setRawHeader("If-Match", "*");

    QNetworkReply *reply = updateEntry(request, pngData);
    THGoogleContactService *service = new THGoogleContactService(contact,   
                                   THGoogleContactService::UpdateContactPhoto);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::addContactEditPhoto (THGoogleContact *contact, 
                                                   const QImage& image)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    addContactEditPhoto(contact, data);
}

void THGoogleContactsService::addContactEditPhoto (THGoogleContact *contact, 
                                                   const QPixmap& pixmap)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    addContactEditPhoto(contact, data);
}

void THGoogleContactsService::addContactEditPhoto (THGoogleContact *contact, 
                                                   const QByteArray& pngData)
{
    QUrl url = QUrl::fromEncoded(contact->editPhotoLink()->href().toLatin1());
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
    request.setRawHeader("If-Match", "*");

    QNetworkReply *reply = updateEntry(request, pngData);
    THGoogleContactService *service = new THGoogleContactService(contact,   
                              THGoogleContactService::UpdateContactPhotoEdit);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::insertGroup (THGoogleContactGroup *group) {
    QUrl url("http://www.google.com/m8/feeds/groups/default/full");
    QNetworkReply *reply = insertEntry(url, group->toXml());

    THGoogleContactService *service = new THGoogleContactService(group,   
                                         THGoogleContactService::InsertGroup);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::updateGroup (THGoogleContactGroup *group) {
    THGoogleLink *editLink = group->link("edit", "application/atom+xml");
    QNetworkReply *reply = updateEntry(QUrl::fromEncoded(editLink->href().toLatin1()),
                                       group->etag().toLatin1(),
                                       group->toXml());

    THGoogleContactService *service = new THGoogleContactService(group,   
                                         THGoogleContactService::UpdateGroup);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::deleteGroup (THGoogleContactGroup *group) {
    QNetworkReply *reply = deleteEntry(QUrl::fromEncoded(group->id().toLatin1()),
                                       group->etag().toLatin1(),
                                       group->toXml());

    THGoogleContactService *service = new THGoogleContactService(group,   
                                           THGoogleContactService::DeleteGroup);
    d->requests.insert(reply, service);
}

void THGoogleContactsService::refreshGroup (THGoogleContactGroup *group) {
    THGoogleLink *selfLink = group->link("self", "application/atom+xml");
    refreshGroup(group, QUrl::fromEncoded(selfLink->href().toLatin1()));
}

void THGoogleContactsService::refreshGroup (THGoogleContactGroup *group, const QUrl& url) {
    QNetworkReply *reply = queryEntry(url);
    THGoogleContactService *service = new THGoogleContactService(group,   
                                       THGoogleContactService::RefreshGroup);
    d->requests.insert(reply, service);
}

//http://www.google.com/m8/feeds/contacts/default/projection
//http://www.google.com/m8/feeds/contacts/default/full
void THGoogleContactsService::retrieveAllContacts (void) {
    QNetworkReply *reply = queryEntry(QUrl("http://www.google.com/m8/feeds/contacts/default/full"));

    THGoogleContactService *service = new THGoogleContactService(   
                                       THGoogleContactService::RetrievingContacts);
    d->requests.insert(reply, service);
}


//http://www.google.com/m8/feeds/groups/default/projection
//http://www.google.com/m8/feeds/groups/default/full
void THGoogleContactsService::retrieveAllGroups (void) {
    QNetworkReply *reply = queryEntry(QUrl("http://www.google.com/m8/feeds/groups/default/full"));
    THGoogleContactService *service = new THGoogleContactService(   
                                       THGoogleContactService::RetrievingGroups);
    d->requests.insert(reply, service);
}

QList<THGoogleContact *> THGoogleContactsService::contactList (void) const {
    return(d->contacts);
}

QList<THGoogleContactGroup *> THGoogleContactsService::groupList (void) const {
    return(d->groups);
}

void THGoogleContactsService::parseResponse (QNetworkReply *reply) {
    if (!d->requests.contains(reply)) {
        THGooglEntryService::parseResponse(reply);
        return;
    }

    THGoogleContactService *service = d->requests.take(reply);
    service->run(this, reply);
}

void THGoogleContactsService::setContactList (const QList<THGoogleContact *>& contacts) {
    // TODO: Free Prev List
    d->contacts = contacts;
}

void THGoogleContactsService::setGroupList (const QList<THGoogleContactGroup *>& groups) {
    // TODO: Free Prev List
    d->groups = groups;
}


