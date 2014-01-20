#ifndef TESTFLOUPDATER_H
#define TESTFLOUPDATER_H

#include <Soca/Updater.h>
class SodaClient;
class QDataStream;

/**
*/
class TestFloUpdater : public Updater {
protected:
    bool run( MP mp );
    void make_geo( QTextStream &geo, const MP &ch, double base_size );
    void make_geo_rec( QMap<Model *,QVector<int> > &elem_corr, QTextStream &geo, const MP &elem, double base_size, const MP &points, int &np, int &ne );
    virtual QString type() const { return "TestFloUpdater"; }
    
public:
    SodaClient *sc;
    const char*& geo();
};

#endif // TESTFLOUPDATER_H
