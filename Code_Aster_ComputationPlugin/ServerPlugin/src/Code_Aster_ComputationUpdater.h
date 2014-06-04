#ifndef CODE_ASTER_COMPUTATIONUPDATER_H
#define CODE_ASTER_COMPUTATIONUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class Code_Aster_ComputationUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "Code_Aster_ComputationUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // CODE_ASTER_COMPUTATIONUPDATER_H
