#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include <containers/vec.h>
#include "TestFloUpdater.h"

bool TestFloUpdater::run( MP mp ) {
  
    int valeur1 = mp[ "valeur1" ];
    std::cout << valeur1 << std::endl;
    mp[ "valeur1" ]=100;
    add_message( mp, ET_Info, "Valeur 1 modifiée" );
    mp.flush();
    
    //mp[ "_mesh.points[0].pos[0]" ] = 0.5;
    MP mesh = mp[ "_mesh" ];
    MP points = mesh[ "points" ];
    MP pos = points[0][ "pos" ];
    pos[1] = 2;
    add_message( mp, ET_Info, "Positions modifiées" );
    mp.flush();
    
    
  
}