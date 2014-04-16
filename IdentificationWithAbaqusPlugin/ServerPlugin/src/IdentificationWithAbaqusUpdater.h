#ifndef IDENTIFICATIONWITHABAQUSUPDATER_H
#define IDENTIFICATIONWITHABAQUSUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class IdentificationWithAbaqusUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "IdentificationWithAbaqusUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // IDENTIFICATIONWITHABAQUSUPDATER_H
