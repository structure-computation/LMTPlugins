#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <containers/vec.h>
#include <containers/eig_lapack.h>
#include "dic/correlation/ImgInterp.h"
#include "dic/correlation/DicCPU.h"
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
    
    //////////////
    
    extract_images(param, images); // Lecture des éventuelles images 
    
    if ((images.size() == 0) and (method == "int")){
	method = "femu";
	PRINT("NO IMAGE DETECTED - USE OF FEMU INSTEAD OF INTEGRATED-DIC");
    }
    
    extract_computation_parameters( param, Mesh_Vector_input, constrained_nodes, indices_bc_cn,  Prop_Mat, fs_output, force_files); // Lecture des paramètres du calcul
    
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
	Vec <Vec <Vec <double> > > calc_dep, calc_force_nodes;
	Vec<double> fnod;
	Vec <Vec<double> > calc_force;
	
	Mesh_Vector_output = Mesh_Vector_input;
	calc_abq_into_LMTppMesh(Mesh_Vector_output, Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Ref field computation for a given set of parameter
	 
	extract_dep_from_LMTppMesh( Mesh_Vector_output, calc_dep );
	extract_fnod_from_LMTppMesh( Mesh_Vector_output, senstrac, calc_force_nodes, constrained_nodes );
	
	for (int p2id =0; p2id<prop2id.size(); p2id++){
	    PRINT(Prop_Mat[prop2id[p2id]][0]);
	    PRINT(Prop_Mat[prop2id[p2id]][1]);
	}
	for (int sf = 0; sf < prop2id.size(); sf++){
	    std::cout << "Sensitivity field " << sf << " : " << Prop_Mat[prop2id[sf]][0] << std::endl;
	    Prop_Mat = Prop_Mat_Backup;
	    double prop;
	    std::string texte = Prop_Mat[prop2id[sf]][1];
	    std::istringstream iss(texte);
	    iss >> prop;
	    prop *= (1 + offset);
	    Prop_Mat[prop2id[sf]][1] = LMT::to_string(prop); 
	    PRINT(Prop_Mat[prop2id[sf]][0]);
	    PRINT(Prop_Mat[prop2id[sf]][1]);
	    Vec<TM> SF = Mesh_Vector_input;
	    calc_abq_into_LMTppMesh(SF, Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Modified fields computations
	    extract_dep_from_LMTppMesh( SF, calc_dep );
	    extract_fnod_from_LMTppMesh( SF, senstrac, calc_force_nodes, constrained_nodes );
	}
	
	Vec<Vec<double> > depl_mes;
	for (int num_mesh = 0; num_mesh < Mesh_Vector_input.size(); ++num_mesh ) {
	    Vec<double> deplmes;
	    for( int n = 0; n < Mesh_Vector_input[ num_mesh ].node_list.size(); ++n ) {
		for( int d = 0; d < TM::dim; ++d ) {
		    deplmes << Mesh_Vector_input[ num_mesh ].node_list[ n ].dep[ d ]*pix2m;
		} 
	    }
	    depl_mes.push_back(deplmes);
	}
	
	
	
	    
	for( int num_mesh = 0; num_mesh < Mesh_Vector_input.size(); ++num_mesh ) { // num_mesh <=> steps
	  
	    Vec<Vec<double> > der;
	    //// transfert des CS dans "der"
	    for (int nsf=1; nsf<calc_dep.size(); nsf++){
		Vec < Vec < double > > CS;
		CS = (calc_dep[0] - calc_dep[nsf])/offset/pix2m;
		Vec <double> temp;
		for (int ii=0+num_mesh*Mesh_Vector_input[ num_mesh ].node_list.size(); ii< Mesh_Vector_input[ num_mesh ].node_list.size()+num_mesh*Mesh_Vector_input[ num_mesh ].node_list.size(); ii++){
		    temp.push_back(CS[0][ii]);
		    temp.push_back(CS[1][ii]);
		}
		der.push_back(temp);
	    }
	    
	    DicCPU<double,2> dic;
	    dic.ass_red( images[ 0 ], images[ num_mesh + 1 ], Mesh_Vector_output[ num_mesh ], dep_DM(), lum_DM(), der ); // assemblage
	  
	  
	  
            if (method == "int"){
                M_red += dic.M_red;
                F_red += dic.F_red;
            }
            else if (method == "femu"){
		PRINT("     NO WEIGHTING WITH THE CORRELATION MATRIX ");
		Vec<double> deplcal;
		for( int n = 0; n < Mesh_Vector_output[ num_mesh ].node_list.size(); ++n ) {
		    for( int d = 0; d < TM::dim; ++d ) {
			deplcal << Mesh_Vector_output[ num_mesh ].node_list[ n ].dep[ d ];
		    }
		}
		
		Vec<double> depmes;
		depmes=depl_mes[num_mesh];
		
		for( int r = 0; r < prop2id.size(); ++r ) {
		    for( int c = 0; c <= r; ++c ){
			M_red( r, c ) += dot( der[ r ], der[ c ] );
		    }
		    F_red[ r ] += dot( der[ r ], (depmes - deplcal)/pix2m );
		}
	    }
	    
	    
	}
	
	if (UF){
		Vec<double> meas_force, res_f;
		//meas_force = load_res(force_files);
		
		// transfert from nodal forces to global forces associated to the bc groups
		for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
		    meas_force = load_res(force_files[ncl][0]);
		    meas_force.resize(calc_force_nodes[0].size());
		    calc_force.resize(calc_force_nodes.size());
		    if (force_files[ncl][1] == "-")
			meas_force = - meas_force;
		    else if (force_files[ncl][1] == "0")
			meas_force *= 0;
		    Mat<double,Sym<>,SparseLine<> > MF_red( prop2id.size() );
		    Vec<double> FF_red; FF_red.resize( prop2id.size(), 0 );
		    calc_force_nodes.resize(calc_dep.size());
		    for (int nsf =0; nsf<calc_dep.size(); nsf ++){
			calc_force[nsf].resize(Mesh_Vector_output.size());
			for (int nim =0; nim<Mesh_Vector_output.size(); nim ++){
			    calc_force[nsf][nim] =0;
			    for (int nn =0; nn<calc_force_nodes[0][0].size(); nn++){
				if (indices_bc_cn[nn] == ncl){
				    calc_force[nsf][nim] += calc_force_nodes[nsf][nim][nn];
				}
			    }
			}
		    }
		    
		    res_f=meas_force-calc_force[0];
			PRINT(meas_force);
			PRINT(calc_force[0]);
			PRINT(res_f);
		    //write_vec(calc_force[0],"dernier_effort_calc.txt");

		    for( int r = 0; r < prop2id.size(); ++r ) {
			for( int c = 0; c <= r; ++c ){
			    MF_red( r, c ) += dot( (calc_force[ 0 ] - calc_force [ r + 1 ]) / offset, (calc_force[ 0 ] - calc_force [ c + 1 ]) / offset );
			}
			FF_red[ r ] += dot( (calc_force[ 0 ] - calc_force [ r + 1 ]) / offset, res_f );
		    }
		    VMF << MF_red;
		    VFF << FF_red;
		    
		}
	}
	
	
	Mat<double,Sym<>,SparseLine<> > M_tot ;
	Vec<double> F_tot ;
	M_tot = M_red;
	F_tot = F_red ;
	if (UF){
		for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
    //	        int ncl = 1;
		    M_tot +=  ponderation_efforts * VMF[ncl] * w / (max(indices_bc_cn)+1);
		    F_tot +=  ponderation_efforts * VFF[ncl] * w / (max(indices_bc_cn)+1);
		    //M_tot =  ponderation_efforts * VMF[ncl] * w / (max(indices_bc_cn)+1);
		    //F_tot =  ponderation_efforts * VFF[ncl] * w / (max(indices_bc_cn)+1);
		}
	}
	
 	PRINT(M_red);
 	PRINT(F_red);
 	PRINT(VMF);
 	PRINT(VFF);
 	PRINT(M_tot);
 	PRINT(F_tot);
 	
	Vec<double> dif = - relaxation * solve_using_eig_sym( M_tot , F_tot , 1e-40 ); // RESOLUTION dp=N-1 B
	PRINT(dif);

	for (int jj=0; jj< dif.size(); jj++){
	    if (dif[jj] >  0.5) dif[jj]= 0.5;
	    if (dif[jj] < -0.5) dif[jj]=-0.5;
	}
