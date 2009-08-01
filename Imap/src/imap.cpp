#include <QCryptographicHash>
#include <QStringList>
#include <QRegExp>

#ifndef QT_NO_OPENSSL
    #include <QSslSocket>
#else
    #include <QTcpSocket>
#endif

#include "imapmessage.h"
#include "imapmailbox.h"
#include "imap.h"

#ifdef IMAP_DEBUG
    #include <QDebug>
#endif

#ifdef Q_WS_WIN
    #define IMAP_MESSAGE_BODY_NEWLINE        "\r\n"
#else
    #define IMAP_MESSAGE_BODY_NEWLINE        "\n"
#endif

#define IMAP_TAG        "THIMAP"

// ===========================================================================
//  PRIVATE Functions
// ===========================================================================
static ImapMessage *_imapParseMessage (const QString& responseText) {
    if (!responseText.startsWith('*'))
        return(NULL);

    QString response = responseText;

    QRegExp regexId("\\* (\\d*)");    
    QRegExp regexMessageId("\"<([^>]+)>\"\\)\\)");
    QRegExp regexFlags("\\(FLAGS \\(([^\\)]*)\\)");
    QRegExp regexInternalDate("INTERNALDATE \"([^\"]+)\"");
    QRegExp regexSize("RFC822.SIZE (\\d+)");
    QRegExp regexEnvelope("ENVELOPE");   

    ImapMessage *message = new ImapMessage;
    if (regexId.indexIn(response) != -1)
        message->setId(regexId.cap(1).toInt());
    if (regexFlags.indexIn(response) != -1)
        message->setFlags(regexFlags.cap(1));
    if (regexInternalDate.indexIn(response) != -1)
        message->setReceived(regexInternalDate.cap(1));
    if (regexSize.indexIn(response) != -1)
        message->setSize(regexSize.cap(1).toInt());
    
    if (regexEnvelope.indexIn(response) != -1) {
        int rmSize = regexEnvelope.pos() + regexEnvelope.matchedLength();
        response = response.remove(0, rmSize);
    }
 
    QRegExp regexSent("\(\"(?:\\w{3}\\, )?([^\"]+)\"");
    if (regexSent.indexIn(response) != -1) {
        QRegExp subRegex("([\\-\\+]\\d{4}.*|NIL.*)");
        subRegex.indexIn(regexSent.cap(1));

        message->setSent(regexSent.cap(1).remove(subRegex.pos()));
        message->setTimeZone(subRegex.cap(1));
        response = response.remove(0, regexSent.pos() + regexSent.matchedLength());
    }
 
    QString token = "((";
    int tokenOffset;        
    if (response.contains(token)) {
        tokenOffset = 0;
    } else {
        token = "\" NIL";
        tokenOffset = 2;
    }
    
    QRegExp regexSubject(" \"(.*)\"");
    QString subject = response.mid(0, response.indexOf(token) + tokenOffset).trimmed();
    if (subject == "NIL")
        subject.clear();
    else if (regexSubject.indexIn(subject) != -1)
        subject = regexSubject.cap(1);
    message->setSubject(Imap::decode(subject.toLatin1()));

    if (response.contains("((")) {
        response = response.remove(0, response.indexOf("(("));
    } else {
        if (message->subject().isEmpty())
            message->setSubject(response);
    }
    
    if (regexMessageId.indexIn(response)) {
        message->setMessageId(regexMessageId.cap(1));
        response = response.remove(regexMessageId.pos()).trimmed();
    }
    
    if (response.endsWith("NIL")) {
        response = response.remove(response.size() - 3);
    } else {
        QRegExp regexReference("\"<([^>]+)>\"");
        if (regexReference.indexIn(response) != -1)
        message->setReference(regexReference.cap(1));
    }
    
    message->setAddresses(response.trimmed());
    return(message);
}

// ===========================================================================
//  PRIVATE Class
// ===========================================================================
class ImapPrivate {
    public:
        QString responseErrorMsg;
        QTcpSocket *socket;

    public:
        bool connectToHost (const QString& host, quint16 port, bool useSsl);

        bool setFlag (int uid, const QString& flag, bool append);

    public:
        QByteArray readLine (bool *ok = NULL);

