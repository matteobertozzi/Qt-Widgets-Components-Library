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

#ifndef _GOOGLECONTACTS_H_
#define _GOOGLECONTACTS_H_

#include <QPair>
class QXmlStreamReader;
class QXmlStreamWriter;
class QPixmap;
class QImage;

#include "googleentry.h"

class THGoogleEmailPrivate;
class THGoogleEmail : public QObject {
    Q_OBJECT

    public:
        THGoogleEmail (QObject *parent = 0);
        ~THGoogleEmail();

        // Static Methods
        static QString relHome (void);
        static QString relWork (void);
        static QString relOther (void);

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        QString label (void) const;
        void setLabel (const QString& label);

        QString address (void) const;
        void setAddress (const QString& address);

        bool isWorkAddress (void) const;
        bool isHomeAddress (void) const;
        bool isOtherAddress (void) const;

        QString displayName (void) const;
        void setDisplayName (const QString& displayName);

        bool primary (void) const;
        void setPrimary (bool primary);

        QString rel (void) const;
        void setRel (const QString& rel);

        QString quota (void) const;
        void setQuota (const QString& quota);

    private:
        THGoogleEmailPrivate *d;
};

class THGoogleGroupMembershipInfoPrivate;
class THGoogleGroupMembershipInfo : public QObject {
    Q_OBJECT

    public:
        THGoogleGroupMembershipInfo (QObject *parent = 0);
        ~THGoogleGroupMembershipInfo();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        bool deleted (void) const;
        void setDeleted (bool deleted);

        bool hasHref (void) const;
        QString href (void) const;
        void setHref (const QString& href);

    private:
        THGoogleGroupMembershipInfoPrivate *d;
};

class THGoogleImPrivate;
class THGoogleIm : public QObject {
    Q_OBJECT

    public:
        THGoogleIm (QObject *parent = 0);
        ~THGoogleIm();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Static Methods
        static QString relHome (void);
        static QString relWork (void);
        static QString relOther (void);

        static QString protocolQq (void);
        static QString protocolAim (void);
        static QString protocolMsn (void);
        static QString protocolIcq (void);
        static QString protocolYahoo (void);
        static QString protocolSkype (void);
        static QString protocolJabber (void);
        static QString protocolGoogleTalk (void);

        // Properties
        bool hasAddress (void) const;
        QString address (void) const;
        void setAddress (const QString& address);

        bool isWorkAddress (void) const;
        bool isHomeAddress (void) const;
        bool isOtherAddress (void) const;

        bool hasLabel (void) const;
        QString label (void) const;
        void setLabel (const QString& label);

        bool primary (void) const;
        void setPrimary (bool primary);

        bool hasProtocol (void) const;
        QString protocol (void) const;
        void setProtocol (const QString& protocol);

        bool hasRel (void) const;
        QString rel (void) const;
        void setRel (const QString& rel);

    private:
        THGoogleImPrivate *d;
};

class THGoogleOrganizationPrivate;
class THGoogleOrganization : public QObject {
    Q_OBJECT

    public:
        THGoogleOrganization (QObject *parent = 0);
        ~THGoogleOrganization();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        bool hasLabel (void) const;
        QString label (void) const;
        void setLabel (const QString& label);

        bool hasOrgName (void) const;
        QString orgName (void) const;
        void setOrgName (const QString& orgName);

        bool hasOrgTitle (void) const;        
        QString orgTitle (void) const;
        void setOrgTitle (const QString& orgTitle);

        bool primary (void);
        void setPrimary (bool primary);

        bool hasRel (void) const;
        QString rel (void) const;
        void setRel (const QString& rel);

    private:
        THGoogleOrganizationPrivate *d;
};

class THGooglePhoneNumberPrivate;
class THGooglePhoneNumber : public QObject {
    Q_OBJECT

    public:
        THGooglePhoneNumber (QObject *parent = 0);
        ~THGooglePhoneNumber();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        QString label (void) const;
        void setLabel (const QString& label);

