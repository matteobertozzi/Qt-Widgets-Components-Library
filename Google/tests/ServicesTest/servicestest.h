#ifdef TEST_GOOGLE_SERVICES
#ifndef _SERVICES_TEST_H_
#define _SERVICES_TEST_H_

#include <QObject>

class ServicesTest : public QObject {
    Q_OBJECT

    public:
        ServicesTest (QObject *parent = 0);
        ~ServicesTest();

    private slots:
        void testAuth (void);
        void testWeather (void);
        void testGeoCoding (void);
        void testTranslator (void);
        void testSuggestions (void);
        void testSpellChecker (void);
        void testLanguageDetection (void);
};

#endif /* !_SERVICES_TEST_H_ */
#endif /* TEST_GOOGLE_SERVICES */
