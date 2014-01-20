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
    
    
    
  
}