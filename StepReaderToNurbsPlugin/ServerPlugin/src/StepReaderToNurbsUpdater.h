#ifndef STEPREADERTONURBSUPDATER_H
#define STEPREADERTONURBSUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class StepReaderToNurbsUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "StepReaderToNurbsUpdater"; }
    
public:
    SodaClient *sc;
    const char*& geo();
};

#endif // STEPREADERTONURBSUPDATER_H