        bool isMultiline   (const QString& data) const;
        bool isResponseOk  (const QByteArray& response) const;
        bool isResponseEnd (const QString& response) const;

        bool sendDataLine (const QString& data);
        bool sendCommand  (const QString& command, 
                           const QStringList& args = QStringList());

    public:
        QByteArray hmacMd5 (const QString& username,
                            const QString& password,
                            const QString& serverResponse);

        ImapMailbox *parseMailbox (const QString& mailboxName);
        ImapMailbox *parseMessages (ImapMailbox *mailbox);

        QByteArray parseBodyPart (ImapMessageBodyPart::Encoding encoding);
        QList<ImapMessageBodyPart *> bodyPartSplit (const QByteArray& response);

        QString rfcDate (const QDateTime& date) const;

    private:
        QString buildId (void) const;

    private:
        QString m_lastId;
};

bool ImapPrivate::connectToHost (const QString& host, quint16 port, bool useSsl)
{
#ifndef QT_NO_OPENSSL
    if (useSsl)
        socket = new QSslSocket;       
    else
        socket = new QTcpSocket;
#else
    Q_UNUSED(useSsl)
    socket = new QTcpSocket;
#endif

#ifndef QT_NO_OPENSSL
    if (useSsl) {
        QSslSocket *sslSocket = static_cast<QSslSocket *>(socket);
        sslSocket->connectToHostEncrypted(host, port);
        return(sslSocket->waitForEncrypted());
    } else {
        socket->connectToHost(host, port);
        return(socket->waitForConnected());
    }
#else
    socket->connectToHost(host, port);
    return(socket->waitForConnected());
#endif
}

bool ImapPrivate::setFlag (int uid, const QString& flag, bool append) {
    QString method = (append ? "+flags" : "-flags");

    QString command = "UID STORE %1 %2 (%3)";
    if (!sendCommand(command.arg(uid).arg(method).arg(flag)))
        return(false);

    QByteArray response = readLine();
    if (!response.startsWith('*'))
        return(false);

    bool ok = false;
    readLine(&ok);
    return(ok);
}

bool ImapPrivate::sendDataLine (const QString& data) {
#ifdef IMAP_DEBUG
    qDebug() << "sendDataLine()" << data;
#endif

    responseErrorMsg.clear();
    socket->write(QString("%1\r\n").arg(data).toLatin1());
    return(socket->waitForBytesWritten());
}

bool ImapPrivate::sendCommand (const QString& command, const QStringList& args)
{
    m_lastId = buildId();

    QString fullCommand = QString("%1 %2").arg(m_lastId).arg(command);
    foreach (QString arg, args)
        fullCommand = fullCommand.arg(arg);

    return(sendDataLine(fullCommand));
}

QByteArray ImapPrivate::readLine (bool *ok) {
    quint8 attempts = 0;
    while (!socket->canReadLine() && attempts < 2) {
        if (!socket->waitForReadyRead())
            attempts++;
    }

    if (attempts >= 2) {
        if (ok != NULL) *ok = false;
        return(QByteArray());
    }

    if (ok != NULL) *ok = true;
#ifdef IMAP_DEBUG
    QByteArray response = socket->readLine();
    qDebug() << "readLine()" << response;
    return(response);
#else
    return(socket->readLine());
#endif
}

bool ImapPrivate::isMultiline (const QString& data) const {
    return(QRegExp("^.*\\{\\d+\\}$").exactMatch(data.trimmed()));
}

bool ImapPrivate::isResponseOk (const QByteArray& response) const {
    if (response.startsWith("* OK"))
        return(true);

    if (response.startsWith(m_lastId.toLatin1() + " OK"))
        return(true);

    return(false);
}

bool ImapPrivate::isResponseEnd (const QString& response) const {
    QString trimmed = response.trimmed().toUpper();

    if (trimmed.contains(m_lastId + " OK")) {
        trimmed = trimmed.replace(".", "").replace("(", "").replace(")", "");
        return(trimmed.endsWith("SUCCESS") || 
               trimmed.endsWith("COMPLETED"));
    } else if (trimmed.contains(m_lastId + " BAD")) {
        return(true);
    } else if (trimmed.contains(m_lastId + " NO")) {
        return(true);
    }

    return(false);
}


