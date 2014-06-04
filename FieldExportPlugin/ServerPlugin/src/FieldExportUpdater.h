#ifndef FIELDEXPORTUPDATER_H
#define FIELDEXPORTUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class FieldExportUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "FieldExportUpdater"; }
    
public:
    SodaClient *sc;
};

#endif // FIELDEXPORTUPDATER_H