        QString phoneNumber (void) const;
        void setPhoneNumber (const QString& phoneNumber);

        bool primary (void) const;
        void setPrimary (bool primary);

        QString rel (void) const;
        void setRel (const QString& rel);

        QString uri (void) const;
        void setUri (const QString& uri);

    private:
        THGooglePhoneNumberPrivate *d;
};

class THGooglePostalAddressPrivate;
class THGooglePostalAddress : public QObject {
    Q_OBJECT

    public:
        THGooglePostalAddress (QObject *parent = 0);
        ~THGooglePostalAddress();

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        QString label (void) const;
        void setLabel (const QString& label);

        bool primary (void) const;
        void setPrimary (bool primary);

        QString rel (void) const;
        void setRel (const QString& rel);

        QString city (void) const;
        void setCity (const QString& city);

        QString region (void) const;
        void setRegion (const QString& region);

        QString address (void) const;
        void setAddress (const QString& address);

        QString country (void) const;
        void setCountry (const QString& country);

        QString postCode (void) const;
        void setPostCode (const QString& postCode);

        QString formattedAddress (void) const;
        void setFormattedAddress (const QString& address);

    private:
        THGooglePostalAddressPrivate *d;
};

class THGoogleContactWebSitePrivate;
class THGoogleContactWebSite : public QObject {
    Q_OBJECT

    public:
        THGoogleContactWebSite (QObject *parent = 0);
        ~THGoogleContactWebSite();

        // Static Methods
        static QString relHomePage (void);
        static QString relProfile (void);
        static QString relOther (void);
        static QString relBlog (void);
        static QString relHome (void);
        static QString relWork (void);
        static QString relFtp (void);

        // Methods
        void readXml (QXmlStreamReader *xmlReader);
        void writeXml (QXmlStreamWriter *xmlWriter);

        // Properties
        bool hasRel (void) const;
        QString rel (void) const;       
        void setRel (const QString& rel);

        bool hasHref (void) const;
        QString href (void) const;
        void setHref (const QString& href);

        bool hasLabel (void) const;
        QString label (void) const;
        void setLabel (const QString& label);

        bool primary (void) const;
        void setPrimary (bool primary);

    private:
        THGoogleContactWebSitePrivate *d;
};

class THGoogleContactExternalId : public THGoogleRelValueInfo {
    Q_OBJECT

    public:
        THGoogleContactExternalId (QObject *parent = 0);
        ~THGoogleContactExternalId();
};

class THGoogleContactGroupPrivate;
class THGoogleContactGroup : public THGoogleEntry, public THGoogleEntryAdapter {
    Q_OBJECT

    public:
        THGoogleContactGroup (QObject *parent = 0);
        ~THGoogleContactGroup();

        // Static Methods
        static QString systemGroupIdCoworkers (void);
        static QString systemGroupIdContacts (void);
        static QString systemGroupIdFriends (void);
        static QString systemGroupIdFamily (void);

        // Properties
        QString systemGroupId (void) const;
        void setSystemGroupId (const QString& groupId);

    public:
        bool readXmlElement (QXmlStreamReader *xmlReader);
        void writeXmlElement (QXmlStreamWriter *xmlWriter) const;

    private:
        THGoogleContactGroupPrivate *d;
};

typedef QPair<QString, QString> THGoogleContactUserField;

class THGoogleContactPrivate;
class THGoogleContact : public THGoogleEntry, public THGoogleEntryAdapter {
    Q_OBJECT

    public:
        THGoogleContact (QObject *parent = 0);
        ~THGoogleContact();

        virtual void clear (void);

        void addImAddress (THGoogleIm *address);
        void addEmailAddress (THGoogleEmail *address);
        void addPhoneNumber (THGooglePhoneNumber *phoneNumber);
        void addOrganization (THGoogleOrganization *organization);
        void addPostalAddress (THGooglePostalAddress *postalAddress);
        void addUserDefinedField (const QString& key, const QString& value);
        void addGroupMembershipInfo (THGoogleGroupMembershipInfo *membershipInfo);