QString ImapPrivate::buildId (void) const {
    QString id;
    do {
        id = QString("%1%2").arg(IMAP_TAG).arg(qrand());
    } while (m_lastId == id);
    return(id);
}

QByteArray ImapPrivate::hmacMd5 (const QString& username,
                                 const QString& password,
                                 const QString& serverResponse)
{
    QByteArray passwordBytes = password.toLatin1();
    QByteArray ipad(64, 0);
    QByteArray opad(64, 0);
    
    for (int i = 0; i < 64; ++i) {
        if (i < passwordBytes.size()) {
            ipad[i] = (quint8)(0x36 ^ passwordBytes[i]);
            opad[i] = (quint8)(0x5c ^ passwordBytes[i]);
        } else {
            ipad[i] = 0x36;
            opad[i] = 0x5c;
        }
    }

    QByteArray serverResponseBytes = serverResponse.toLatin1();
    while (serverResponseBytes[0] == '+' || serverResponseBytes[0] == ' ')
        serverResponseBytes.remove(0, 1);
        
    QCryptographicHash md5hash(QCryptographicHash::Md5);
    md5hash.addData(ipad);
    md5hash.addData(serverResponseBytes);
    QByteArray resultIpad = md5hash.result();
    
    md5hash.reset();
    md5hash.addData(opad);
    md5hash.addData(resultIpad);
    QByteArray resultOpad = md5hash.result();
    
    QByteArray response = username.toLatin1() + ' ' + resultOpad.toHex();
    return(response.toBase64());
}

ImapMailbox *ImapPrivate::parseMailbox (const QString& mailboxName) {
    ImapMailbox *mailbox = NULL;       
    
    QByteArray response = readLine();
    if (response.startsWith('*')) {
        QRegExp regexUnseen("\\[UNSEEN (\\d+)\\]");
        QRegExp regexExists("(\\d+) EXISTS");
        QRegExp regexRecent("(\\d+) RECENT");
        QRegExp regexFlags(" FLAGS \\((.*)\\)");
        
        mailbox = new ImapMailbox(mailboxName);
        do {
            if (regexExists.indexIn(response) != -1) {
                mailbox->setExists(regexExists.cap(1).toInt());
            } else if (regexRecent.indexIn(response) != -1) {
                mailbox->setRecent(regexRecent.cap(1).toInt());
            } else if (regexFlags.indexIn(response) != -1) {
                mailbox->setFlags(regexFlags.cap(1));
            } else if (regexUnseen.indexIn(response) != -1) {
                mailbox->setUnseen(regexUnseen.cap(1).toInt());
            }
            
            response = readLine();
        } while (response.startsWith('*'));
        
        response = response.toUpper();
        if (isResponseOk(response) &&
           (response.contains("READ/WRITE") || response.contains("READ-WRITE")))
        {
            mailbox->setReadWrite(true);
        }
    }
    
    return(mailbox);
}

ImapMailbox *ImapPrivate::parseMessages (ImapMailbox *mailbox) {
    QByteArray response;

    while (true) {
        // Read Message
        response.clear();       
        do {
            if (response.size() > 0) {
                response.remove(response.lastIndexOf('}'), 
                                response.size() - response.lastIndexOf('{'));
            }
            response.append(readLine());
        } while (isMultiline(response));

        // Break if End Response Found.
        if (isResponseEnd(response))
            break;

        // Parse and Add Message to Mailbox
        ImapMessage *message = _imapParseMessage(response.trimmed());
        if (message != NULL) mailbox->addMessage(message);
    }
    
    return(mailbox);
}

