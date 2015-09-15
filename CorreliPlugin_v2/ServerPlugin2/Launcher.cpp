#include <iostream>
#include <qglobal.h>
#include "Launcher.h"


Launcher::Launcher(){  

};

Launcher::~Launcher(){
    qDebug() << "CLOSE LOOP";
};

void Launcher::launch(SodaClient::Event event){
    if(event.event_num == 1){  //evennement issu du timer
        //test
    }

    if(event.event_num != 1){  //evennement issu d'une modification de l'objet sur le serveur 
        MP output_visu = event.mp();
//        qDebug() << output_visu.type();

        if(output_visu.type() == "FieldSetCorreliItem"){
            int time = output_visu[ "time.val" ];
            int old_time = output_visu[ "_old_time_step" ];
//            qDebug() << time << old_time;

            if(time != old_time){

                //Chargement du fichier contenant les résultats
                MP data_file = output_visu[ "_file_field_set_data" ];

                quint64 ptr = data_file[ "_ptr" ];
                MP output_field = sc->load_ptr( ptr );

                MP list_visualisation_output = output_field["visualization.color_by.lst"];
//                qDebug() << "list_visualisation_output = " << list_visualisation_output.size();
                MP list_visualisation = output_visu["visualization.color_by.lst"];
//                qDebug() << "list_visualisation = " << list_visualisation.size();


                //parcours du champs de résultats
                for(int i=0; i<list_visualisation_output.size(); i++){
                    QString name = list_visualisation_output[i]["name"];
                    MP data = list_visualisation_output[i]["data._data"];
                    MP data_visu = list_visualisation[i]["data._data"];
//                    qDebug() << data_visu;
                    for(int j=0; j<data.size(); j++){
                        QString axe_name = data[j]["pos[0].axe_name"];
                        int axe_value = data[j]["pos[0].axe_value"];
                        if(axe_name == "time" and axe_value == time){
                            data_visu.clear();
                            data_visu << data[j];
//                            qDebug() << data_visu[0]["pos"];
                        }
                    }
                }


                output_visu[ "_old_time_step" ] = time;
                output_visu.flush();
            }
        }
//        qDebug() << "event";
    }
};


