#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <containers/vec.h>
#include "dic/correlation/ImgInterp.h"
#include "mesh/mesh.h"
#include "../../../sources/control_code_aster.h"
#include "mesh/write_mesh_vtk.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "IdentificationWithCode_AsterUpdater.h"

bool IdentificationWithCode_AsterUpdater::run( MP mpid ) {
    
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
    int ex_field =0;
    //Vec< std::string> force_file = Qforce_file.toStdString();
    Vec<Vec< std::string> > force_files;
    FieldSet fs_output;
    Vec < Vec < std::string > > Prop_Mat, Prop_Mat_Backup; // FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    Vec<int> prop2id;
    bool id_ok = 0;
    Vec<I2> images;
	bool UF =1;
    
    //////////////
    
    int senstrac = mpid["traction_direction"];
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
    
    extract_computation_parameters( mpid, Mesh_Vector_input, constrained_nodes, indices_bc_cn,  Prop_Mat, fs_output, force_files, ex_field); // Lecture des paramètres du calcul
    
    
    
    if (ex_field == 0){
	add_message( mpid, ET_Info, "No input field or mesh !" );    mpid.flush(); 
	PRINT("No input field or mesh !");
    }
    else if (ex_field > 1){
	add_message( mpid, ET_Info, "More than one input field or mesh. Possible confusion ! Doing nothing." );    mpid.flush(); 
	PRINT("MORE THAN 1 INPUT FIELD, POSSIBLE CONFUSION");
	Mesh_Vector_input.resize(0);
    }
    else if (ex_field == -1){
	add_message( mpid, ET_Info, "Not enough information (steps) in the txt files for the boundary conditions. Doing nothing." );    mpid.flush(); 
	Mesh_Vector_input.resize(0);
    }
    else if ( indices_bc_cn.size() == 0){
	add_message( mpid, ET_Info, "No boundary condition defined, by field or text file. Doing nothing." );    mpid.flush(); 
	Mesh_Vector_input.resize(0);
    }
    if (Mesh_Vector_input.size() == 0) {
	add_message( mpid, ET_Info, "There was a problem with the model edition. Check your computation conditions." );    mpid.flush(); 
    }
    else {    
            
        extract_id_properties(param, Prop_Mat, prop2id); // Lecture des paramètres d'identification
        
        PRINT(Prop_Mat);    

        //////////////
        
        std::string thelaw = Prop_Mat[0][1];
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
        Vec<double>  dif;
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
                    
            std::cout << " " << std::endl;
            std::cout << "Computation for sensitivity fields" << std::endl;
            std::cout << " " << std::endl;
                
            Vec<std::string> jobnames;
            
            //REF
            std::string jobname = launch_calc_code_aster_for_id(Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness, 0); // Reference field computation
            jobnames << jobname;
            
            // SENS
            for (int sf = 1; sf < prop2id.size()+1; sf++){
                Prop_Mat = update_material_parameters(Prop_Mat_Backup, prop2id, sf, offset);
                std::string jobname = launch_calc_code_aster_for_id(Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness, sf); // Modified fields computations
                jobnames << jobname;
            }
            
            std::cout << " " << std::endl;
            std::cout << "Waiting for result" << std::endl;
            std::cout << " " << std::endl;
            
            Vec<int> comp_finished; comp_finished.resize(prop2id.size()+1);
            for (int ii =0; ii < comp_finished.size(); ii++) comp_finished[ii] = 0;
            Vec<Vec<TM> > Metares; Metares.resize(prop2id.size()+1);
            for (int ii =0; ii < Metares.size(); ii++) Metares[ii] = Mesh_Vector_input;
            
            char* HomeDir;
            HomeDir = getenv ("HOME");
            std::string root_dir = std::string(HomeDir) + "/scratch";
            while (sum (comp_finished) < comp_finished.size()){
                for (int ii = 0; ii < comp_finished.size(); ii++){
                    if (exists((root_dir + "/ok_" + to_string(ii) + ".txt").c_str()) and (comp_finished[ii]==0) ) {
                        std::cout << " " << std::endl;
                        std::cout << "Computation " << ii << " finished" << std::endl;
                        std::cout << " " << std::endl;
                        load_aster_res_into_LMTppMesh(jobnames[ii], Metares[ii], thickness); 
                        extract_dep_from_LMTppMesh( Metares[ii], comp_disp );
                        extract_fnod_from_LMTppMesh( Metares[ii], senstrac, calc_force_nodes, constrained_nodes);
                        comp_finished[ii]=1;
                    }
                }
                sleep(1);
            }
            
            Mesh_Vector_output = Metares[0];
            
            build_matrix_for_the_kinematic_part(M_red, F_red, Mesh_Vector_input, Mesh_Vector_output, images, comp_disp, pix2m, offset, method);
            if (UF) build_matrix_for_the_force_part(VMF, VFF, force_files, calc_force, calc_force_nodes, indices_bc_cn, Mesh_Vector_output.size(), prop2id.size(), comp_disp, pix2m, offset, method, senstrac);
            assemble_global_matrix (M_tot, F_tot, M_red, F_red, VMF, VFF, UF, ponderation_efforts, w);
            
            if (F_tot.size() > 0){
                dif = solve_with_max(M_tot, F_tot, max_level, resolution, relaxation);
                update_properties(Prop_Mat, Prop_Mat_Backup, prop2id, dif);
            }
                
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
                if ( (norm_inf( dif ) < 1e-3)) id_ok = 1;
                else id_ok = 0;
                break;
            }
        }
        
        mpid["id_done"] = (char*)LMT::to_string(id_ok).c_str(); 
        push_back_material_parameters(param, Prop_Mat); mpid.flush();
        
        std::string report_address = root_dir + "/report";
        write_identification_report (report_address, Mesh_Vector_output, Prop_Mat, it_report, iterations, M_d_report, M_f_report, F_d_report, F_f_report, calc_force_report, meas_force_report, prop2id, ponderation_efforts, dif_report);

        put_result_in_MP(Mesh_Vector_output, mpid, fs_output); // Sortie dans un FieldSet "calcul"
        add_message( mpid, ET_Info, "Result transmitted" );    mpid.flush();
        return 1;
    }
}



