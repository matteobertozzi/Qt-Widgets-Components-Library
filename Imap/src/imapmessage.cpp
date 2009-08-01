#include <QStringList>
#include <QSharedData>

#include "imapmessage.h"
#include "imapaddress.h"
#include "imap.h"

// ===========================================================================
//  PRIVATE Functions
// ===========================================================================
static QList<ImapAddress> _createAddressList (const QString& addresses) {
    QList<ImapAddress> list;

    QRegExp rx("(NIL|\\\".+\\\")\\s+(NIL|\\\".+\\\")\\s+(NIL|\\\".+\\\")\\s+(NIL|\\\".+\\\")\\)");

    QStringList addressList = addresses.split('(', QString::SkipEmptyParts);
    foreach (QString address, addressList) {
        if (rx.indexIn(address) == -1)
            continue;

        QString hostName = rx.cap(4);
        QString mailboxName = rx.cap(3);
        if (mailboxName == "NIL" && hostName == "NIL")
            continue;

        QString email;
        if (mailboxName == "NIL")
            email = QString("unknown@%1").arg(hostName);
        else if (hostName == "NIL")
            email = QString("%1@unknown").arg(mailboxName);
        else
            email = QString("%1@%2").arg(mailboxName).arg(hostName);

        // Create new ImapAddress
        ImapAddress imapAddress(email.replace("\"", ""));

        // Setup Personal Name (Display Name)
        QString personalName = rx.cap(1);
        if (personalName != "NIL") {
            personalName.replace("\"", "");
            imapAddress.setDisplayName(Imap::decode(personalName));
        }

        // Setup SMTP Domain
        QString smtpDomain = rx.cap(2);
        if (smtpDomain != "NIL") {
            smtpDomain.replace("\"", "");
            imapAddress.setSmtpDomain(Imap::decode(smtpDomain));
        }

        list.append(imapAddress);
    }

    return(list);
}

// ===========================================================================
//  PRIVATE Class
// ===========================================================================
class ImapMessageBodyPartPrivate {
    public:
        ImapMessageBodyPart::Encoding encoding;
        QString contentDescription;
        QString disposition;
        QString contentType;
        QString contentId;
        QString language;
        QString fileName;
        QString charset;
        QString bodyPart;

        bool isAttachment;
        QByteArray data;
        quint32 lines;
        quint32 size;
        QString md5;

    public:
        QString parseNil (const QString& text) const;
        QString parseNilAndQuote (const QString& text) const;

        QString parseCharset (const QString& text) const;
        QString parseFileName (const QString& text) const;
        ImapMessageBodyPart::Encoding parseEncoding (const QString& text) const;
};

QString ImapMessageBodyPartPrivate::parseNil (const QString& text) const {
    return(text == "NIL" ? QString() : text);
}

QString ImapMessageBodyPartPrivate::parseNilAndQuote (const QString& text) const {
    return(text == "NIL" ? QString() : text.mid(1, text.size() - 2));
}

QString ImapMessageBodyPartPrivate::parseCharset (const QString& text) const {
    QRegExp rx("\"CHARSET\"\\s+\"([^\"]*)\"");
    if (rx.indexIn(text.toUpper()) != -1)
        return(rx.cap(1));
    return(QString());
}

QString ImapMessageBodyPartPrivate::parseFileName (const QString& text) const {
    QRegExp rx("\"NAME\"\\s+\"([^\"]*)\"");
    if (rx.indexIn(text.toUpper()) != -1)
        return(rx.cap(1));
    return(QString());
}

// None, Unknown, Utf7, Utf8, Base64, QuotedPrintable
ImapMessageBodyPart::Encoding ImapMessageBodyPartPrivate::parseEncoding (
    const QString& text) const
{
    QString data = parseNilAndQuote(text).toUpper();
    if (data.isEmpty())
        return(ImapMessageBodyPart::UnknownEncoding);

    switch (data[0].toAscii()) {
        case '7':
            return(ImapMessageBodyPart::Utf7Encoding);
        case '8':
            if (data.indexOf("BIT", 1) == 1)
                return(ImapMessageBodyPart::UnknownEncoding);
            return(ImapMessageBodyPart::Utf8Encoding);
        case 'B':
            if (data.startsWith("BASE64"))
                return(ImapMessageBodyPart::Base64Encoding);
            if (data.startsWith("BINARY"))
                return(ImapMessageBodyPart::NoEncoding);
            return(ImapMessageBodyPart::UnknownEncoding);
        case 'Q':
            if (data.startsWith("QUOTED-PRINTABLE"))
                return(ImapMessageBodyPart::QuotedPrintableEncoding);
            return(ImapMessageBodyPart::UnknownEncoding);
    }

    return(ImapMessageBodyPart::UnknownEncoding);
}

