#ifndef ABAQUSCOMPUTATION_H
#define ABAQUSCOMPUTATION_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class AbaqusComputationUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "AbaqusComputationUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // TESTFLOUPDATER_H
