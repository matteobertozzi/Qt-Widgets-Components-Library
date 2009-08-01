#include <QString>

#include "imapaddress.h"

// ===========================================================================
//  PUBLIC Constructors/Destructor
// ===========================================================================
ImapAddress::ImapAddress()
    : d(new ImapAddressData)
{
}

ImapAddress::ImapAddress (const QString& address)
    : d(new ImapAddressData)
{
    d->address = address;
}

ImapAddress::ImapAddress (const QString& address, const QString& displayName) 
    : d(new ImapAddressData)
{
    d->address = address;
    d->displayName = displayName;
}

ImapAddress::ImapAddress (const ImapAddress& address) 
    : d(address.d)
{
}

ImapAddress::~ImapAddress() {
}

// ===========================================================================
//  PUBLIC Methods 
// ===========================================================================
QString ImapAddress::toString (void) const {
    if (d->displayName.isEmpty())
        return(d->address);

    return(QString("\"%1\" <%2>").arg(d->displayName).arg(d->address));
}

// ===========================================================================
//  PUBLIC Properties 
// ===========================================================================
bool ImapAddress::isNull (void) const {
    return(d->address.isEmpty());
}

QString ImapAddress::address (void) const {
    return(d->address);
}

void ImapAddress::setAddress (const QString& address) {
    d->address = address;
}
        
bool ImapAddress::hasDisplayName (void) const {
    return(!d->displayName.isEmpty());
}

QString ImapAddress::displayName (void) const {
    return(d->displayName);
}

void ImapAddress::setDisplayName (const QString& displayName) {
    d->displayName = displayName;
}

QString ImapAddress::smtpDomain (void) const {
    return(d->smtpDomain);
}

void ImapAddress::setSmtpDomain (const QString& smtpDomain) {
    d->smtpDomain = smtpDomain;
}

