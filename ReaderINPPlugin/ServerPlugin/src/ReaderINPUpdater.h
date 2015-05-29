#ifndef READERINPUPDATER_H
#define READERINPUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class ReaderINPUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "ReaderINPUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // READERINPUPDATER_H
