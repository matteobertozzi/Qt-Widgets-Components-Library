#ifndef _IMAP_H_
#define _IMAP_H_

class ImapMessage;
class ImapMailbox;
class ImapPrivate;
class Imap {
    public:
        enum LoginType { LoginPlain, LoginAuthenticate, LoginCramMd5 };

    public:
        Imap();
        ~Imap();

    public:
        static QString decode (const QString& text);
        static QByteArray decode (const QByteArray& text);

    public:
        // Methods (Connect/Disconnect)
        bool connectToHost   (const QString& host, 
                               quint16 port = 143,
                               bool useSsl = false);
#ifndef QT_NO_OPENSSL
        bool connectToSslHost (const QString& host,
                               quint16 port = 993);
#endif

        bool disconnectFromHost (void);

        // Methods (IMAP Login/Logout)
        bool login (const QString& username,
                    const QString& password,
                    LoginType type = LoginPlain);
        bool logout (void);

        // Methods (IMAP Commands)
        QString capability (void);

        // Methods (Imap Mailbox Related)
        bool expunge (void);

        ImapMailbox *select  (const QString& mailbox);
        ImapMailbox *examine (const QString& mailbox);

        QStringList list (const QString& directory = "\"\"", 
                          const QString& pattern = "*");

        bool createMailbox (const QString& mailbox);
        bool createMailbox (const QString& folder, const QString& name);

        bool renameMailbox (const QString& oldName, const QString& newName);

        bool deleteMailbox (const QString& mailbox);
        bool deleteMailbox (const QString& folder, const QString& name);

        bool copyMailbox (const QString& mailbox, int begin, int end);

        // Methods (Imap Message Related)
        int fetchUid (int messageNumber);

        ImapMailbox *fetch (int begin, int end);
        ImapMailbox *fetch (const QList<int>& messages);
        ImapMailbox *fetch (ImapMailbox *mailbox);
        ImapMailbox *fetch (ImapMailbox *mailbox, int begin, int end);
        ImapMailbox *fetch (ImapMailbox *mailbox, const QList<int>& messages);

        ImapMessage *fetchHeaders (int message);

        bool fetchBodyStructure (ImapMessage *message);
        bool fetchBodyPart (ImapMessage *message, int part);

        bool setSeen (int messageNumber, bool value);
        bool setDraft (int messageNumber, bool value);
        bool setRecent (int messageNumber, bool value);
        bool setFlagged (int messageNumber, bool value);
        bool setDeleted (int messageNumber, bool value);
        bool setAnswered (int messageNumber, bool value);

        // Methods (Imap Message Search Related)
        QList<int> search (const QString& criteria);
        QList<int> searchTo (const QString& criteria);
        QList<int> searchCc (const QString& criteria);
        QList<int> searchBcc (const QString& criteria);
        QList<int> searchFrom (const QString& criteria);
        QList<int> searchText (const QString& criteria);
        QList<int> searchBody (const QString& criteria);
        QList<int> searchSubject (const QString& criteria);
        QList<int> searchSince (const QDateTime& criteria);
        QList<int> searchSentOn (const QDateTime& criteria);
        QList<int> searchSentSince (const QDateTime& criteria);
        QList<int> searchSentBefore (const QDateTime& criteria);
        QList<int> searchSizeLarger (int size);
        QList<int> searchSizeSmaller (int size);

        QList<int> searchDraft (void);
        QList<int> searchRecent (void);
        QList<int> searchUnseen (void);
        QList<int> searchDeleted (void);
        QList<int> searchAnswered (void);
        QList<int> searchUnanswered (void);
        QList<int> searchRecentUnseen (void);

        // Properties
        QString errorString (void) const;

    private:
        ImapPrivate *d;
};

#endif /* !_IMAP_H_ */

