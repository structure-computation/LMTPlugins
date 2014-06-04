#ifndef DICUNCERTAINTYUPDATER_H
#define DICUNCERTAINTYUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class DicUncertaintyUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "DicUncertaintyUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // DICUNCERTAINTYUPDATER_H