QList<ImapMessageBodyPart *> ImapPrivate::bodyPartSplit (const QByteArray& response) {
    QList<ImapMessageBodyPart *> partList;
    int count = 0;
    int index = 1;
    int i = 0;

    do {
        int next = index;
        do {
            if (response[next] == '(')
                i++;
            else if (response[next] == ')')
                i--;
            next++;
        } while (i > 0 || response[next - 1] != ')');

        if (i >= 0 && response[index] == '(') {
            count++;

            if (response.indexOf("((", index) == index) {
                QByteArray part = response.mid(index, next);
                QList<ImapMessageBodyPart *> tempParts = bodyPartSplit(part);
                foreach (ImapMessageBodyPart *p, tempParts) {
                    p->setBodyPart(QString("%1.%2").arg(count).arg(p->bodyPart()));
                    partList.append(p);
                }
            } else {
                QByteArray part = response.mid(index, next - index);
                ImapMessageBodyPart *bodyPart = new ImapMessageBodyPart(part);
                bodyPart->setBodyPart(QString::number(count));
                partList.append(bodyPart);
            }
        } else if (partList.size() == 0) {
            ImapMessageBodyPart *bodyPart = new ImapMessageBodyPart(response);
            bodyPart->setBodyPart("1");
            partList.append(bodyPart);
        }

        index = next;
    } while (i >= 0);    

    return(partList);
}

QByteArray ImapPrivate::parseBodyPart (ImapMessageBodyPart::Encoding encoding) {
    QByteArray response;
    QByteArray data;

    while (!isResponseEnd((response = readLine()))) {
        response.remove(response.size() - 2, 2);
        if (encoding == ImapMessageBodyPart::Base64Encoding) {
            data += response;
        } else if (encoding == ImapMessageBodyPart::QuotedPrintableEncoding) {
            if (response.endsWith('=') || response.endsWith(')'))
                data += response.mid(0, response.size() - 1);
            else
                data += response + IMAP_MESSAGE_BODY_NEWLINE;
        } else {
            data += response + IMAP_MESSAGE_BODY_NEWLINE;
        }
    }

    if (data.trimmed().endsWith(')'))
        data = data.remove(data.lastIndexOf(')'), 1);

    if (encoding != ImapMessageBodyPart::Base64Encoding)
        return(Imap::decode(data));
    return(data);
}


QString ImapPrivate::rfcDate (const QDateTime& date) const {
    return(date.toString("dd-MMM-yyyy HH:mm:ss +0000"));
}

// ===========================================================================
//  PUBLIC Constructors/Destructor
// ===========================================================================
Imap::Imap()
    : d(new ImapPrivate)
{
}

Imap::~Imap() {
    disconnectFromHost();
    delete d;
}

// ===========================================================================
//  PUBLIC STATIC Methods (Decode)
// ===========================================================================
QString Imap::decode (const QString& text) {
    return(decode(text.toLatin1()));
}

QByteArray Imap::decode (const QByteArray& text) {
    QRegExp rx("=\\?([^\\?]+)\\?([^\\?]+)\\?([^\\?]+)\\?=");
    QByteArray output;
    int pos = 0;

    while ((pos = rx.indexIn(text, pos)) != -1) {
        QString method = rx.cap(2).toLower();
        QString data = rx.cap(3);

        if (method == "b") {
            output += QByteArray::fromBase64(data.toLatin1());
        } else if (!data.isEmpty()) {
            bool ok;
            for (int i = 0; i < data.size(); ++i) {
                if (data[i] == '=') {
                    i++;
                    int chr = data.mid(i, 2).toInt(&ok, 16);
                    if (data.size() >= (i + 2) && ok) {
                        output += (char)chr;
                        i++;
                    }
                } else if (data[i] == '_') {
                    output += ' ';
                } else {
                    output += data[i].toAscii();
                }
            }
        }

        pos += rx.matchedLength();
    }

    return(output.size() > 0 ? output : text);
}

// ===========================================================================
//  PUBLIC Methods (Connect/Disconnect)
// ===========================================================================
/**
 * Connect to IMAP Server, using SSL connection if useSsl flag is True.
 */
bool Imap::connectToHost (const QString& host, quint16 port, bool useSsl) {
    if (!d->connectToHost(host, port, useSsl))
        return(false);

    QByteArray response = d->readLine();
    if (!response.startsWith("* OK"))
        return(false);

    return(true);
}

#ifndef QT_NO_OPENSSL
/**
 * Connect to IMAP Server using SSL connection.
 */
bool Imap::connectToSslHost (const QString& host, quint16 port) {
    return(connectToHost(host, port, true));
}
#endif

/**
 * Disconnect from IMAP Server.
 */
