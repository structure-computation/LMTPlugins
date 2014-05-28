#ifndef VIRTUALGAUGEUPDATER_H
#define VIRTUALGAUGEUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class VirtualGaugeUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "VirtualGaugeUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // VIRTUALGAUGEUPDATER_H