// 	if (thelaw == "Equation"){
// 	    for (int jj=0; jj< dif.size(); jj++){
// 		if (dif[jj] >  0.1) dif[jj]= 0.1;
// 		if (dif[jj] < -0.1) dif[jj]=-0.1;
// 	    }	
// 	}
	PRINT( dif );
	
	Prop_Mat = Prop_Mat_Backup;
	// Properties update
	for (int nsf = 0; nsf < dif.size(); nsf++){
	    double prop;
	    std::string texte = Prop_Mat[prop2id[nsf]][1];
	    std::istringstream iss(texte);
	    iss >> prop;
	    prop *= (1 + dif[nsf]);
	    if (thelaw == "RO"){
		if (Prop_Mat[prop2id[nsf]][1] == "n"){
		    if (prop < 1.01)
		       prop =1.1;
		}
	    }
	    Prop_Mat[prop2id[nsf]][1] = LMT::to_string(prop);
	}
	
	
	   
	if ( (norm_inf( dif ) < 1e-3) or (it+1 == iterations) ){
	   //it_report, M_d_report, M_f_report, F_d_report, F_f_report, calc_force_report, meas_force_report
	    it_report = it;
	    dif_report = dif;
	    M_d_report = M_red;
	    F_d_report = F_red;
	    calc_force_report = calc_force;
	    meas_force_report = meas_force_report;
	    PRINT(VMF.size());
	    PRINT(VFF.size());
	    for (int ncl = 0; ncl < VMF.size(); ncl++){
		M_f_report << VMF[ncl];
		F_f_report << VFF[ncl];
	    }
	    PRINT(M_f_report.size());
	    PRINT(F_f_report.size());
	    for (int num_mesh = 0; num_mesh < Mesh_Vector_output.size(); num_mesh++)
		write_mesh_vtk( "/home/mathieu/aaa_test" + to_string(num_mesh) + ".vtu",Mesh_Vector_output[ num_mesh ]);
	    if (thelaw == "Elas_iso"){
		
		Vec<double> meas_force, res_f, coef_vec;
		for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
		    if (exists(force_files[ncl][0])){
			meas_force = load_res(force_files[ncl][0]);
			calc_force.resize(calc_force_nodes.size());
			if (force_files[ncl][1] == "-")
			    meas_force = - meas_force;
			else if (force_files[ncl][1] == "0")
			    meas_force *= 0;
			calc_force_nodes.resize(calc_dep.size());
			for (int nsf =0; nsf<calc_dep.size(); nsf ++){
			    calc_force[nsf].resize(Mesh_Vector_output.size());
			    for (int nim =0; nim<Mesh_Vector_output.size(); nim ++){
				calc_force[nsf][nim] =0;
				for (int nn =0; nn<calc_force_nodes[0][0].size(); nn++){
				    if (indices_bc_cn[nn] == ncl){
					calc_force[nsf][nim] += calc_force_nodes[nsf][nim][nn];
				    }
				}
			    }
			}
			meas_force.resize(calc_force[0].size());
			
			res_f=meas_force-calc_force[0];
			PRINT(meas_force);
			PRINT(calc_force[0]);
			PRINT(res_f);
			double coef = pow(dot(calc_force[0],calc_force[0])/ dot(meas_force, meas_force),0.5);
			coef_vec << coef;
			PRINT(coef);
		    }
		}
		
		if (res_f.size() != 0){
		    std::string texte = Prop_Mat[1][1];
		    std::istringstream iss(texte);
		    double prop;
		    iss >> prop;
		    prop /=  coef_vec[0];
		    Prop_Mat[1][1] = LMT::to_string(prop); 
		}
	
	    }
	    if ( (norm_inf( dif ) < 1e-3)) id_ok = 1;
	    else id_ok = 0;
            break;
        }
        
        
    }
    
    mpid["id_done"] = id_ok;
    push_back_material_parameters(param, Prop_Mat); mpid.flush();
    
    std::string report_address = "/home/mathieu/report.txt";
    write_identification_report (report_address, Mesh_Vector_output, Prop_Mat, it_report, iterations, M_d_report, M_f_report, F_d_report, F_f_report, calc_force_report, meas_force_report, prop2id, ponderation_efforts, dif_report);

    
    //put_result_in_MP(Mesh_Vector_output, mpid, fs_output); // Sortie dans un FieldSet "calcul"
    add_message( mpid, ET_Info, "Résultat renvoyé" );    mpid.flush();
    
    
    
    return 1;
}



