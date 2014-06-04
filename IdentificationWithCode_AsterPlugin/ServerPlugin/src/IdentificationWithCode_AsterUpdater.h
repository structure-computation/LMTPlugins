#ifndef IDENTIFICATIONWITHCODE_ASTERUPDATER_H
#define IDENTIFICATIONWITHCODE_ASTERUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class IdentificationWithCode_AsterUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "IdentificationWithCode_AsterUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // IDENTIFICATIONWITHCODE_ASTERUPDATER_H