bool Imap::disconnectFromHost (void) {
    if (d->socket == NULL)
        return(true);

    if (d->socket->state() == QAbstractSocket::UnconnectedState)
        return(true);

    d->socket->disconnectFromHost();
    if (!d->socket->waitForDisconnected())
        return(false);

    delete d->socket;
    d->socket = NULL;
    return(true);
}

// ===========================================================================
//  PUBLIC Methods (IMAP Login/Logout)
// ===========================================================================
/**
 * Login to the IMAP Server, using the specified username/password.
 * TODO: Need to check server capabilities before use specified login type.
 */
bool Imap::login (const QString& username,
                  const QString& password,
                  LoginType type)
{
    // TODO: Check Capabilities before use specified Login Type.
    //QString capabilities = capability();

    switch (type) {
        case LoginPlain:
            if (!d->sendCommand("LOGIN %1 %2", QStringList() << username << password))
                return(false);
            break;
        case LoginAuthenticate:
            if (!d->sendCommand("AUTHENTICATE LOGIN"))
                return(false);

            // Send Username
            d->readLine();            
            if (!d->sendDataLine(username.toLatin1().toBase64()))
                return(false);
                
            // Send Password
            d->readLine();
            if (!d->sendDataLine(password.toLatin1().toBase64()))
                return(false);
            
            break;        
        case LoginCramMd5:            
            if (!d->sendCommand("AUTHENTICATE CRAM-MD5"))
                return(false);

            QByteArray serverResponse = d->readLine();
            QByteArray hmac = d->hmacMd5(username, password, serverResponse);
            if (!d->sendDataLine(hmac))
                return(false);
            break;
    }
    
    QByteArray result = d->readLine();
    if (!d->isResponseOk(result)) {
        d->responseErrorMsg = result;
        return(false);
    }
    return(true);
}

/**
 * Shutdown connection to server. 
 */
bool Imap::logout (void) {
    if (!d->sendCommand("LOGOUT"))
        return(false);

    bool ok = false;
    d->readLine(&ok);
    return(ok);
}

// ===========================================================================
//  PUBLIC Methods (IMAP Commands)
// ===========================================================================
/**
 * Returns server capabilities.
 */
QString Imap::capability (void) {
    if (!d->sendCommand("CAPABILITY"))
        return(false);
    
    QByteArray capability = d->readLine();
    d->readLine();
    
    return(capability.trimmed());
}

// ===========================================================================
//  PUBLIC Methods (IMAP Mailbox Related)
// ===========================================================================
/**
 * Permanently remove deleted items from selected mailbox.
 */
bool Imap::expunge (void) {
    QRegExp regexExpunge("^"IMAP_TAG"\\d+\\WOK\\W(EXPUNGE\\W|)COMPLETE");
    QByteArray response;
        
    if (!d->sendCommand("EXPUNGE"))
        return(false);
    
    do {
        response = d->readLine();
        if (regexExpunge.exactMatch(response))
            return(true);
    } while (!d->isResponseEnd(response));
    
    return(true);
}

/* 
 * Selects a mailbox to perform commands on.
 * Returns a mailbox object containing the properties of the mailbox.
 */
ImapMailbox *Imap::select (const QString& mailbox) {
    if (!d->sendCommand("SELECT %1", QStringList() << mailbox))
        return(NULL);

    return(d->parseMailbox(mailbox));
}

/* 
 * Examines a mailbox.
 * Returns a mailbox object containing the properties of the mailbox.
 */
ImapMailbox *Imap::examine (const QString& mailbox) {    
    if (!d->sendCommand("EXAMINE %2", QStringList() << mailbox))
        return(NULL);

    return(d->parseMailbox(mailbox));
}

/**
 * List mailbox names in directory matching pattern. 
 * Directory defaults to the top-level mail folder, 
 * and pattern defaults to match anything.
 */
QStringList Imap::list (const QString& directory, const QString& pattern) {
    QByteArray response;
    QStringList folders;
        
    if (!d->sendCommand("LIST %1 %2", QStringList() << directory << pattern))
        return(folders);    

    QRegExp rx("\\((.*)\\)\\s+\"(.*)\"\\s+\"(.*)\"");
    while (!d->isResponseEnd(response = d->readLine())) {
        if (rx.indexIn(response) != -1)
            folders.append(rx.cap(3));
    }

    return(folders);
}