        //void declareExtension (THGoogleExtensionProfile *extensionProfile);

        QString fullName (void) const;  
        void setFullName (const QString& fullName);

        QString givenName (void) const;
        void setGivenName (const QString& givenName);

        QString familyName (void) const;
        void setFamilyName (const QString& familyName);

        bool hasPhoto (void) const;
        bool hasPhotoLink (void) const;
        THGoogleLink *photoLink (void) const;

        bool hasEditPhoto (void) const;
        bool hasEditPhotoLink (void) const;
        THGoogleLink *editPhotoLink (void) const;

        QList<THGoogleIm *> imAddresses (void) const;
        QList<THGoogleEmail *> emailAddresses (void) const;
        QList<THGooglePhoneNumber *> phoneNumbers (void) const;
        QList<THGoogleOrganization *> organizations (void) const;
        QList<THGooglePostalAddress *> postalAddresses (void) const;
        QList<THGoogleContactExternalId *> externalIds (void) const;
        QList<THGoogleContactUserField> userDefinedFields (void) const;
        QList<THGoogleGroupMembershipInfo *> groupMembershipInfos (void) const;

        bool hasImAddresses (void) const;
        bool hasPhoneNumbers (void) const;
        bool hasOrganizations (void) const;
        bool hasEmailAddresses (void) const;
        bool hasPostalAddresses (void) const;
        bool hasGroupMembershipInfos (void) const;

    public:
        static QString relPhoto (void);
        static QString relEditPhoto (void);

    public:
        bool readXmlElement (QXmlStreamReader *xmlReader);
        void writeXmlElement (QXmlStreamWriter *xmlWriter) const;

    private:
        THGoogleContactPrivate *d;
};

class THGoogleContactService;
class THGoogleContactsServicePrivate;
class THGoogleContactsService : public THGooglEntryService {
    Q_OBJECT

    friend class THGoogleContactService;

    public:
        THGoogleContactsService (QObject *parent = 0);
        ~THGoogleContactsService();

        // Methods
        void insertContact (THGoogleContact *contact);
        void updateContact (THGoogleContact *contact);
        void deleteContact (THGoogleContact *contact);
        void refreshContact (THGoogleContact *contact);
        void refreshContact (THGoogleContact *contact, const QUrl& url);

        void addContactPhoto (THGoogleContact *contact, 
                              const QImage& image);
        void addContactPhoto (THGoogleContact *contact, 
                              const QPixmap& pixmap);
        void addContactPhoto (THGoogleContact *contact, 
                              const QByteArray& pngData);

        void addContactEditPhoto (THGoogleContact *contact, 
                                  const QImage& image);
        void addContactEditPhoto (THGoogleContact *contact, 
                                  const QPixmap& pixmap);
        void addContactEditPhoto (THGoogleContact *contact, 
                                  const QByteArray& pngData);

        void insertGroup (THGoogleContactGroup *group);
        void updateGroup (THGoogleContactGroup *group);
        void deleteGroup (THGoogleContactGroup *group);
        void refreshGroup (THGoogleContactGroup *group);
        void refreshGroup (THGoogleContactGroup *group, const QUrl& url);

        void retrieveAllContacts (void);
        void retrieveAllGroups (void);

        QList<THGoogleContact *> contactList (void) const;
        QList<THGoogleContactGroup *> groupList (void) const;

    protected:
        void parseResponse (QNetworkReply *reply);

    protected:
        void setContactList (const QList<THGoogleContact *>& contacts);
        void setGroupList (const QList<THGoogleContactGroup *>& groups);

    private:
        THGoogleContactsServicePrivate *d;
};

#endif /* !_GOOGLECONTACTS_H_ */

