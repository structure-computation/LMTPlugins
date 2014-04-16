#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <containers/vec.h>
#include <containers/eig_lapack.h>
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
    Vec<TM> Mesh_Vector_input; 
    Vec<int> constrained_nodes; 
    Vec<int> indices_bc_cn;
    MP param = mpid["_children[0]"];
    double pix2m = param[ "pix2m" ];
    double thickness = param[ "thickness" ];
    double offset = mpid["offset"];
    int iterations = mpid["iterations"];
    double relaxation = mpid["relaxation"]; 
    QString Qforce_file = mpid["force"];std::string force_file = Qforce_file.toStdString();
    FieldSet fs_output;
    Vec < Vec < std::string > > Prop_Mat, Prop_Mat_Backup; // FORMAT "GENERIQUE" LMT : vecteur de vecteurs string qui contiennent le nom ([0]) et la valeur ([1])
    Vec<int> prop2id;
    bool id_ok = 0;
    
    //////////////
    
    std::string senstrac = "hor";
    
    //////////////
    
    extract_computation_parameters( param, Mesh_Vector_input, constrained_nodes, indices_bc_cn,  Prop_Mat, fs_output); // Lecture des paramètres du calcul

    PRINT(Prop_Mat);    
    
    std::string thelaw = Prop_Mat[0][1];
    bool UF =1;
    
    if (thelaw == "Elas_iso"){
      UF = 0;
    }
    
    if (thelaw == "Elas_iso"){
	prop2id << 2; // nu
    }
    else if (thelaw == "Elas_ortho"){
	prop2id << 2;// nu
	prop2id << 3;// E1/E2
    }
    else if (thelaw == "RO"){
	prop2id << 3;// sigma_0
	prop2id << 4;// n
    }
    else if (thelaw == "UMAT"){
	std::istringstream iss(Prop_Mat[1][1]);
	int umat_nparam;
	iss >> umat_nparam;
	std::string texte2 = Prop_Mat[1+1+umat_nparam+1][1];
	std::istringstream iss2(texte2);
	int umat_nparamid;
	iss2 >> umat_nparamid;
	for (int npa = 0; npa < umat_nparamid; npa++){
	    std::string stri = "paramid" + LMT::to_string(npa);
	    std::string texte = Prop_Mat[1+1+umat_nparam+1+1+npa][1];
	    std::istringstream is(Prop_Mat[1+1+umat_nparam+1+1+npa][1]);
	    int prop;
	    is >> prop;
	    prop2id << prop + 1 ;
	}
    }
    else if (thelaw == "Equation"){
	std::istringstream iss(Prop_Mat[3][1]);
	int nparam;
	iss >> nparam;
	std::string texte2 = Prop_Mat[2+2+2*nparam][1];
	std::istringstream iss2(texte2);
	int nparamid; 
	iss2 >> nparamid;
	for (int npa = 0; npa < nparamid; npa++){
	    std::istringstream is(Prop_Mat[2+2+2*nparam+1+npa][1]);
	    int prop;
	    is >> prop;
	    prop2id << prop + 3 ;
	}
    }
    else if (thelaw == "UMAT_Lem_diccit"){
	std::istringstream iss(Prop_Mat[1][1]);
	int umat_nparam;
	iss >> umat_nparam;
	std::string texte2 = Prop_Mat[1+1+umat_nparam+1][1];
	std::istringstream iss2(texte2);
	int umat_nparamid;
	iss2 >> umat_nparamid;
	for (int npa = 0; npa < umat_nparamid; npa++){
	    std::string stri = "paramid" + LMT::to_string(npa);
	    std::string texte = Prop_Mat[1+1+umat_nparam+1+1+npa][1];
	    std::istringstream is(Prop_Mat[1+1+umat_nparam+1+1+npa][1]);
	    int prop;
	    is >> prop;
	    prop2id << prop + 1 ;
	}
    }
    //Prop_Mat_Backup = Prop_Mat;
    
    
    double ponderation_efforts = pow(0.1,2)*2*Mesh_Vector_input[0].node_list.size()/pow(1.41,2); // basic, FEMU without weighting
    double w = 1;
    
    for (int it = 0 ; it < iterations; it++){
	
	Prop_Mat_Backup = Prop_Mat;
      
	Mat<double,Sym<>,SparseLine<> > M_red( prop2id.size() );
	Mat<double,Sym<>,SparseLine<> > H( prop2id.size() );
	Vec<double> F_red; F_red.resize( prop2id.size(), 0 );
	Vec<Mat<double,Sym<>,SparseLine<> > > VMF;
	Vec <Vec<double> > VFF;
	Vec <Vec <Vec <double> > > calc_dep, calc_force_nodes;
	Vec<double> fnod;
	Vec <Vec<double> > calc_force;
	
	Vec<TM> Mesh_Vector_output = Mesh_Vector_input;
	calc_abq_into_LMTppMesh(Mesh_Vector_output, Mesh_Vector_input, constrained_nodes, pix2m, Prop_Mat, thickness); // Ref field computation for a given set of parameter
	 
    PRINT("coucou");
	extract_dep_from_LMTppMesh( Mesh_Vector_output, calc_dep );
    PRINT("coucou");
	extract_fnod_from_LMTppMesh( Mesh_Vector_output, senstrac, calc_force_nodes, constrained_nodes );
    PRINT("coucou");
	
	for (int p2id =0; p2id<prop2id.size(); p2id++){
	    PRINT(Prop_Mat[prop2id[p2id]][0]);
	    PRINT(Prop_Mat[prop2id[p2id]][1]);
	}
	for (int sf = 0; sf < prop2id.size(); sf++){
    PRINT("coucou");
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
	
    PRINT("coucou");
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
	
	
	PRINT("     NO WEIGHTING WITH THE CORRELATION MATRIX ");
	    
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
		F_red[ r ] += dot( der[ r ], depmes - deplcal );
	    }
	    
	    
	}
	
	if (UF){
		Vec<double> meas_force, res_f;
		meas_force = load_res(force_file);
		meas_force.resize(calc_force_nodes[0].size());
		calc_force.resize(calc_force_nodes.size());
		
		// transfert from nodal forces to global forces associated to the bc groups
		for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
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
		    VMF.push_back(MF_red);
		    VFF.push_back(FF_red);
		    
		}
	}
	
	
	Mat<double,Sym<>,SparseLine<> > M_tot ;
	Vec<double> F_tot ;
	M_tot = M_red;
	F_tot = F_red ;
	if (UF){
	    //for (int ncl = min(indices_bc_cn); ncl < max(indices_bc_cn)+1; ncl++){
	        int ncl = 1;
		M_tot +=  ponderation_efforts * VMF[ncl] * w / (max(indices_bc_cn)+1);
		F_tot +=  ponderation_efforts * VFF[ncl] * w / (max(indices_bc_cn)+1);
		//M_tot =  ponderation_efforts * VMF[ncl] * w / (max(indices_bc_cn)+1);
		//F_tot =  ponderation_efforts * VFF[ncl] * w / (max(indices_bc_cn)+1);
	    //}
	}
	PRINT(UF);
	PRINT(VMF);
	PRINT(VFF);
	
	
	PRINT(ponderation_efforts);
	PRINT(M_red);
	PRINT(F_red);
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
	   
	  for (int num_mesh = 0; num_mesh < Mesh_Vector_output.size(); num_mesh++){
	      write_mesh_vtk( "/home/mathieu/aaa_test" + to_string(num_mesh) + ".vtu",Mesh_Vector_output[ num_mesh ]);
	  }
	if ( (norm_inf( dif ) < 1e-3) or (it == iterations) ){
	    for (int num_mesh = 0; num_mesh < Mesh_Vector_output.size(); num_mesh++)
	      write_mesh_vtk( "/home/mathieu/aaa_test" + to_string(num_mesh) + ".vtu",Mesh_Vector_output[ num_mesh ]);
            break;
	    id_ok = 1;
        }
    }
    
    mpid["id_done"] = id_ok;
    push_back_material_parameters(param, Prop_Mat); mpid.flush();
    
    //put_result_in_MP(Mesh_Vector_output, mp, fs_output); // Sortie dans un FieldSet "calcul"
    add_message( mpid, ET_Info, "Résultat renvoyé" );    mpid.flush();
    
    return 1;
}



