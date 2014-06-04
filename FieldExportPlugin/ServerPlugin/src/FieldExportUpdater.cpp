#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "FieldExportUpdater.h"
#include "../../../sources/commandes_generales.h"
#include "mesh/write_mesh_vtk.h"
#include "mesh/interpolation.h"
                        
typedef LMT::Mesh<Mesh_carac_pb_correlation_basic<double,2> > TM;

bool FieldExportUpdater::run( MP mp ) {
    
  
    MP ch = mp[ "_children" ];
    MP c, i;
    int nb_fields=0;
    int nb_imgs=0;

    
    for (int nch=0; nch< ch.size(); nch++){
	//QString s= "_children[" + QString::fromStdString(to_string(0)) + "]";
	MP chi = ch[ nch ];
	if (( chi.type() == "FieldSetItem" ) or ( chi.type() == "FieldSetCorreliItem" )){
	    QString s= "_children[" + QString::fromStdString(to_string(nch)) + "]";
	    c.reassign(chi);
	    nb_fields++;
	}
	if ( chi.type() == "ImgItem" ){
	    QString s= "_children[" + QString::fromStdString(to_string(nch)) + "]";
	    i.reassign(chi);
	    nb_imgs++;
	}   
    }
    
    if (nb_fields == 0){
	add_message( mp, ET_Info, "No input field or mesh !" );    mp.flush(); 
	PRINT("No input field or mesh !");
    }
    else if (nb_fields > 1){
	add_message( mp, ET_Info, "More than one input field or mesh. Possible confusion ! Doing nothing." );    mp.flush(); 
	PRINT("MORE THAN 1 INPUT FIELD, POSSIBLE CONFUSION");
    }
    else if (nb_fields == 1){
    
	Vec<TM> Mesh_Vector = load_FieldSetItem_into_LMTpp_Mesh(c);
	DicCPU<double, TM::dim> dic; // For the computation of the strain
	
	int format = mp["Format.num"];
	QString adresse = mp["Adresse"];std::string stdadresse = adresse.toStdString();
	QString nom = mp["Nom"];std::string stdnom = nom.toStdString();
	
	std::string racine_fic;
	if (stdadresse[stdadresse.size()-1] == '/')
	    racine_fic = stdadresse + stdnom + "_";
	else
	    racine_fic = stdadresse + "/" + stdnom + "_";
	
	if ( exists(stdadresse) == 0)
	    create_dir(stdadresse);
	
	int save_disp = mp["Displacements"];
	int save_eps = mp["Strains"];
	    
	if (format == 0){// TXT
	    // ELEMENTS
	    std::string name_els = racine_fic + "elements.txt";
	    if (exists(name_els)) remove(name_els.c_str());
	    PRINT(name_els);
	    std::ofstream els (name_els.c_str());
	    for (int i = 0; i < Mesh_Vector[0].elem_list.size(); i++){ 
		els << i + 1 ;
		for (int elsize =0; elsize < Mesh_Vector[0].elem_list[i]->nb_nodes_virtual(); elsize ++)
		    els << ", " << Mesh_Vector[0].elem_list[i]->node_virtual(elsize)->number + 1 ;
		els << "\n";
	    }
	    //NOEUDS
	    std::string name_nodes = racine_fic + "nodes.txt";
	    if (exists(name_nodes)) remove(name_nodes.c_str());
	    PRINT(name_nodes);
	    std::ofstream nodes (name_nodes.c_str());
	    //PRINT(Mesh_Vector[0].node_list[0].dim);
	    for (int i = 0; i < Mesh_Vector[0].node_list.size(); i++){
		nodes << i + 1 ;
		for (int numnodes = 0; numnodes < Mesh_Vector[0].node_list[0].dim; numnodes++)
		    nodes << ", " << Mesh_Vector[0].node_list[i].pos[ numnodes ];
	    nodes << "\n";
	    }
	    // DEPLACEMENTS
	    std::string rac_disp = racine_fic + "disp";
	    std::string rac_eps = racine_fic + "eps";
	    for (int i = 0; i < Mesh_Vector.size(); i++){
		if (save_disp){
		    for (int j = 0; j < Mesh_Vector[0].node_list[0].dim; j++){
			std::string name_disp = rac_disp + "XYZ"[ j ] + "_im" + to_string(i+1) + ".txt";
			if (exists(name_disp)) remove(name_disp.c_str());
			std::ofstream disp (name_disp.c_str());
			for (int k = 0; k < Mesh_Vector[0].node_list.size(); k++)
			    disp << Mesh_Vector[i].node_list[k].dep[j] << "\n";
		    }
		}
		if (save_eps){
		    dic.get_epsilon( Mesh_Vector[i] );
		    GetEpsInVecs ge;
		    apply( Mesh_Vector[i].elem_list, ge );
		    for (int j = 0; j < 3; j++){
			std::string name_eps;
			if (j == 0)
			  name_eps = rac_eps + "X" + "_im" + to_string(i+1) + ".txt";
			else if (j == 1)
			  name_eps = rac_eps + "Y" + "_im" + to_string(i+1) + ".txt";
			else if (j == 2)
			  name_eps = rac_eps + "XY" + "_im" + to_string(i+1) + ".txt";
			if (exists(name_eps)) remove(name_eps.c_str());
			std::ofstream eps (name_eps.c_str());
			for (int k = 0; k < Mesh_Vector[0].elem_list.size(); k++)
			    eps << ge.e[k][j] << "\n";
		    }
		}
	    }
	    add_message( mp, ET_Info, "Résultat écrit" );
	}
	else if (format == 1){// VTU
	    for (int num_mesh = 0; num_mesh < Mesh_Vector.size(); num_mesh++){
	      dic.get_epsilon( Mesh_Vector[num_mesh] );
	      write_mesh_vtk( racine_fic + "im" + to_string(num_mesh+1) + ".vtu",Mesh_Vector[ num_mesh ]);
	    } 
	    add_message( mp, ET_Info, "Résultat écrit" );
	}
	else if (format == 2){// IMAGE
	    for (int num_mesh = 0; num_mesh < Mesh_Vector.size(); num_mesh++)
		dic.get_epsilon( Mesh_Vector[num_mesh] );
	    if (nb_imgs == 0){
		add_message( mp, ET_Info, "Image edition needs a reference image to fix the size of the edited one." );    mp.flush(); 
		PRINT("No input image !");
	    }
	    else if (nb_imgs > 1){
		add_message( mp, ET_Info, "More than one input image. Possible confusion ! Doing nothing." );    mp.flush(); 
		PRINT("MORE THAN 1 INPUT IMAGE, POSSIBLE CONFUSION");
	    }
	    else if (nb_imgs == 1){
		I2 image, ref_image; 
		QString name = i[ "img.src" ];
		qDebug() << name.toAscii().data();
                ref_image.load( name.toAscii().data() );
                ref_image.reverse_y();
		
		std::string rac_disp = racine_fic + "disp";
		std::string rac_eps = racine_fic + "eps";
		
		PRINT(rac_disp+".png");
		PRINT(ref_image.sizes);
		
		image.resize(ref_image.sizes);
 		for (int ii =0; ii<image.sizes[0]; ii++){
 		    for (int jj =0; jj<image.sizes[1]; jj++){
			image.tex_int(ii,jj) =0;
		    }
		}
		
		
		for (int num_mesh = 0; num_mesh < Mesh_Vector.size(); num_mesh++){
		  
		    if (save_disp){
			Pvec tmxmin, tmxmax;
			get_min_max( Mesh_Vector[ num_mesh ].node_list, ExtractDM<dep_DM>(), tmxmin, tmxmax );
			apply( Mesh_Vector[num_mesh].elem_list, FillImgWhenInsideElement(), image , 1, tmxmin[0]);
			ref_image.reverse_y();
			image.save(rac_disp+"_" + to_string(num_mesh) + "_x.png",true);
			//ref_image.reverse_y();
			apply( Mesh_Vector[num_mesh].elem_list, FillImgWhenInsideElement(), image , 2, tmxmin[1]);
			//ref_image.reverse_y();
			image.save(rac_disp+"_" + to_string(num_mesh) + "_y.png",true);
		    }
		    if (save_eps){
			GetEpsInVecs ge;
			apply( Mesh_Vector[num_mesh].elem_list, ge );
			double min1 = 1e9, min2 = 1e9, min12 = 1e9;
			for (int k = 0; k < Mesh_Vector[0].elem_list.size(); k++){
			    if (ge.e[k][0] < min1) min1 = ge.e[k][0];
			    if (ge.e[k][1] < min2) min2 = ge.e[k][1];
			    if (ge.e[k][2] < min1) min12 = ge.e[k][2];
			}
			apply( Mesh_Vector[num_mesh].elem_list, FillImgWhenInsideElement(), image , 11, min1);
			ref_image.reverse_y();
			image.save(rac_eps+"_" + to_string(num_mesh) + "_x.png",true);
			ref_image.reverse_y();
			apply( Mesh_Vector[num_mesh].elem_list, FillImgWhenInsideElement(), image , 12, min2);
			ref_image.reverse_y();
			image.save(rac_eps+"_" + to_string(num_mesh) + "_y.png",true);
			ref_image.reverse_y();
			apply( Mesh_Vector[num_mesh].elem_list, FillImgWhenInsideElement(), image , 13, min12);
			ref_image.reverse_y();
			image.save(rac_eps+"_" + to_string(num_mesh) + "_xy.png",true);
		    }
		}
		
		add_message( mp, ET_Info, "Résultat écrit" );
	    }
	}
	
	mp.flush();
    
    }
    return 0;
    
}



