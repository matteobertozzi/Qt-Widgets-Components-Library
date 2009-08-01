#ifndef _IMAP_MAILBOX_H_
#define _IMAP_MAILBOX_H_

#include <QList>
#include "imapmessage.h"

class ImapMailboxPrivate;
class ImapMailbox {
    public:
        ImapMailbox();
        ImapMailbox (const QString& mailbox);
        ~ImapMailbox();

        // Methods
        void addMessage (ImapMessage *message);
        void clearMessages (void);
        
        // Properties
        QString name (void) const;
        void setName (const QString& name);
        
        int exists (void) const;
        void setExists (int exist);
        
        int recent (void) const;
        void setRecent (int recent);
        
        int unseen (void) const;
        void setUnseen (int unseen);

        bool isReadWrite (void) const;
        void setReadWrite (bool readWrite);
                
        ImapMessageFlags flags (void) const;
        void setFlags (const QString& flags);
        void setFlags (ImapMessageFlags flags);
        
        ImapMessage *takeAt (int index);
        ImapMessage *at (int index) const;
        QList<ImapMessage *> messages (void) const;
        ImapMessage *findById (int messageId) const;

    private:
        ImapMailboxPrivate *d;
};

#endif /* !_IMAP_MAILBOX_H_ */