/*
 * Create Mailbox with specified name.
 */
bool Imap::createMailbox (const QString& mailbox) {
    QByteArray response;
        
    if (!d->sendCommand("CREATE %1", QStringList() << mailbox))
        return(false);

    do {
        response = d->readLine();
    } while (!d->isResponseEnd(response));
    
    return(d->isResponseOk(response));
}

/**
 * Create Mailbox with specified name 'folder.name'.
 */
bool Imap::createMailbox (const QString& folder, const QString& name) {
    return(createMailbox(QString("%1%.%2").arg(folder).arg(name)));
}

/**
 * Rename Mailbox (oldName) with the specified newName.
 */
bool Imap::renameMailbox (const QString& oldName, const QString& newName) {
    QByteArray response;
        
    if (!d->sendCommand("RENAME %1 %2", QStringList() << oldName << newName))
        return(false);

    do {
        response = d->readLine();
    } while (!d->isResponseEnd(response));
    
    return(d->isResponseOk(response));
}

/**
 * Delete Mailbox with specified name.
 */
bool Imap::deleteMailbox (const QString& mailbox) {
    QByteArray response;
        
    if (!d->sendCommand("DELETE %1", QStringList() << mailbox))
        return(false);    

    do {
        response = d->readLine();
    } while (!d->isResponseEnd(response));
    
    return(d->isResponseOk(response));
}

/**
 * Delte Mailbox Named 'folder.name'.
 */
bool Imap::deleteMailbox (const QString& folder, const QString& name) {
    return(deleteMailbox(QString("%1.%2").arg(folder).arg(name)));
}

/**
 * Copy Messages onto end of new Mailbox.
 */
bool Imap::copyMailbox (const QString& mailbox, int begin, int end) {
    QByteArray response;

    QString command = "COPY %1:%2 \"%3\"";
    if (!d->sendCommand(command.arg(begin).arg(end).arg(mailbox)))
        return(false);

    do {
        response = d->readLine();
    } while (!d->isResponseEnd(response));

    return(true);
}

// ===========================================================================
//  PUBLIC Methods (IMAP Message Related)
// ===========================================================================
/**
 * Fetch UID of specified Message. (message->id())
 */
int Imap::fetchUid (int messageNumber) {
    if (!d->sendCommand(QString("FETCH %1 UID").arg(messageNumber)))
        return(-1);

    QByteArray response = d->readLine();
    if (!response.startsWith('*')) {
        d->responseErrorMsg = response;
        return(-1);
    }

    QRegExp regex("\\(UID (\\d+)\\)");
    if (regex.indexIn(response) != -1) {
        int uid = regex.cap(1).toInt();
        d->readLine();
        return(uid);
    }
    
    return(-1);
}

/**
 * Fetch messages of selected mailbox from 'begin' to 'end'.
 */
ImapMailbox *Imap::fetch (int begin, int end) {
    ImapMailbox *mailbox = new ImapMailbox;
    return(fetch(mailbox, begin, end));
}

/**
 * Fetch message of selected mailbox.
 */
ImapMailbox *Imap::fetch (const QList<int>& messages) {
    if (messages.size() < 1)
        return(NULL);

    ImapMailbox *mailbox = new ImapMailbox;
    return(fetch(mailbox, messages));
}

/**
 * Fetch all mailbox messages.
 */
ImapMailbox *Imap::fetch (ImapMailbox *mailbox) {
    if (!d->sendCommand("FETCH 1:* ALL"))
        return(NULL);

    return(d->parseMessages(mailbox));
}

/**
 * Fetch messages of selected mailbox from 'begin' to 'end'.
 */
ImapMailbox *Imap::fetch (ImapMailbox *mailbox, int begin, int end) {
    if (!d->sendCommand(QString("FETCH %1:%2 ALL").arg(begin).arg(end)))
        return(NULL);

    return(d->parseMessages(mailbox));
}

/**
 * Fetch messages of selected mailbox.
 */
