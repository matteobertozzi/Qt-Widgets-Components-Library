#include <QCoreApplication>
#include <QStringList>
#include <QtGlobal>
#include <QDebug>

#include "imapmailbox.h"
#include "imapmessage.h"
#include "imapaddress.h"
#include "imap.h"

#define IMAP_HOST           "imap.gmail.com"
#define IMAP_PORT           (993)
#define IMAP_USE_SSL        (true)

#define IMAP_USERNAME       "USERNAME@gmail.com"
#define IMAP_PASSWORD       "PASSWORD"
#define IMAP_LOGIN_TYPE     (Imap::LoginPlain)

#define IMAP_MAIN_ABORT(func, message)     \
    { qDebug() << func << message; return(1); }

int main (int argc, char **argv) {
    QCoreApplication app(argc, argv);

    Imap imap;
    if (!imap.connectToHost(IMAP_HOST, IMAP_PORT, IMAP_USE_SSL))
        IMAP_MAIN_ABORT("connectToHost()", imap.errorString());

    if (!imap.login(IMAP_USERNAME, IMAP_PASSWORD, IMAP_LOGIN_TYPE))
        IMAP_MAIN_ABORT("login()", imap.errorString());

    qDebug() << imap.list();

    ImapMailbox *mailbox = imap.select("INBOX");
    if (mailbox == NULL)
        IMAP_MAIN_ABORT("select()", imap.errorString());

    qDebug() << "INBOX";
    qDebug() << " - Exists:" << mailbox->exists();
    qDebug() << " - Recent:" << mailbox->recent();
    qDebug() << " - Unseen:" << mailbox->unseen();
    qDebug() << " - Is RW:" << mailbox->isReadWrite();
    qDebug() << " - Flags:" << mailbox->flags();

    //QList<int> messageList = imap.searchText("\"Test mail attachment binary\"");
    QList<int> messageList = imap.searchRecentUnseen();
    qDebug() << messageList;

    if (imap.fetch(mailbox, messageList) == NULL)
        IMAP_MAIN_ABORT("fetch()", imap.errorString());

    foreach (int msgId, messageList) {
        ImapMessage *message = mailbox->findById(msgId);
        if (message == NULL) {
            qDebug() << "Message" << msgId << "Not Found.";
            continue;
        }

        if (!imap.fetchBodyStructure(message))
            IMAP_MAIN_ABORT("fetchBodyStructure()", imap.errorString());

        qDebug() << "ID" << message->id() 
                 << "UID" << message->uid() 
                 << "REF" << message->reference();
        qDebug() << "FROM" << message->fromAddress().toString();
        foreach (ImapAddress address, message->toAddresses())
            qDebug() << " - TO" << address.toString();
        foreach (ImapAddress address, message->ccAddresses())
            qDebug() << " - CC" << address.toString();
        foreach (ImapAddress address, message->bccAddresses())
            qDebug() << " - BCC" << address.toString();
        qDebug() << "SUBJECT" << message->subject();

        for (int i = 0; i < message->bodyPartCount(); ++i) {
            ImapMessageBodyPart *bodyPart = message->bodyPartAt(i);

            qDebug() << bodyPart->isAttachment() << bodyPart->bodyPart() 
                     << bodyPart->fileName() << bodyPart->encoding() << bodyPart->contentType();

            if (!imap.fetchBodyPart(message, i))
                IMAP_MAIN_ABORT("fetchBodyPart()", imap.errorString());

            //imap.setSeen(message->id(), true);
            qDebug() << bodyPart->data();
            qDebug() << "=======================================================";
        }
    }

    // Detroy Mailbox.
    delete mailbox;

    if (!imap.logout())
        IMAP_MAIN_ABORT("logout()", imap.errorString());

    if (!imap.disconnectFromHost())
        IMAP_MAIN_ABORT("disconnectFromHost()", imap.errorString());

    return(0);
}

