#include <Soca/Com/SodaClient.h>
#include "BlankUpdater.h"

int main( int argc, char **argv ) {
    // connection
    SodaClient sc( QHostAddress::Any, 8890 );
    if ( not sc.connected() ) return 1;
  
    //id du model à traiter
    quint64 model_id = atoi(argv[1]);   
    sc.reg_type( "BlankItem" );
    MP mp = sc.load_ptr(model_id);
    qDebug() << "###############   launch BlankItem ###############" ;
    BlankUpdater updater;
    updater.sc = &sc;
    updater.exec( mp );
    qDebug() << "###############   finish stand alone BlankItem ###############" ;
    
    
}