ImapMailbox *Imap::fetch (ImapMailbox *mailbox, const QList<int>& messages) {
    if (messages.size() < 1)
        return(NULL);

    QString messageList;
    foreach (int messageNr, messages)
        messageList += QString("%1,").arg(messageNr);
    messageList.remove(messageList.size() - 1, 1);
    
    if (!d->sendCommand(QString("FETCH %1 ALL").arg(messageList)))
        return(NULL);

    return(d->parseMessages(mailbox));
}

/**
 * Fetch headers of specified message.
 */
ImapMessage *Imap::fetchHeaders (int messageNumber) {
    ImapMailbox mailbox;
    fetch(&mailbox, messageNumber, messageNumber);
    ImapMessage *message = mailbox.takeAt(0);
    mailbox.clearMessages();    
    return(message);
}

/**
 * Set seen flag at specified value to message.
 */
bool Imap::setSeen (int messageNumber, bool value) {
    return(d->setFlag(fetchUid(messageNumber), "\\Seen", value));
}

/**
 * Set draft flag at specified value to message.
 */
bool Imap::setDraft (int messageNumber, bool value) {
    return(d->setFlag(fetchUid(messageNumber), "\\Draft", value));
}

/**
 * Set recent flag at specified value to message.
 */
bool Imap::setRecent (int messageNumber, bool value) {
    return(d->setFlag(fetchUid(messageNumber), "\\Recent", value));
}

/**
 * Set flagged flag at specified value to message.
 */
bool Imap::setFlagged (int messageNumber, bool value) {
    return(d->setFlag(fetchUid(messageNumber), "\\Flagged", value));
}

/**
 * Set deleted flag at specified value to message.
 */
bool Imap::setDeleted (int messageNumber, bool value) {
    return(d->setFlag(fetchUid(messageNumber), "\\Deleted", value));
}

/**
 * Set answered flag at specified value to message.
 */
bool Imap::setAnswered (int messageNumber, bool value) {
    return(d->setFlag(fetchUid(messageNumber), "\\Answered", value));
}

/**
 * Fetch message body structure.
 */
bool Imap::fetchBodyStructure (ImapMessage *message) {
    QByteArray response;
    QByteArray data;
    
    if (!d->sendCommand(QString("FETCH %1 BODYSTRUCTURE").arg(message->id())))
        return(false);

    while (!d->isResponseEnd((response = d->readLine())))
        data += response;
    
    if (!data.startsWith('*')) {
        d->responseErrorMsg = response;
        return(false);
    }
    
    data.remove(0, data.indexOf(" (", data.indexOf("BODYSTRUCTURE")));
    QList<ImapMessageBodyPart *> bodyParts = d->bodyPartSplit(data.trimmed());
    message->setBodyParts(bodyParts);
    for (int i = 0; i < 2 && i < bodyParts.size(); ++i) {
        QString contentType = bodyParts[i]->contentType().toUpper();
        if (contentType == "TEXT/PLAIN") {
            message->setTextPartIndex(i);
        } else if (contentType == "TEXT/HTML") {
            message->setHtmlPartIndex(i);
        }
    }

    return(true);
}

/**
 * Fetch Specified Message body part.
 */
bool Imap::fetchBodyPart (ImapMessage *message, int part) {
    ImapMessageBodyPart *msgPart = message->bodyPartAt(part);
    QString bodyPart = msgPart->bodyPart();
    int msgId = message->id();

    if (!d->sendCommand(QString("FETCH %1 BODY[%2]").arg(msgId).arg(bodyPart)))
        return(false);

    QByteArray response = d->readLine();
    if (!response.startsWith('*')) {
        d->responseErrorMsg = response;
        return(false);
    }

    msgPart->setData(d->parseBodyPart(msgPart->encoding()));
    return(true);
}

// ===========================================================================
//  PUBLIC Methods (IMAP Message Search Related)
// ===========================================================================
/**
 * Search mailbox for matching messages.
 *  The IMAP protocol requires that at least one criterion be specified.
 */