ImapMessageBodyPart::ImapMessageBodyPart(const QString& data)
    : d(new ImapMessageBodyPartPrivate)
{
    QRegExp rxNonAttachment("^\\((\"[^\"]*\"|NIL)\\s+(\"[^\"]*\"|NIL)"
            "\\s+(\\(.*\\)|NIL)\\s+(\"[^\"]*\"|NIL)\\s+(\"[^\"]*\"|NIL)"
            "\\s+(\"[^\"]*\"|NIL)\\s+(\\d+|NIL)\\s+(\\d+|NIL)"
            "\\s+(\"[^\"]*\"|NIL)\\s+(\\(.*\\)|NIL)\\s+(\"[^\"]*\"|NIL).*\\)");

    QRegExp rxAttachment("^\\((\"[^\"]*\"|NIL)\\s+(\"[^\"]*\"|NIL)"
            "\\s+\\((.*|NIL)\\)\\s+(\"[^\"]*\"|NIL)\\s+(\"[^\"]*\"|NIL)"
            "\\s+(\"[^\"]*\"|NIL)\\s+(\\d+|NIL)\\s+(.*|NIL)"
            "\\s+\\((\"[^\"]*\"|NIL)\\s+\\((.*|NIL)\\)\\)"
            "\\s+(\"[^\"]*\"|NIL).*\\)");

    if (rxNonAttachment.indexIn(data) != -1) {
        d->contentType = QString("%1/%2");
        d->contentType = d->contentType.arg(d->parseNilAndQuote(rxNonAttachment.cap(1)));
        d->contentType = d->contentType.arg(d->parseNilAndQuote(rxNonAttachment.cap(2)));

        d->charset = d->parseCharset(rxNonAttachment.cap(3));
        d->fileName = d->parseFileName(rxNonAttachment.cap(3));
        d->isAttachment = !d->fileName.isEmpty();

        d->contentId = d->parseNilAndQuote(rxNonAttachment.cap(4));
        d->contentDescription = d->parseNilAndQuote(rxNonAttachment.cap(5));
        d->encoding = d->parseEncoding(rxNonAttachment.cap(6));
        d->size = rxNonAttachment.cap(7).toInt();
        d->lines = rxNonAttachment.cap(8).toInt();
        d->md5 = d->parseNilAndQuote(rxNonAttachment.cap(9));
        d->disposition = d->parseNil(rxNonAttachment.cap(10));
        d->language = d->parseNilAndQuote(rxNonAttachment.cap(11));
    } else if (rxAttachment.indexIn(data) != -1) {
        d->isAttachment = true;

        d->contentType = QString("%1/%2");
        d->contentType = d->contentType.arg(d->parseNilAndQuote(rxAttachment.cap(1)));
        d->contentType = d->contentType.arg(d->parseNilAndQuote(rxAttachment.cap(2)));

        d->fileName = d->parseFileName(rxAttachment.cap(3));
        d->contentId = d->parseNilAndQuote(rxAttachment.cap(4));
        d->contentDescription = d->parseNilAndQuote(rxAttachment.cap(5));
        d->encoding = d->parseEncoding(rxAttachment.cap(6));
        d->size = rxAttachment.cap(7).toInt();
        d->lines = rxAttachment.cap(8).toInt();
        d->disposition = d->parseNil(rxAttachment.cap(9));
        d->language = d->parseNilAndQuote(rxAttachment.cap(11));
    } else {
        qDebug("-----------------> TODO: NO MATCH BODY PART");
    }
}

ImapMessageBodyPart::~ImapMessageBodyPart() {
    delete d;
}

quint32 ImapMessageBodyPart::size (void) const {
    return(d->size);
}

quint32 ImapMessageBodyPart::lines (void) const {
    return(d->lines);
}

QByteArray ImapMessageBodyPart::data (void) const {
    return(d->data);
}

void ImapMessageBodyPart::setData (const QByteArray& data) {
    d->data = data;
}

ImapMessageBodyPart::Encoding ImapMessageBodyPart::encoding (void) const {
    return(d->encoding);
}

bool ImapMessageBodyPart::isAttachment (void) const {
    return(d->isAttachment);
}

QString ImapMessageBodyPart::bodyPart (void) const {
    return(d->bodyPart);
}

void ImapMessageBodyPart::setBodyPart (const QString& bodyPart) {
    d->bodyPart = bodyPart;
}


QString ImapMessageBodyPart::fileName (void) const {
    return(d->fileName);
}

void ImapMessageBodyPart::setFileName (const QString& fileName) {
    d->fileName = fileName;
}

QString ImapMessageBodyPart::charset (void) const {
    return(d->charset);
}

