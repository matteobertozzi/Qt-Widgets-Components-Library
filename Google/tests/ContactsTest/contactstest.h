#ifdef TEST_GOOGLE_CONTACTS
#ifndef _CONTACTS_TEST_H_
#define _CONTACTS_TEST_H_

#include <QObject>
class THGoogleContactsService;
class THGoogleContactGroup;
class THGoogleContact;

class ContactsTest : public QObject {
    Q_OBJECT

    public:
        ContactsTest (QObject *parent = 0);
        ~ContactsTest();

    private slots:
        void testInsert (void);
        void testUpdate (void);
        void testUpdatePhoto (void);

        void testInsertGroup (void);
        void testUpdateGroup (void);

        void testAllContacts (void);    
        void testAllGroups (void);

        void testDelete (void);
        void testDeleteGroup (void);

    private:
        THGoogleContactsService *m_gContacts;
        THGoogleContactGroup *m_group;
        THGoogleContact *m_contact;
};

#endif /* !_CONTACTS_TEST_H_ */
#endif /* !TEST_GOOGLE_CONTACTS */

