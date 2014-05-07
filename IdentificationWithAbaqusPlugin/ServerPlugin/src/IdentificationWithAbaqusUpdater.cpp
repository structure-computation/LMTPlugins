#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <containers/vec.h>
#include "dic/correlation/ImgInterp.h"
#include "mesh/mesh.h"
#include "../../../sources/control_abq.h"
#include "mesh/write_mesh_vtk.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "IdentificationWithAbaqusUpdater.h"

bool IdentificationWithAbaqusUpdater::run( MP mpid ) {
    
    //// Variables 
    Vec<TM> Mesh_Vector_input, Mesh_Vector_output;
    Vec<int> constrained_nodes; 
    Vec<int> indices_bc_cn;
    MP param = mpid["_children[0]"];
    double pix2m = param[ "pix2m" ];
    double thickness = param[ "thickness" ];
    double offset = mpid["offset"];
    int iterations = mpid["iterations"];
    double relaxation = mpid["relaxation"]; 
    QString Qforce_file = mpid["force"]; 
    //Vec< std::string> force_file = Qforce_file.toStdString();
    Vec<Vec< std::string> > force_files;
    FieldSet fs_output;
    Vec < Vec < std::string > > Prop_Mat, Prop_Mat_Backup; // FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    Vec<int> prop2id;
    bool id_ok = 0;
    Vec<I2> images;
    
    //////////////
    
    std::string senstrac = "hor";
    std::string method = "int";
	
    double resolution = 1e-40;
    double max_level = 0.5;
    
    char* HomeDir;
    HomeDir = getenv ("HOME");
    std::string root_dir = std::string(HomeDir) + "/scratch";
    
    //////////////
    
    extract_images(param, images); // Lecture des éventuelles images 
    
    if ((images.size() == 0) and (method == "int")){
	method = "femu";
	PRINT("NO IMAGE DETECTED - USE OF FEMU INSTEAD OF INTEGRATED-DIC");
    }
    
    extract_computation_parameters( param, Mesh_Vector_input, constrained_nodes, indices_bc_cn,  Prop_Mat, fs_output, force_files); // Lecture des paramètres du calcul
    PRINT(force_files);
    extract_id_properties(Prop_Mat, prop2id); // Lecture des paramètres d'identification

    //////////////
    
    PRINT(Prop_Mat);    
    
    std::string thelaw = Prop_Mat[0][1];
    bool UF =1;
    if (thelaw == "Elas_iso"){
	UF = 0;
    }
    
    double ponderation_efforts = pow(3,2)*2*Mesh_Vector_input[0].node_list.size()/pow(1.41,2); // basic, FEMU without weighting
    double w = 1;
    
    /////////// VARIABLES TO BE INCLUDED IN THE REPORT
    int it_report;
    Mat<double,Sym<>,SparseLine<> > M_d_report;
    Vec<Mat<double,Sym<>,SparseLine<> > > M_f_report;
    Vec<double> F_d_report, dif_report;
    Vec <Vec<double> > F_f_report;
    Vec <Vec<double> > calc_force_report, meas_force_report;
    /////////// VARIABLES TO BE INCLUDED IN THE REPORT

    int it;
    for (it = 0 ; it < iterations; it++){
	
	Prop_Mat_Backup = Prop_Mat;
      
	Mat<double,Sym<>,SparseLine<> > M_red( prop2id.size() );
	Mat<double,Sym<>,SparseLine<> > H( prop2id.size() );
	Vec<double> F_red; F_red.resize( prop2id.size(), 0 );
	Vec<Mat<double,Sym<>,SparseLine<> > > VMF;
	Vec <Vec<double> > VFF; 
	Vec <Vec <Vec <double> > > comp_disp, calc_force_nodes;
	Vec<double> fnod;
	Vec <Vec<double> > calc_force;
	
	Mat<double,Sym<>,SparseLine<> > M_tot ;
	Vec<double> F_tot ;
	
	Mesh_Vector_output = calc_abq_into_LMTppMesh(Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Ref field computation for a given set of parameter
	 
	extract_dep_from_LMTppMesh( Mesh_Vector_output, comp_disp );
	extract_fnod_from_LMTppMesh( Mesh_Vector_output, senstrac, calc_force_nodes, constrained_nodes );
	
	for (int p2id =0; p2id<prop2id.size(); p2id++){
	    std::cout << " " << std::endl;
	    PRINT(Prop_Mat[prop2id[p2id]][0]);
	    PRINT(Prop_Mat[prop2id[p2id]][1]);
	    std::cout << " " << std::endl;
	}
	for (int sf = 0; sf < prop2id.size(); sf++){
	    Prop_Mat = update_material_parameters(Prop_Mat_Backup, prop2id, sf, offset);
	    Vec<TM> SF = calc_abq_into_LMTppMesh(Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Modified fields computations
	    extract_dep_from_LMTppMesh( SF, comp_disp );
	    extract_fnod_from_LMTppMesh( SF, senstrac, calc_force_nodes, constrained_nodes);
	}
	
	build_matrix_for_the_kinematic_part(M_red, F_red, Mesh_Vector_input, Mesh_Vector_output, images, comp_disp, pix2m, offset, method);
	if (UF) build_matrix_for_the_force_part(VMF, VFF, force_files, calc_force, calc_force_nodes, indices_bc_cn, Mesh_Vector_output.size(), comp_disp, pix2m, offset, method);
	assemble_global_matrix (M_tot, F_tot, M_red, F_red, VMF, VFF, UF, indices_bc_cn, ponderation_efforts, w);
	
	Vec<double> dif = solve_with_max(M_tot, F_tot, max_level, resolution, relaxation);
	update_properties(Prop_Mat, Prop_Mat_Backup, prop2id, dif, thelaw);
	
	if ( (norm_inf( dif ) < 1e-3) or (it+1 == iterations) ){
	   //it_report, M_d_report, M_f_report, F_d_report, F_f_report, calc_force_report, meas_force_report
	    it_report = it;
	    dif_report = dif;
	    M_d_report = M_red;
	    F_d_report = F_red;
	    calc_force_report = calc_force;
	    meas_force_report = meas_force_report;
	    for (int ncl = 0; ncl < VMF.size(); ncl++){
		M_f_report << VMF[ncl];
		F_f_report << VFF[ncl];
	    }
	    
	    for (int num_mesh = 0; num_mesh < Mesh_Vector_output.size(); num_mesh++)
		write_mesh_vtk( root_dir + "/aaa_test" + to_string(num_mesh) + ".vtu",Mesh_Vector_output[ num_mesh ]);
	    
	    if (thelaw == "Elas_iso") calc_young_for_elastic_case(indices_bc_cn, force_files, calc_force, calc_force_nodes, prop2id.size(), Mesh_Vector_output.size(), Prop_Mat);

	    if ( (norm_inf( dif ) < 1e-3)) id_ok = 1;
	    else id_ok = 0;
            break;
        }
        
    }
    
    mpid["id_done"] = id_ok;
    push_back_material_parameters(param, Prop_Mat); mpid.flush();
    
    std::string report_address = root_dir + "/report.txt";
    write_identification_report (report_address, Mesh_Vector_output, Prop_Mat, it_report, iterations, M_d_report, M_f_report, F_d_report, F_f_report, calc_force_report, meas_force_report, prop2id, ponderation_efforts, dif_report);

    //put_result_in_MP(Mesh_Vector_output, mpid, fs_output); // Sortie dans un FieldSet "calcul"
    add_message( mpid, ET_Info, "Résultat renvoyé" );    mpid.flush();
    
    
    
    return 1;
}



