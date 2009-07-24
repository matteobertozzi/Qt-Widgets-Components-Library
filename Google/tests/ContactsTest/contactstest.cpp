#ifdef TEST_GOOGLE_CONTACTS

#include <QtTest>

#include <QXmlStreamWriter>
#include <QDebug>

#include "contactstest.h"

#include "googlecontacts.h"
#include "googleauth.h"

#define GOOGLE_USERNAME         "USER@gmail.com"
#define GOOGLE_PASSWORD         "PASSWORD"

ContactsTest::ContactsTest (QObject *parent)
    : QObject(parent)
{
    QEventLoop l;
    THGoogleAuth gAuth(GOOGLE_USERNAME);
    connect(&gAuth, SIGNAL(authenticated()), &l, SLOT(quit()));
    gAuth.login(THGoogleAuth::Contacts, GOOGLE_PASSWORD);
    l.exec();

    m_contact = NULL;
    if (gAuth.error() != THGoogleAuth::NoError) {
        qDebug() << "AUTH FAILED" << gAuth.errorString();
        m_gContacts = NULL;
    } else {
        // Setup Google Contacts Service
        m_gContacts = new THGoogleContactsService;
        m_gContacts->setAuthToken(gAuth.auth());
    }
}

ContactsTest::~ContactsTest() {
    if (m_contact != NULL)
        delete m_contact;
    delete m_gContacts;
}

void ContactsTest::testInsert (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");

    // Setup Contact
    m_contact = new THGoogleContact;
    m_contact->setFullName("John Doe");

    THGoogleIm *im = new THGoogleIm;
    im->setRel(THGoogleIm::relWork());
    im->setAddress("john.doe@mac.com");
    im->setProtocol(THGoogleIm::protocolAim());
    m_contact->addImAddress(im);

    THGoogleEmail *email = new THGoogleEmail;
    email->setAddress("john.doe@th3sft.com");
    email->setRel(THGoogleEmail::relWork());
    m_contact->addEmailAddress(email);

    email = new THGoogleEmail;
    email->setAddress("john.doe@gmail.com");
    email->setRel(THGoogleEmail::relHome());
    m_contact->addEmailAddress(email);

    // Add Extended Property
    m_contact->addUserDefinedField("TestCode", "1289005");

    // Insert Contact
    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->insertContact(m_contact);
    l.exec();

    qDebug() << m_gContacts->errorString();
    qDebug() << m_contact->id();
    qDebug() << m_contact->etag();
}

void ContactsTest::testUpdate (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");
    QVERIFY2(m_contact != NULL, "Contact not Found");

    // Setup Contact
    THGoogleIm *im = new THGoogleIm;
    im->setRel(THGoogleIm::relWork());
    im->setAddress("john.doe@jabber.com");
    im->setProtocol(THGoogleIm::protocolJabber());
    m_contact->addImAddress(im);

    // Update Contact
    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->updateContact(m_contact);
    l.exec();

    qDebug() << m_gContacts->errorString();
    qDebug() << m_contact->id();
    qDebug() << m_contact->etag();
}

void ContactsTest::testUpdatePhoto (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");
    QVERIFY2(m_contact != NULL, "Contact not Found");

    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->addContactPhoto(m_contact, QImage("chess.jpg"));
    l.exec();
}

void ContactsTest::testInsertGroup (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");

    m_group = new THGoogleContactGroup;
    m_group->setTitle("Test Qt Group");

    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->insertGroup(m_group);
    l.exec();
}

void ContactsTest::testUpdateGroup (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");
    QVERIFY2(m_contact != NULL, "Contact not Found");
    QVERIFY2(m_group != NULL, "Group not Found");

    THGoogleGroupMembershipInfo *groupMembership = new THGoogleGroupMembershipInfo;
    groupMembership->setHref(m_group->id());
    m_contact->addGroupMembershipInfo(groupMembership);

    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->updateContact(m_contact);
    l.exec();
}

void ContactsTest::testAllContacts (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");

    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->retrieveAllContacts();
    l.exec();

    foreach (THGoogleContact *contact, m_gContacts->contactList())
        qDebug() << contact->fullName();
}

void ContactsTest::testAllGroups (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");

    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->retrieveAllGroups();
    l.exec();

    foreach (THGoogleContactGroup *group, m_gContacts->groupList())
        qDebug() << group->title();
}

#define ENABLE_DELETION
void ContactsTest::testDelete (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");
    QVERIFY2(m_contact != NULL, "Contact not Found");

#ifdef ENABLE_DELETION
    m_contact->setDeleted(true);

    // Update Contact
    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->deleteContact(m_contact);
    l.exec();
#endif
}

void ContactsTest::testDeleteGroup (void) {
    QVERIFY2(m_gContacts != NULL, "Authentication Failed");
    QVERIFY2(m_group != NULL, "Group not Found");

#ifdef ENABLE_DELETION
    m_group->setDeleted(true);

    // Update Contact
    QEventLoop l;
    connect(m_gContacts, SIGNAL(finished(bool)), &l, SLOT(quit()));
    m_gContacts->deleteGroup(m_group);
    l.exec();
#endif
}

QTEST_MAIN(ContactsTest)

#endif /* TEST_GOOGLE_CONTACTS */

