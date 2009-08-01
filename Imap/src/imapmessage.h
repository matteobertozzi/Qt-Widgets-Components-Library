#ifndef _IMAP_MESSAGE_H_
#define _IMAP_MESSAGE_H_

#include <QtGlobal>
#include <QDateTime>

typedef uint ImapMessageFlags;
typedef enum {
    None                   = 0,
    ImapMessageAnswered    = 1,
    ImapMessageDeleted     = 2,
    ImapMessageDraft       = 4,
    ImapMessageFlagged     = 8,
    ImapMessageRecent      = 16,
    ImapMessageSeen        = 32
} ImapMessageFlag;

class ImapMessageBodyPartPrivate;
class ImapMessageBodyPart {
    public:
        enum Encoding { 
            NoEncoding, UnknownEncoding, 
            Utf7Encoding, Utf8Encoding, 
            Base64Encoding, QuotedPrintableEncoding 
        };

    public:
        ImapMessageBodyPart(const QString& data);
        ~ImapMessageBodyPart();

        QByteArray data (void) const;
        void setData (const QByteArray& data);

        Encoding encoding (void) const;

        QString bodyPart (void) const;
        void setBodyPart (const QString& bodyPart);

        QString fileName (void) const;
        void setFileName (const QString& fileName);

        quint32 size (void) const;
        quint32 lines (void) const;

        bool isAttachment (void) const;
        QString disposition (void) const;

        QString charset (void) const;
        QString contentId (void) const;
        QString contentMd5 (void) const;
        QString contentType (void) const;
        QString contentLanguage (void) const;
        QString contentDescription (void) const;

    private:
        ImapMessageBodyPartPrivate *d;
};

class ImapAddress;
class ImapMessagePrivate;
class ImapMessage {
    public:
        ImapMessage();
        ~ImapMessage();
        
        // STATIC Methods
        static ImapMessageFlags parseFlags (const QString& textFlags);

        // Properties
        bool isNull (void) const;

        int id (void) const;
        void setId (int id);
        
        QString uid (void) const;
        void setUid (const QString& uid);
        
        QString reference (void) const;
        void setReference (const QString& reference);
        
        QString messageId (void) const;
        void setMessageId (const QString& id);               
        
        int size (void) const;
        void setSize (int size);
        
        QString subject (void) const;
        void setSubject (const QString& subject);

        QDateTime sent (void) const;
        void setSent (const QString& dateTime);
        void setSent (const QDateTime& dateTime);

        QDateTime received (void) const;
        void setReceived (const QString& dateTime);
        void setReceived (const QDateTime& dateTime);

        QString timeZone (void) const;
        void setTimeZone (const QString& timeZone);

        ImapMessageFlags flags (void) const;
        void setFlags (const QString& flags);
        void setFlags (ImapMessageFlags flags);

        ImapAddress fromAddress (void) const;
        ImapAddress senderAddress (void) const;
        QList<ImapAddress> toAddresses (void) const;
        QList<ImapAddress> ccAddresses (void) const;
        QList<ImapAddress> bccAddresses (void) const;
        QList<ImapAddress> replyAddresses (void) const;
        void setAddresses (const QString& addresses);

        bool hasHtmlPart (void) const;
        int htmlPartIndex (void) const;
        void setHtmlPartIndex (int bodyPart);
        ImapMessageBodyPart *htmlPart (void) const;

        bool hasTextPart (void) const;
        int textPartIndex (void) const;
        void setTextPartIndex (int bodyPart);
        ImapMessageBodyPart *textPart (void) const;

        int bodyPartCount (void) const;
        ImapMessageBodyPart *bodyPartAt (int index) const;
        QList<ImapMessageBodyPart *> bodyParts (void) const;
        void setBodyParts (const QList<ImapMessageBodyPart *> parts);

    private:
        ImapMessagePrivate *d;
};

#endif /* !_IMAP_MESSAGE_H_ */