QString ImapMessageBodyPart::disposition (void) const {
    return(d->disposition);
}

QString ImapMessageBodyPart::contentId (void) const {
    return(d->contentId);
}

QString ImapMessageBodyPart::contentMd5 (void) const {
    return(d->md5);
}

QString ImapMessageBodyPart::contentType (void) const {
    return(d->contentType);
}

QString ImapMessageBodyPart::contentLanguage (void) const {
    return(d->language);
}

QString ImapMessageBodyPart::contentDescription (void) const {
    return(d->contentDescription);
}

// ===========================================================================
//  PRIVATE Class (Message)
// ===========================================================================
class ImapMessagePrivate {
    public:
        QList<ImapMessageBodyPart *> bodyParts;

        QList<ImapAddress> replyAddresses;
        QList<ImapAddress> bccAddresses;
        QList<ImapAddress> ccAddresses;
        QList<ImapAddress> toAddresses;
        ImapAddress senderAddress;
        ImapAddress fromAddress;

        ImapMessageFlags flags;
        QDateTime received;
        QString reference;
        QString messageId;
        int htmlPartIndex;
        int textPartIndex;
        QString timeZone;
        QString subject;
        QDateTime sent;
        QString uid;
        int size;
        int id;
};

// ===========================================================================
//  PUBLIC Constructors/Destructor
// ===========================================================================
ImapMessage::ImapMessage()
    : d(new ImapMessagePrivate)
{
    d->id = -1;
    d->flags = 0;
    d->htmlPartIndex = d->textPartIndex = -1;
}

ImapMessage::~ImapMessage() {
    QList<ImapMessageBodyPart *>::iterator it = d->bodyParts.begin();
    while (it != d->bodyParts.end()) {
        delete *it;
        it = d->bodyParts.erase(it);
    }
    delete d;
}

// ===========================================================================
//  PUBLIC STATIC Methods
// ===========================================================================
ImapMessageFlags ImapMessage::parseFlags (const QString& textFlags) {
    QStringList flagList = textFlags.split(" ", QString::SkipEmptyParts);    
    ImapMessageFlags flags = 0;
    
    foreach (QString flag, flagList) {
        flag = flag.trimmed();
        if (flag == "\\Answered")
            flags |= ImapMessageAnswered;
        else if (flag == "\\Deleted")
            flags |= ImapMessageDeleted;
        else if (flag == "\\Draft") 
            flags |= ImapMessageDraft;
        else if (flag == "\\Flagged") 
            flags |= ImapMessageFlagged;
        else if (flag == "\\Recent") 
            flags |= ImapMessageRecent;
        else if (flag == "\\Seen")
            flags |= ImapMessageSeen;
    }

    return(flags);
}

// ===========================================================================
//  PUBLIC Properties
// ===========================================================================
bool ImapMessage::isNull (void) const {
    return(d->id == -1);
}

int ImapMessage::id (void) const {
    return(d->id);
}

void ImapMessage::setId (int id) {
    d->id = id;
}


QString ImapMessage::uid (void) const {
    return(d->uid);
}

void ImapMessage::setUid (const QString& uid) {
    d->uid = uid;
}


QString ImapMessage::reference (void) const {
    return(d->reference);
}

void ImapMessage::setReference (const QString& reference) {
    d->reference = reference;
}


QString ImapMessage::messageId (void) const {
    return(d->messageId);
}

void ImapMessage::setMessageId (const QString& id) {
    d->messageId = id;
}


int ImapMessage::size (void) const {
    return(d->size);
}

void ImapMessage::setSize (int size) {
    d->size = size;
}


QString ImapMessage::subject (void) const {
    return(d->subject);
}

void ImapMessage::setSubject (const QString& subject) {
    d->subject = subject;
}

QDateTime ImapMessage::sent (void) const {
    return(d->sent);
}

void ImapMessage::setSent (const QString& dateTime) {
    d->sent = QDateTime::fromString(dateTime, "dd-MMM-yyyy HH:mm:ss +0000");
}

void ImapMessage::setSent (const QDateTime& dateTime) {
    d->sent = dateTime;
}

QDateTime ImapMessage::received (void) const {
    return(d->received);
}

void ImapMessage::setReceived (const QString& dateTime) {
    d->received = QDateTime::fromString(dateTime, "dd-MMM-yyyy HH:mm:ss +0000");
}

void ImapMessage::setReceived (const QDateTime& dateTime) {
    d->received = dateTime;
}

QString ImapMessage::timeZone (void) const {
    return(d->timeZone);
}

void ImapMessage::setTimeZone (const QString& timeZone) {
    d->timeZone = timeZone;
}

ImapMessageFlags ImapMessage::flags (void) const {
    return(d->flags);
}

