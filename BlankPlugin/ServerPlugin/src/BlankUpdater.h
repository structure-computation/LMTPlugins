#ifndef BLANKUPDATER_H
#define BLANKUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class BlankUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "BlankUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // BLANKUPDATER_H
