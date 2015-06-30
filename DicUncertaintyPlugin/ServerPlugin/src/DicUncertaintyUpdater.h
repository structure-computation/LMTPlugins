#ifndef DICUNCERTAINTYUPDATER_H
#define DICUNCERTAINTYUPDATER_H
#include <Soca/Com/SodaClient.h>
#include <Soca/Updater.h>
// #include <containers/vec.h>
// #include "../../../../Soda/src/Soda/Sys/Vec.h"

// #include "../../../../LMTpp/include/containers/vec.h"
//using namespace LMT;
// #include "LMT/include/containers/mat.h"
#include <containers/mat.h>
//  puis utiliser l'espace LMT
    using namespace std;
    using namespace LMT;
class SodaClient;
class QDataStream;

/**
*/
class DicUncertaintyUpdater : public Updater {
protected:
    bool run( MP mp );
    virtual QString type() const { return "DicUncertaintyUpdater"; }
//     void set_vecToTreeItemVec(const Vec<double> &vec, MP MPvec, MP mpTreeItem_Vec, QString attrName);
    double round_to_digits(const double value, const int digits);
    void set_vecToTreeItemVec(const Vec<double> &vec, MP mpp, int index, QString attrName);
    
public:
    SodaClient *sc;
    void new_lst(const char* arg1);
    void print(const char* arg1);
    int vec_temp();
//     void qDebug(const char* arg1);
  //  void PRINT(Vec< double > ErrY);
//     void mean(Vec< double > Ux);
//double mean(Vec< double > Ux);//TEST
};

#endif // DICUNCERTAINTYUPDATER_H