QList<int> Imap::search (const QString& criteria) {
    QList<int> messageList;

    if (criteria.isEmpty())
        return(messageList);

    if (!d->sendCommand("SEARCH %1", QStringList() << criteria))
        return(messageList);    

    QByteArray response = d->readLine();
    if (!response.startsWith("* SEARCH")) {
        d->responseErrorMsg = response;
        return(messageList);
    }

    // Parse Search Results
    int lastIndex = 8;
    int index = 9;
    while ((index = response.indexOf(' ', index)) >= 0) {        
        QByteArray number = response.mid(lastIndex + 1, index - lastIndex - 1);
        messageList.append(number.toInt());

        lastIndex = index;
        index++;
    }

    if ((index = response.indexOf('\r', lastIndex)) >= 0) {
        int size = (index - lastIndex - 1);
        if (size > 0) {
            QByteArray number = response.mid(lastIndex + 1, size);  
            messageList.append(number.toInt());
        }
    }

    // Ok Response Line.
    d->readLine();

    return(messageList);
}

/** Search for Messages with specified TO criteria. */
QList<int> Imap::searchTo (const QString& criteria) {
    return(search(QString("TO %1").arg(criteria)));
}

/** Search for Messages with specified CC criteria. */
QList<int> Imap::searchCc (const QString& criteria) {
    return(search(QString("CC %1").arg(criteria)));
}

/** Search for Messages with specified BCC criteria. */
QList<int> Imap::searchBcc (const QString& criteria) {
    return(search(QString("BCC %1").arg(criteria)));
}

/** Search for Messages with specified FROM criteria. */
QList<int> Imap::searchFrom (const QString& criteria) {
    return(search(QString("FROM %1").arg(criteria)));
}

/** Search for Messages with specified TEXT criteria. */
QList<int> Imap::searchText (const QString& criteria) {
    return(search(QString("TEXT %1").arg(criteria)));
}

/** Search for Messages with specified BODY criteria. */
QList<int> Imap::searchBody (const QString& criteria) {
    return(search(QString("BODY %1").arg(criteria)));
}

/** Search for Messages with specified SUBJECT criteria. */
QList<int> Imap::searchSubject (const QString& criteria) {
    return(search(QString("SUBJECT %1").arg(criteria)));
}

/** Search for Messages since specified date. */
QList<int> Imap::searchSince (const QDateTime& date) {
    return(search(QString("SINCE %1").arg(d->rfcDate(date))));
}

/** Search for Messages sent on specified date. */
QList<int> Imap::searchSentOn (const QDateTime& date) {
    return(search(QString("SENTON %1").arg(d->rfcDate(date))));
}

/** Search for Messages sent since specified date. */
QList<int> Imap::searchSentSince (const QDateTime& date) {
    return(search(QString("SENTSINCE %1").arg(d->rfcDate(date))));
}

/** Search for Messages sent before specified date. */
QList<int> Imap::searchSentBefore (const QDateTime& date) {
    return(search(QString("SENTBEFORE %1").arg(d->rfcDate(date))));
}

/** Search for Messages with Size Larger than specified size. */
QList<int> Imap::searchSizeLarger (int size) {
    return(search(QString("LARGER %1").arg(size)));
}

/** Search for Messages with Size Smaller than specified size. */
QList<int> Imap::searchSizeSmaller (int size) {
    return(search(QString("SMALLER %1").arg(size)));
}

/** Search for Draft Messages. */
QList<int> Imap::searchDraft (void) {
    return(search("DRAFT"));
}

/** Search for Recent Messages. */
QList<int> Imap::searchRecent (void) {
    return(search("RECENT"));
}

/** Search for Unseen Messages. */
QList<int> Imap::searchUnseen (void) {
    return(search("UNSEEN"));
}

/** Search for Deleted Messages. */
QList<int> Imap::searchDeleted (void) {
    return(search("DELETED"));
}

/** Search for Answered Messages. */
QList<int> Imap::searchAnswered (void) {
    return(search("ANSWERED"));
}

/** Search for Unanswered Messages. */
QList<int> Imap::searchUnanswered (void) {
    return(search("UNANSWERED"));
}

/** Search for New Messages (Recent Unseen). */
QList<int> Imap::searchRecentUnseen (void) {
    return(search("RECENT UNSEEN"));
}

// ===========================================================================
//  PUBLIC Properties
// ===========================================================================
QString Imap::errorString (void) const {
    if (d->responseErrorMsg.isEmpty())
        return(d->socket->errorString());
    return(d->responseErrorMsg);
}

