#include <QString>

#include "imapmailbox.h"

// ===========================================================================
//  PRIVATE Class
// ===========================================================================
class ImapMailboxPrivate {
    public:
        QList<ImapMessage *> messages;
        ImapMessageFlags flags;
        bool readWrite;
        QString name;
        int unseen;
        int recent;
        int exists;
};

// ===========================================================================
//  PUBLIC Constructors/Destructor
// ===========================================================================
ImapMailbox::ImapMailbox()
    : d(new ImapMailboxPrivate)
{
    d->unseen = d->exists = d->recent = 0;
    d->readWrite = false;
    d->flags = 0;
}

ImapMailbox::ImapMailbox (const QString& mailbox)
    : d(new ImapMailboxPrivate)
{
    d->unseen = d->exists = d->recent = 0;
    d->readWrite = false;
    d->name = mailbox;
    d->flags = 0;
}

ImapMailbox::~ImapMailbox() {
    clearMessages();
    delete d;
}

// ===========================================================================
//  PUBLIC Methods 
// ===========================================================================
void ImapMailbox::addMessage (ImapMessage *message) {
    d->messages.append(message);
}

void ImapMailbox::clearMessages (void) {
    QList<ImapMessage *>::iterator it = d->messages.begin();
    while (it != d->messages.end()) {
        delete *it;
        it = d->messages.erase(it);
    }
}

// ===========================================================================
//  PUBLIC Properties
// ===========================================================================
QString ImapMailbox::name (void) const {
    return(d->name);
}

void ImapMailbox::setName (const QString& name) {
    d->name = name;
}


int ImapMailbox::exists (void) const {
    return(d->exists);
}

void ImapMailbox::setExists (int exist) {
    d->exists = exist;
}


int ImapMailbox::recent (void) const {
    return(d->recent);
}

void ImapMailbox::setRecent (int recent) {
    d->recent = recent;
}

int ImapMailbox::unseen (void) const {
    return(d->unseen);
}

void ImapMailbox::setUnseen (int unseen) {
    d->unseen = unseen;
}

bool ImapMailbox::isReadWrite (void) const {
    return(d->readWrite);
}

void ImapMailbox::setReadWrite (bool readWrite) {
    d->readWrite = readWrite;
}


ImapMessageFlags ImapMailbox::flags (void) const {
    return(d->flags);
}

void ImapMailbox::setFlags (const QString& flags) {
    d->flags = ImapMessage::parseFlags(flags);
}

void ImapMailbox::setFlags (ImapMessageFlags flags) {
    d->flags = flags;
}

ImapMessage *ImapMailbox::takeAt (int index) {
    return(d->messages.takeAt(index));
}

ImapMessage *ImapMailbox::at (int index) const {
    return(d->messages[index]);
}

QList<ImapMessage *> ImapMailbox::messages (void) const {
    return(d->messages);
}

ImapMessage *ImapMailbox::findById (int messageId) const {
    foreach (ImapMessage *message, d->messages) {
        if (message->id() == messageId)
            return(message);
    }
    return(NULL);
}

