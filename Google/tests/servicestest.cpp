#include <QtTest>

#include <QStringList>
#include <QEventLoop>
#include <QDebug>

#include "googledetectlanguage.h"
#include "googlespellchecker.h"
#include "googletranslator.h"
#include "googlesuggest.h"
#include "googleauth.h"

#include "servicestest.h"

#define GOOGLE_USERNAME         "USER@gmail.com"
#define GOOGLE_PASSWORD         "PASSWORD"

ServicesTest::ServicesTest (QObject *parent)
    : QObject(parent)
{
}

ServicesTest::~ServicesTest() {
}

void ServicesTest::testAuth (void) {
    QEventLoop l;
    THGoogleAuth gAuth(GOOGLE_USERNAME);
    connect(&gAuth, SIGNAL(authenticated()), &l, SLOT(quit()));
    gAuth.login(THGoogleAuth::Contacts, GOOGLE_PASSWORD);
    l.exec();

    if (gAuth.error() == THGoogleAuth::NoError) {
        qDebug() << "SID" << gAuth.sid();
        qDebug() << "LSID" << gAuth.lsid();
        qDebug() << "AUTH" << gAuth.auth();
    } else {
        qDebug() << gAuth.errorString();
    }

    QVERIFY(1);     // Do some test...
}

void ServicesTest::testTranslator (void) {
    QEventLoop l;

    THGoogleTranslator gTranslator;
    connect(&gTranslator, SIGNAL(finished(bool)), &l, SLOT(quit()));
    gTranslator.setDestinationLanguage("en");
    gTranslator.translate("Ciao");

    l.exec();

    qDebug() << gTranslator.query();
    qDebug() << gTranslator.translatedText();
    qDebug() << gTranslator.detectedSourceLanguage();

    QVERIFY(1);     // Do some test...
}

void ServicesTest::testSuggestions (void) {
    QEventLoop l;

    THGoogleSuggest gSuggest;
    connect(&gSuggest, SIGNAL(finished(bool)), &l, SLOT(quit()));
    gSuggest.suggest("Qt Softw");

    l.exec();

    qDebug() << gSuggest.suggestions();

    QVERIFY(1);     // Do some test...
}

void ServicesTest::testSpellChecker (void) {
    QEventLoop l;

    THGoogleSpellChecker gSpellChecker;
    connect(&gSpellChecker, SIGNAL(finished(bool)), &l, SLOT(quit()));
    gSpellChecker.spellCheck("Gogl Spll");

    l.exec();

    foreach (QString original, gSpellChecker.keys())
        qDebug() << original << gSpellChecker.suggestions(original);

    QVERIFY(1);     // Do some test...
}

void ServicesTest::testLanguageDetection (void) {
    QEventLoop l;

    THGoogleDetectLanguage gDetectLang;
    connect(&gDetectLang, SIGNAL(finished(bool)), &l, SLOT(quit()));
    gDetectLang.detectLanguage("Ciao");

    l.exec();

    qDebug() << "Query" << gDetectLang.query();
    qDebug() << "Language" << gDetectLang.language();
    qDebug() << "Is Reliable" << gDetectLang.isReliable();
    qDebug() << "Confidence" << gDetectLang.confidence();

    QVERIFY(1);     // Do some test...
}

QTEST_MAIN(ServicesTest)

