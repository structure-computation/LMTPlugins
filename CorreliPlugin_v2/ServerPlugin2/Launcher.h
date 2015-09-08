#ifndef LAUNCHER_H
#define LAUNCHER_H
#include <Soca/Com/SodaClient.h>
#include <QtCore/QProcess>
#include <QtCore/QList>


class Launcher : public QObject { 
  Q_OBJECT
 public:
    Launcher();
    
    ~Launcher();

    
 public slots:
      void launch(SodaClient::Event event);
      
 public: 
      SodaClient* sc;                           //référence au SodaClient

};


#endif // LAUNCHER_H