void ImapMessage::setFlags (const QString& flags) {
    d->flags = parseFlags(flags);
}

void ImapMessage::setFlags (ImapMessageFlags flags) {
    d->flags = flags;
}

ImapAddress ImapMessage::fromAddress (void) const {
    return(d->fromAddress);
}

ImapAddress ImapMessage::senderAddress (void) const {
    return(d->senderAddress);
}

QList<ImapAddress> ImapMessage::toAddresses (void) const {
    return(d->toAddresses);
}

QList<ImapAddress> ImapMessage::ccAddresses (void) const {
    return(d->ccAddresses);
}

QList<ImapAddress> ImapMessage::bccAddresses (void) const {
    return(d->bccAddresses);
}

QList<ImapAddress> ImapMessage::replyAddresses (void) const {
    return(d->replyAddresses);
}

void ImapMessage::setAddresses (const QString& fullAddresses) {
    QString addresses = fullAddresses;
    QString addressList;
    int index;

    // From Address
    if (addresses.startsWith("NIL")) {
        addresses = addresses.remove(0, 3).trimmed();
    } else {
        index = addresses.indexOf("))") + 2;
        addressList = addresses.mid(0, index);
        addresses = addresses.mid(addressList.size()).trimmed();
        d->fromAddress = _createAddressList(addressList).takeFirst();
    }

    // Sender Address
    if (addresses.startsWith("NIL")) {
        addresses = addresses.remove(0, 3).trimmed();
    } else {
        index = addresses.indexOf("))") + 2;
        addressList = addresses.mid(0, index);
        addresses = addresses.mid(addressList.size()).trimmed();
        d->senderAddress = _createAddressList(addressList).takeFirst();
    }

    // ReplyTo Address
    if (addresses.startsWith("NIL")) {
        addresses = addresses.remove(0, 3).trimmed();
    } else {
        index = addresses.indexOf("))") + 2;
        addressList = addresses.mid(0, index);
        addresses = addresses.mid(addressList.size()).trimmed();
        d->replyAddresses = _createAddressList(addressList);
    }

    // To Address
    if (addresses.startsWith("NIL")) {
        addresses = addresses.remove(0, 3).trimmed();
    } else {
        index = addresses.indexOf("))") + 2;
        addressList = addresses.mid(0, index);
        addresses = addresses.mid(addressList.size()).trimmed();
        d->toAddresses = _createAddressList(addressList);
    }

    // CC Address
    if (addresses.startsWith("NIL")) {
        addresses = addresses.remove(0, 3).trimmed();
    } else {
        index = addresses.indexOf("))") + 2;
        addressList = addresses.mid(0, index);
        addresses = addresses.mid(addressList.size()).trimmed();
        d->ccAddresses = _createAddressList(addressList);
    }

    // BCC Address
    if (addresses.startsWith("NIL")) {
        addresses = addresses.remove(0, 3).trimmed();
    } else {
        index = addresses.indexOf("))") + 2;
        addressList = addresses.mid(0, index);
        addresses = addresses.mid(addressList.size()).trimmed();
        d->bccAddresses = _createAddressList(addressList);
    }
}

bool ImapMessage::hasHtmlPart (void) const {
    return(d->htmlPartIndex != -1);
}

int ImapMessage::htmlPartIndex (void) const {
    return(d->htmlPartIndex);
}

void ImapMessage::setHtmlPartIndex (int bodyPart) {
    d->htmlPartIndex = bodyPart;
}

ImapMessageBodyPart *ImapMessage::htmlPart (void) const {
    if (d->htmlPartIndex < 0 || d->htmlPartIndex >= d->bodyParts.size())
        return(NULL);
    return(d->bodyParts[d->htmlPartIndex]);
}

bool ImapMessage::hasTextPart (void) const {
    return(d->textPartIndex != -1);
}

int ImapMessage::textPartIndex (void) const {
    return(d->textPartIndex);
}

void ImapMessage::setTextPartIndex (int bodyPart) {
    d->textPartIndex = bodyPart;
}

ImapMessageBodyPart *ImapMessage::textPart (void) const {
    if (d->textPartIndex < 0 || d->textPartIndex >= d->bodyParts.size())
        return(NULL);
    return(d->bodyParts[d->textPartIndex]);
}

int ImapMessage::bodyPartCount (void) const {
    return(d->bodyParts.size());
}

ImapMessageBodyPart *ImapMessage::bodyPartAt (int index) const {
    return(d->bodyParts[index]);
}

QList<ImapMessageBodyPart *> ImapMessage::bodyParts (void) const {
    return(d->bodyParts);
}

void ImapMessage::setBodyParts (const QList<ImapMessageBodyPart *> parts) {
    d->bodyParts = parts;
}


