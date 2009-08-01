#ifndef _IMAP_ADDRESS_H_
#define _IMAP_ADDRESS_H_

#include <QSharedDataPointer>

class ImapAddressData : public QSharedData {
    public:
        QString displayName;
        QString smtpDomain;
        QString address;
};

class ImapAddress {
    public:
        ImapAddress();
        ImapAddress(const QString& address);
        ImapAddress(const QString& address, const QString& displayName);
        ImapAddress(const ImapAddress& address);
        ~ImapAddress();

        bool isNull (void) const;
        
        QString address (void) const;
        void setAddress (const QString& address);

        bool hasDisplayName (void) const;
        QString displayName (void) const;
        void setDisplayName (const QString& displayName);
        
        QString smtpDomain (void) const;
        void setSmtpDomain (const QString& smtpDomain);

        QString toString (void) const;

    private:
        QSharedDataPointer<ImapAddressData> d;
};

#endif /* !_IMAP_ADDRESS_H_ */

