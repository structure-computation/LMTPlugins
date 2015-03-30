
#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include "LMT/include/containers/vec.h"
#include "dic/correlation/ImgInterp.h"
#include "LMT/include/mesh/mesh.h"
#include "header_abq.h"
#include "LMT/include/containers/apply_ij.h"
// #include </usr/local/6.14-1/code/include/odb_API.h> // pour les fonctions odb d'abaqus
#include "odb_API.h" // pour les fonctions odb d'abaqus
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "FieldSet.h"
#include "commandes_generales.h"

// Writes 2D (of extruded 3D) input file for Abaqus, based on a Vec(LMT::Mesh)
void Write2DINP (Vec<TM> &m, std::string root_dir, Vec<double> constrained_nodes, double pix2m, Vec < Vec < std::string > > Prop_Mat, double thickness){
    std::string nom_fic = ( root_dir + ".inp" );
    for (int z = 0; z <1; ++z) { // fausse boucle pour écrire l'inp
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string thelaw;
        double elastic_modulus, poisson_ratio, rapport, sigma_0, n, nparam, friction_angle , flowstress_ratio , dilation_angle, eps_0, time_order, pl_multiplier, eq_stress_order;
        int n_el;
	
        std::string computation_type = Prop_Mat[Prop_Mat.size()-1][0]; 
        std::string n_el_str = Prop_Mat[Prop_Mat.size()-1][1];
        std::istringstream iss(n_el_str);
        iss >> n_el;
		
        for (int ii=0; ii< Prop_Mat.size(); ii++){
            if (Prop_Mat[ii][0] == "thelaw"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> thelaw;
            }
            else if (Prop_Mat[ii][0] == "Young"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> elastic_modulus;
            }
            else if (Prop_Mat[ii][0] == "Poisson"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> poisson_ratio;     
            }
            else if (Prop_Mat[ii][0] == "rapport"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> rapport;   
            }
            else if (Prop_Mat[ii][0] == "sigma_0"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> sigma_0;     
            }
            else if (Prop_Mat[ii][0] == "n"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> n;      
            }
            else if (Prop_Mat[ii][0] == "eps_0"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> eps_0;      
            }
            else if (Prop_Mat[ii][0] == "friction_angle"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> friction_angle;      
            }
            else if (Prop_Mat[ii][0] == "flowstress_ratio"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> flowstress_ratio;      
            }
            else if (Prop_Mat[ii][0] == "dilation_angle"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> dilation_angle;      
            }
            else if (Prop_Mat[ii][0] == "pl_multiplier"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> pl_multiplier;      
            }
            else if (Prop_Mat[ii][0] == "eq_stress_order"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> eq_stress_order;      
            }
            else if (Prop_Mat[ii][0] == "time_order"){
                std::string texte = Prop_Mat[ii][1];
                std::istringstream iss(texte);
                iss >> time_order;      
            }
        }
        
        if (exists(nom_fic)) remove(nom_fic.c_str());
        int vide = system(("touch " + nom_fic).c_str());
        std::ofstream inp( nom_fic.c_str() , std::ios::app);
        Vec<std::string> eps; eps << "0.";
        int neps = 50;
        double maxeps=0.5;
        if (thelaw == "Equation"){
            for (int numeps = 1; numeps<neps; numeps++){
                eps << LMT::to_string(numeps*(maxeps/neps));
            }
          
            inp << "*Parameter\n";
            std::istringstream iss(Prop_Mat[3][1]);
            iss >> nparam;
            for (int npa=0; npa<nparam; npa++){
                inp << Prop_Mat[2+2+nparam+npa][1] << "=" << Prop_Mat[2+2+npa][1] << "\n";
            }
            for (int numeps = 0; numeps<neps; numeps++){
                inp << "eps" << numeps << "=" << eps[numeps] << "\n";
            }
            for (int numeps = 0; numeps<neps; numeps++){
                //inp << "sig" << numeps << "=" << Prop_Mat[2+2+nparam][1] << "*(" << Prop_Mat[2+2+nparam+1][1] << "+eps" << numeps << ")**" << Prop_Mat[2+2+nparam+2][1] << "\n"; // SWIFT
                //inp << "sig" << numeps << "=" << Prop_Mat[2+2+nparam][1] << "+" << Prop_Mat[2+2+nparam+1][1] << "*eps" << numeps << "**" << Prop_Mat[2+2+nparam+2][1] << "\n"; // POWER LAW
                std::string eq = Prop_Mat[Prop_Mat.size()-2][1]; 
                std::string subs = "varsigma";
                Vec<std::size_t> fs = find_str_in_str(eq, subs);
                std::string sube = "varepsilonp";
                Vec<std::size_t> fe = find_str_in_str(eq, sube);
                inp << eq.substr(0,fs[0]);
                inp << "sig" << numeps; 
                inp << eq.substr(fs[0]+subs.size(),fe[0]-(fs[0]+subs.size()));
                inp << "eps" << numeps;
                inp << eq.substr(fe[0]+sube.size(),eq.size()) << "\n";
            }
          
        }
        else if (thelaw == "UMAT_Lem_diccit"){  
            for (int numeps = 1; numeps<neps; numeps++){
                eps << LMT::to_string(numeps*(maxeps/neps));
            }
            inp << "*Parameter\n";
            inp << "E=" << Prop_Mat[3][1] << "\n";
            inp << "Nu=" << Prop_Mat[4][1] << "\n";
            inp << "B=" << Prop_Mat[5][1] << "\n";
            inp << "C=" << Prop_Mat[6][1] << "\n";
            inp << "n=" << Prop_Mat[7][1] << "\n";
            inp << "s=" << Prop_Mat[8][1] << "\n";
            inp << "S=" << Prop_Mat[9][1] << "\n";
            inp << "pth=" << Prop_Mat[10][1] << "\n";
            inp << "Dcr=" << Prop_Mat[11][1] << "\n";
            inp << "mDam=" << Prop_Mat[12][1] << "\n";
            inp << "eDam=" << Prop_Mat[13][1] << "\n";
          
            for (int numeps = 0; numeps<neps; numeps++){
            inp << "eps" << numeps << "=" << eps[numeps] << "\n";
            }
            for (int numeps = 0; numeps<neps; numeps++){
            inp << "sig" << numeps << "=B*(C+eps" << numeps << ")**n\n"; // SWIFT
            }
            inp << "G=E/(2.*(1+Nu))\n";
            inp << "Hourg=0.005*G\n";
        }
        
            inp << "*Heading\n";
            inp << "** Job name: test_florent Model name: Model-1\n";
            inp << "*Preprint, echo=NO, model=NO, history=NO, contact=NO\n";
            inp << "**\n";
            inp << "** PARTS\n";
            inp << "**\n";
            inp << "*Part, name=Part\n";
            inp << "*Node\n";
            if (computation_type == "3Dex"){
            for (int layer_number =0; layer_number < (n_el+1); layer_number++){
            for (int i = 0; i < m[0].node_list.size() ; ++i){//liste des noeuds avec leur position dans l'espace
                inp << "     " << layer_number*m[0].node_list.size() + i + 1 << ", " << m[0].node_list[i].pos[ 0 ]*pix2m << ", " << m[0].node_list[i].pos[ 1 ]*pix2m << ", " << layer_number*(thickness/n_el)<< "\n";                 
            }
            }	
        }
        else{
            for (int i = 0; i < m[0].node_list.size() ; ++i){//liste des noeuds avec leur position dans le plan
            inp << "     " << i + 1 << ", " << m[0].node_list[i].pos[ 0 ]*pix2m << ", " << m[0].node_list[i].pos[ 1 ]*pix2m << "\n";
            }
        }
            inp << "*Element, type=";
            //////////////////////////////////////////////////////////////////////////////////////////////////////////

            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (m[0].elem_list[0]->nb_nodes_virtual() == 3){
                if (computation_type == "2DPS") inp << "CPS3";
                else if (computation_type == "2DPE") inp << "CPE3";
                else if (computation_type == "3Dex") inp << "C3D6";
            }
            else if (m[0].elem_list[0]->nb_nodes_virtual() == 4){
                if (computation_type == "2DPS") inp << "CPS4";
                else if (computation_type == "2DPE") inp << "CPE4";
                else if (computation_type == "3Dex") inp << "C4D8";
            }

            inp << "\n";
        if (computation_type == "3Dex"){
            for (int layer_number =0; layer_number < (n_el); layer_number++){
              for (int i = 0; i < m[0].elem_list.size(); ++i){//liste des noeuds avec leur position dans l'espace
              inp << "     " << m[0].elem_list.size()*layer_number + i + 1 ;
              for (int jj=0; jj<m[0].elem_list[0]->nb_nodes_virtual(); jj++)
                  inp   << ", "  << m[0].elem_list[i]->node_virtual(jj)->number + 1 + layer_number*m[0].node_list.size() ;
              for (int jj=0; jj<m[0].elem_list[0]->nb_nodes_virtual(); jj++)
                  inp   << ", "  << m[0].elem_list[i]->node_virtual(jj)->number + 1 + (layer_number+1)*m[0].node_list.size() ;
              inp << "\n";
              }
            }
        }
        else{
            for (int i = 0; i < m[0].elem_list.size(); ++i){//liste des noeuds avec leur position dans l'espace
            inp << "     " << i + 1 ;
            for (int jj=0; jj<m[0].elem_list[0]->nb_nodes_virtual(); jj++)
                inp   << ", "  << m[0].elem_list[i]->node_virtual(jj)->number + 1 ;
            inp << "\n";
            }
        }
            inp << "*Nset, nset=_PickedSet5, internal, generate\n";
        if (computation_type == "3Dex") 
            inp << " 1, " << (n_el+1)*m[0].node_list.size() << ", 1\n"; 
        else
            inp << " 1, " << m[0].node_list.size() << ", 1\n";
            inp << "*Elset, elset=_PickedSet5, internal, generate\n";
        if (computation_type == "3Dex") 
            inp << " 1, " << n_el*m[0].elem_list.size() << ", 1\n";
        else
            inp << " 1, " << m[0].elem_list.size() << ", 1\n";
            inp << "*Orientation, name=Ori-1\n";
            inp << "          1.,           0.,           0.,           0.,           1.,           0.\n";
            inp << "1, 0.\n";
            inp << "** Section: Section-1-_PICKEDSET5\n";
            inp << "*Solid Section, elset=_PICKEDSET5, orientation=Ori-1, material=";
            
            if ((thelaw == "UMAT") or (thelaw == "UMAT_Lem_diccit")){
                inp << Prop_Mat[0][1] << "\n";
            }
            else if (thelaw == "dpc"){
                //////////////////////////////////////////////////////////////////////////////////////////////////////////
                inp << "MATERIAL-cg\n";
            }
            else{
                inp << "MATERIAL-1\n";
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            inp << thickness << "\n";
            inp << "*End Part\n";
            inp << "**  \n";
            inp << "** ASSEMBLY\n";
            inp << "**\n";
            inp << "*Assembly, name=Assembly\n";
            inp << "**  \n";
            inp << "*Instance, name=Part-1-1, part=Part\n";
            inp << "*End Instance\n";
            inp << "**  \n";
            for (int i = 0; i < (n_el+1)*m[0].node_list.size() ; ++i){//liste des sets de noeuds
                inp << "*Nset, nset=N_1_" << i+1 << ", instance=Part-1-1\n";
                inp << " " << i + 1 << ",\n";
            }
            inp << "*End Assembly\n";
            inp << "** \n";
            inp << "** MATERIALS\n";
            inp << "** \n";
            
            if (thelaw == "Elas_iso"){
                inp << "*Material, name=MATERIAL-1\n";
                inp << "*Elastic\n";
                inp << "" << elastic_modulus << ", " << poisson_ratio << "\n";
            }
            else if (thelaw == "Elas_ortho"){
                inp << "*Material, name=MATERIAL-1\n";
                inp << "*Elastic, TYPE=ENGINEERING CONSTANTS\n";
                inp << "" << elastic_modulus << ", " << elastic_modulus*rapport << ", " << elastic_modulus*rapport << ", " << poisson_ratio << ", " << poisson_ratio << ", " << poisson_ratio << ", " << elastic_modulus/(2 + 2* poisson_ratio) << ", " <<  elastic_modulus*rapport/(2 + 2* poisson_ratio) << "\n";
                inp << elastic_modulus*rapport/(2 + 2*poisson_ratio) << ",\n";
            }
            else if (thelaw == "RO"){
                inp << "*Material, name=MATERIAL-1\n";
                inp << "*Deformation Plasticity\n";
                double ys = 0.02; // Par défaut
                inp << "" << elastic_modulus << ", " << poisson_ratio << ", " << sigma_0 << ", " << n << ", " << ys << "\n";
            }
            else if (thelaw == "Drucker_Prager_Hardening"){
                inp << "*Material, name=MATERIAL-1\n";
                inp << "*Elastic\n";
                inp << "" << elastic_modulus << ", " << poisson_ratio << "\n";
                inp << "*Drucker Prager\n";
                inp << "" << friction_angle << ", " << flowstress_ratio << ", " << dilation_angle << "\n";
                inp << "*Drucker Prager Hardening\n";
                inp << "" << sigma_0 << ", " << eps_0 << "\n";
            }
            else if (thelaw == "Creep_Time_Hardening"){
                inp << "*Material, name=MATERIAL-1\n";
                inp << "*Elastic\n";
                inp << "" << elastic_modulus << ", " << poisson_ratio << "\n";
                inp << "*Creep, law=TIME\n";
                inp << "" << pl_multiplier << ", " << eq_stress_order << ", " << time_order << "\n";
            }
            else if (thelaw == "UMAT"){
                inp << "*Material, name=";
                inp << Prop_Mat[0][1];
                inp << "\n";
                inp << "*Depvar\n";
            std::istringstream iss(Prop_Mat[1][1]);
            iss >> nparam;
                inp << "    " << Prop_Mat[1+1+nparam][1] << ",\n";
                inp << "*User Material, constants=";
                inp << nparam;
                inp << "\n";
                int npl=0;
                for (int np=1; np<nparam; np++){
                  npl++;
                  if (npl>7){
                      inp << "\n";
                      npl=1;
                  }
                  inp << Prop_Mat[1+np][1] << ", ";
                }
                inp << Prop_Mat[1+nparam][1] << "\n";
                
            }
            else if (thelaw == "Equation"){
                inp << "*Material, name=MATERIAL-1\n";
                inp << "*Elastic\n";
                inp << "" << elastic_modulus << ", " << poisson_ratio << "\n";
                inp << "*Plastic\n";
                for (int numeps = 0; numeps<neps; numeps++){
                    inp << "<sig" << numeps << ">,  " << eps[numeps] << "\n";
                }
            }
            else if (thelaw == "UMAT_Lem_diccit"){
                inp << "*Material, name=";
                inp << Prop_Mat[0][1];
                inp << "\n";
                inp << "*Depvar\n";
            std::istringstream iss(Prop_Mat[1][1]);
            iss >> nparam;
                inp << "    " << Prop_Mat[1+1+nparam][1] << ",\n";
                inp << "    	 1,EpsE11\n";
                inp << "    	 2,EpsE22\n";
                inp << "    	 3,EpsE33\n";
                inp << "    	 4,EpsE12\n";
                inp << "    	 5,EpsE13\n";
                inp << "    	 6,EpsE23\n";
                inp << "    	 7,EpsP11\n";
                inp << "    	 8,EpsP22\n";
                inp << "    	 9,EpsP33\n";
                inp << "    	 10,EpsP12\n";
                inp << "    	 11,EpsP13\n";
                inp << "    	 12,EpsP23\n";
                inp << "    	 13,p\n";
                inp << "    	 14,r\n";
                inp << "    	 15,gR\n";
                inp << "    	 16,D \n";
                inp << "    	 17,EpsCr\n";
                inp << "    	 18,Tx\n";
                inp << "*DENSITY\n";
                inp << "2.7e-9\n";
                inp << "*User Material, constants=66\n";
                inp << "<E>,        <Nu>,     <sig0>,     <eps0>,  <sig1>,     <eps1>,    <sig2>,     <eps2>, \n";
                inp << "<sig3>,    <eps3>, <sig4>,    <eps4>,   <sig5>,     <eps5>,    <sig6>,     <eps6>,   \n";
                inp << "<sig7>,    <eps7>,  <sig8>,    <eps8>,   <sig9>,     <eps9>,    <sig10>,     <eps10>,   \n";
                inp << "<sig11>,    <eps11>, <sig12>,    <eps12>,   <sig13>,     <eps13>,    <sig14>,     <eps14>,   \n";
                inp << "<sig15>,    <eps15>, <sig16>,    <eps16>,   <sig17>,     <eps17>,    <sig18>,     <eps18>,   \n";
                inp << "<sig19>,    <eps19>, <sig20>,    <eps20>,   <sig21>,     <eps21>,    <sig22>,     <eps22>,   \n";
                inp << "<sig23>,    <eps23>, <sig24>,    <eps24>,   <sig25>,     <eps25>,    <sig26>,     <eps26>,   \n";
                inp << "<sig27>,    <eps27>, <sig28>,    <eps28>,   <sig29>,     <eps29>,    <pth>,       <s>,       \n";
                inp << "<S>,        <Dcr>, <mDam>,     <eDam>\n";
          
        }
        else if (thelaw == "dpc"){/*
            
            std::string nom_fic_param_dpc = "dpc_param_tmp.txt";
            double eps_crit = load_param(nom_fic_param_dpc,"eps_crit");
            double min_eps = load_param(nom_fic_param_dpc,"min_eps");
            double mid_min_eps = load_param(nom_fic_param_dpc,"mid_min_eps");
            double mid_max_eps = load_param(nom_fic_param_dpc,"mid_max_eps");
            double max_eps = load_param(nom_fic_param_dpc,"max_eps");
            double a2 = load_param(nom_fic_param_dpc,"a2");
            double b2 = load_param(nom_fic_param_dpc,"b2");
            double n2 = load_param(nom_fic_param_dpc,"n2");
            double tan_beta = load_param(nom_fic_param_dpc,"tan_beta");
            double da_a = load_param(nom_fic_param_dpc,"da_a");
            double da_b = load_param(nom_fic_param_dpc,"da_b");
            double da_c = load_param(nom_fic_param_dpc,"da_c");
            double r_a_min = load_param(nom_fic_param_dpc,"r_a_min");
            double r_a_max = load_param(nom_fic_param_dpc,"r_a_max");
            double k_p = load_param(nom_fic_param_dpc,"k_p");
            double k_crit = load_param(nom_fic_param_dpc,"k_crit");
            double a_k = load_param(nom_fic_param_dpc,"a_k");
            double g_p = load_param(nom_fic_param_dpc,"g_p");
            double g_crit = load_param(nom_fic_param_dpc,"g_crit");
            double a_g = load_param(nom_fic_param_dpc,"a_g");
            
            Vec<double> eps_vol_plas, p_ab, d_ab, tan_beta_ab, r_ab, k_ab, g_ab, e_ab, nu_ab  ;
            int npt1 = 10;
            int npt2 = 60;
            for (int pt=0; pt<npt1; pt++){
                eps_vol_plas << min_eps + pt * (eps_crit-mid_min_eps-min_eps)/(npt1-1);
        }
        for (int pt=0; pt<npt2; pt++){
            eps_vol_plas << eps_crit-mid_max_eps + pt * (mid_max_eps-max_eps)/(npt2-1);
        }
        
        for (int pt=0; pt<eps_vol_plas.size(); pt++){
            p_ab << a2 * pow( log( b2 / (eps_crit-eps_vol_plas[pt]) ) , n2) ;
            d_ab << da_a * exp( -da_b * (eps_crit-eps_vol_plas[pt]) ) + da_c ;
            if (pt == 0) d_ab[pt] = 0.2596;
            else if (pt == 1) d_ab[pt] = 0.3718;
            else if (pt == 2) d_ab[pt] = 0.5325;
            tan_beta_ab << tan_beta ;
            if (pt < 20)
                r_ab << r_a_min;
            else
                r_ab << r_a_max;
            k_ab << k_p + (k_crit - k_p) * exp ( -a_k * (eps_crit - eps_vol_plas[pt]) ) ;
            g_ab << g_p + (g_crit - g_p) * exp ( -a_g * (eps_crit - eps_vol_plas[pt]) ) ;
            e_ab << ( 9 * k_ab[pt] * g_ab[pt] ) / ( 3 * k_ab[pt] + g_ab[pt] ) ;
            double nu_tmp = ( 3 * k_ab[pt] - 2 * g_ab[pt] ) / ( 6 * k_ab[pt] + 2 * g_ab[pt] ) ;
            if (pt>0) {
                if (nu_tmp > nu_ab[pt-1])
                    nu_ab << nu_tmp;
                else
                    nu_ab << nu_ab[pt-1];
        }
        else
            nu_ab << nu_tmp;
        }
        eps_vol_plas[0]=0;
        
        inp << "*Material, name=MATERIAL-cg\n";
        inp << "*Cap Plasticity, dependencies=1\n";
        for (int pt =0; pt < eps_vol_plas.size(); pt++){
            inp << d_ab[pt] << ", " << tan_beta_ab[pt] << ", " << r_ab[pt] << ", 0.0, 0.0, 1.0, , " << eps_vol_plas[pt]  << "\n";
        }
        inp << "*Cap Hardening, dependencies=1\n";
        for (int pt =0; pt < eps_vol_plas.size(); pt++){
            inp << p_ab[pt] << ", " << eps_vol_plas[pt]  << "\n";
        }
        inp << "*Depvar\n";
        inp << "2,\n";
        inp << "*Elastic, dependencies=1\n";
        for (int pt =0; pt < eps_vol_plas.size(); pt++){
            inp << e_ab[pt] << ", " << nu_ab[pt] << ", " << eps_vol_plas[pt]  << "\n";
        }
        inp << "*User Defined Field\n";*/
        
        }
        
        for (int i=0; i < m.size(); ++i  ){// steps
            inp << "** \n";
            //inp << "** STEP: Step-" << i+1  << ", inc=11000\n";
            inp << "** STEP: Step-" << i+1  << "\n";
            inp << "** \n";
	    
//             inp << "*Step, NLGEOM, name=Step-" << i+1 << "\n";
//             //inp << "*Step, name=Step-" << i+1 << "\n";
//             inp << "*Static\n"; 
// 	    inp << "0.01, 1., 0.0001, 0.1\n";
// 	    //inp << "0.01, 1., 1e-05, 0.01\n";
//             inp << "** \n";
//             inp << "** BOUNDARY CONDITIONS\n";
	    
	    inp << "*Step, NLGEOM=YES, name=Step-" << i+1 << ", inc=1000000\n";
        if (thelaw == "Creep_Time_Hardening"){
            inp << "*Visco, CETOL=0.0001\n";
        }
        else{
            inp << "*Static\n";
        }
        inp << "0.1, 1., 1e-07, 1.\n";
        inp << "*CONTROLS, PARAMETERS=TIME INCREMENTATION\n";
        inp << ", , , , , , , 10000, , ,\n";
        inp << "0.5, , , , , , 2.5, \n";
	    inp << "** \n";
	    inp << "** BOUNDARY CONDITIONS\n";

	    if (computation_type == "3Dex"){ 
		int nbc=/*TM::dim*/2*i*constrained_nodes.size();
		if (i>0){
		    for (int j=0; j<constrained_nodes.size(); ++j ){// CL
			for( int d = 0; d < /*TM::dim*/3; ++d ){//direction
			    ++nbc;
			    inp << "** Name: BC-" << nbc - /*TM::dim*/2*constrained_nodes.size() <<" Type: Displacement/Rotation\n";
			    inp << "*Boundary, op=NEW\n";
			}
		    }
		}
		
		for (int layer_number =0; layer_number < (n_el+1); layer_number++){
		    for (int j=0; j<constrained_nodes.size(); ++j ){// CL
			for( int d = 0; d < /*TM::dim*/2; ++d ){//direction
			    ++nbc;
			    inp << "** Name: BC-" << nbc - /*TM::dim*/2*constrained_nodes.size() <<" Type: Displacement/Rotation\n";
			    inp << "*Boundary";
			    if (i>0){
				inp << ", op=NEW";
			    }
			    inp << "\n";
			    inp << "N_1_" << layer_number*m[i].node_list.size() + constrained_nodes[j] + 1 << ", " << d+1 << ", " << d+1 << ", " << m[ i ].node_list[ constrained_nodes[ j ] ].dep[d]*pix2m << "\n";
			}
			++nbc;
			inp << "** Name: BC-" << nbc - /*TM::dim*/2*constrained_nodes.size() <<" Type: Displacement/Rotation\n";
			inp << "*Boundary";
			if (i>0){
			    inp << ", op=NEW";
			}
			inp << "\n";
			inp << "N_1_" << layer_number*m[i].node_list.size() + constrained_nodes[j] + 1 << ", 3, 3, 0\n";
		    }
		}
	    }
            else{
		int nbc=/*TM::dim*/2*i*constrained_nodes.size();
		if (i>0){
		    for (int j=0; j<constrained_nodes.size(); ++j ){// CL
			for( int d = 0; d < /*TM::dim*/2; ++d ){//direction
			    ++nbc;
			    inp << "** Name: BC-" << nbc - /*TM::dim*/2*constrained_nodes.size() <<" Type: Displacement/Rotation\n";
			    inp << "*Boundary, op=NEW\n";
			}
		    }
		}
		for (int j=0; j<constrained_nodes.size(); ++j ){// CL
		    for( int d = 0; d < /*TM::dim*/2; ++d ){//direction
			++nbc;
			inp << "** Name: BC-" << nbc - /*TM::dim*/2*constrained_nodes.size() <<" Type: Displacement/Rotation\n";
			inp << "*Boundary";
			if (i>0){
			    inp << ", op=NEW";
			}
			inp << "\n";
			inp << "N_1_" << constrained_nodes[j] + 1 << ", " << d+1 << ", " << d+1 << ", " << m[ i ].node_list[ constrained_nodes[ j ] ].dep[d]*pix2m << "\n";
		    }
		}
	    }
            inp << "** \n";
            inp << "** \n";
            inp << "** OUTPUT REQUESTS\n";
            inp << "** \n";
            inp << "*Restart, write, frequency=0\n";
            inp << "** \n";
            inp << "** FIELD OUTPUT: F-Output-" << i+1 << "\n";
            inp << "** \n";
            inp << "*Output, field";
            if (thelaw == "UMAT"){
                inp << "\n";
                inp << "*Node Output\n";
                inp << "CF, RF, U\n";
                inp << "*Element Output, directions=YES\n";
                inp << "E, S, SDV\n";
            }
            else {
                inp << ", variable=PRESELECT\n";
            }
            inp << "** \n";
            inp << "** HISTORY OUTPUT: H-Output-" << i+1 << "\n";
            inp << "** \n";
            inp << "*Output, history, variable=PRESELECT\n";
            inp << "** \n";
            inp << "*End Step\n";
            inp << "** \n";
        }
    }
    
    std::remove((root_dir +".odb").c_str());
    std::remove((root_dir +".sta").c_str());
    std::remove((root_dir +".sim").c_str());
    std::remove((root_dir +".prt").c_str());
    std::remove((root_dir +".dat").c_str());
    std::remove((root_dir +".msg").c_str());
}

// Loads a abq result from an odb file to a vec(LMT::Mesh). Doesn't create the mesh : supposed to be known (same and elements node numbers than the result : this part is commented in the beginning). 
void load_abq_res_odb(std::string nom_fic, Vec<TM> &res){
  
    std::cout << " " << std::endl;
    std::cout << "Reading ";
    std::cout << nom_fic.c_str() << std::endl;
    
    odb_initializeAPI();  // pour initialiser l'interface c++ abaqus il faut impérativement appeler cette fonction avant de faire appel à  d'autre fonctions
    odb_Odb& odb = openOdb( nom_fic.c_str() );// appel de la fonction openOdb qui permet d'ouvrir un fichier abaqus odb
    odb_Assembly& rootAssy = odb.rootAssembly();
    odb_InstanceRepositoryIT instIter( rootAssy.instances() );
    for ( instIter.first(); !instIter.isDone(); instIter.next() )
        std::cout << instIter.currentKey().CStr() << std::endl;
    
    // Lecture des coordonnées des noeuds
    odb_Instance& instance1 =rootAssy.instances()[ "PART-1-1" ];
    odb_Enum::odb_DimensionEnum instanceType = instance1.embeddedSpace();
    const odb_SequenceNode& nodeList = instance1.nodes();
    int nodeListSize = nodeList.size();
    
    std::cout << nodeListSize << std::endl;
            
    Vec<int> face_nodes;
    //res[0].node_list.resize(nodeList.size());
    if ( instanceType == odb_Enum::THREE_D ) {
        for ( int n = 0; n < nodeListSize; n++ ) {
            const odb_Node node      = nodeList[ n ];
            int nodeLabel            = node.label();
            const float* const coord = node.coordinates();
            //   res[0].node_list[n].pos = node.coordinates();
	    if (coord[2] == 0) face_nodes << n;
        }
    }
    else if ( ( instanceType == odb_Enum::TWO_D_PLANAR ) || ( instanceType == odb_Enum::AXISYMMETRIC ) ) {
        for ( int n = 0; n < nodeListSize; n++) {
            const odb_Node node      = nodeList[ n ];
            int nodeLabel            = node.label();
            const float* const coord = node.coordinates();
            //res[0].node_list[n].pos = node.coordinates();
        }
    }
    
    
    // Lecture des steps
    odb_StepRepositoryIT stepIter( odb.steps() );
    int nstep = 0;
    for (stepIter.first(); !stepIter.isDone(); stepIter.next()){
        nstep +=1;
    }
    
    
    if (res.size() != 1){
        res.resize( nstep );
        for( int i = 1; i < res.size(); ++i ) { // INITIALISATION
            res[i]=res[0];
        }
    }
    int num_step = 0;
    for (stepIter.first(); !stepIter.isDone(); stepIter.next()) {
      
        //std::cout << " " << std::endl;
        //std::cout << stepIter.currentKey().CStr()  << std::endl; // Nom du step
        //std::cout << " " << std::endl;
        
        odb_Step& step = odb.steps()[stepIter.currentKey().CStr()];
        odb_SequenceFrame& allFramesInStep = step.frames();
        int numFrames = allFramesInStep.size();
        //std::cout << to_string(numFrames) << std::endl;
        odb_Frame& lastFrame = allFramesInStep[numFrames-1];
        
        odb_FieldOutputRepository& fieldOutputRep = lastFrame.fieldOutputs();
        odb_FieldOutputRepositoryIT fieldIter( fieldOutputRep );
        //
        for  (fieldIter.first(); !fieldIter.isDone(); fieldIter.next()) { // inutile ?
            //          std::cout << fieldIter.currentKey().CStr() << std::endl;
            odb_FieldOutput& field = fieldOutputRep[fieldIter.currentKey()];
            //const odb_SequenceFieldValue& seqVal = field.values();
            const odb_SequenceFieldLocation& seqLoc = field.locations();
            //          std::cout << field.name().CStr() << " : " << field.description().CStr() << std::endl;
            //          std::cout << "  Type: " << field.type() << std::endl;
            int numLoc = seqLoc.size();
            for (int loc = 0; loc<numLoc; loc++){
                //                  std::cout << "Position: "<<seqLoc.constGet(loc).position();
            }
            //          std::cout << std::endl;
        }
        
        const odb_SequenceFieldValue& displacements = lastFrame.fieldOutputs()["U"].values();
        const odb_SequenceFieldValue& reac_forces = lastFrame.fieldOutputs()["RF"].values();
	
	// NODAL VALUES
        int numValues = displacements.size();
        int numComp = 0;
	//int n_layers = displacements.size()/face_nodes.size();
	//if (face_nodes.size()>0) numValues = face_nodes.size();
	
        for (int i=0; i<numValues; i++) {
            const odb_FieldValue valU = displacements[i];
            const odb_FieldValue valRF = reac_forces[i];
            const float* const U = valU.data(numComp);
            const float* const RF = valRF.data(numComp);
            bool face;
            if ( instanceType == odb_Enum::THREE_D ) {
                const odb_Node node      = nodeList[ i ];
                const float* const coord = node.coordinates();
                if (coord[2] == 0) face = 1;
                else face = 0;
                for (int comp=0; comp < numComp; comp++) {
                    if (face)
                        res[num_step].node_list[i].dep[comp] = U[comp];
                    double noeud = i;
                    int num_layer = floor(noeud/face_nodes.size());
                    res[num_step].node_list[i-num_layer*face_nodes.size()].f_nodal[comp] += RF[comp];
                }
            }
                else{
                    for (int comp=0; comp < numComp; comp++) {
                        res[num_step].node_list[i].dep[comp] = U[comp];
                        res[num_step].node_list[i].f_nodal[comp] += RF[comp];
                    }
                }
        }
        
//         // ELEMENT VALUE
// 	odb_FieldOutput& stress = lastFrame.fieldOutputs()["S"];
// 	odb_FieldOutput& strain = lastFrame.fieldOutputs()["E"];
// 	const odb_SequenceFieldBulkData& seqStressBulkData = stress.bulkDataBlocks();
// 	const odb_SequenceFieldBulkData& seqStrainBulkData = strain.bulkDataBlocks();
// 	int numStressBlocks = seqStressBulkData.size();
// 	int numStrainBlocks = seqStrainBulkData.size();
// 	
// 	for (int jblock=0; jblock<numStressBlocks; jblock++) {
// 	    const odb_FieldBulkData& bulkData = seqStressBulkData[jblock];
// 	    int numValues = bulkData.length();
// 	    int numComp = bulkData.width();
// 	    float* data = bulkData.data();
// 	    int nElems = bulkData.numberOfElements();
// 	    int numIP = numValues/nElems;
// 	    int* elementLabels = bulkData.elementLabels();
// 	    int* integrationPoints = bulkData.integrationPoints();
// 	    const odb_SectionPoint& myBulkSectionPoint =  bulkData.sectionPoint();
// 	    int sectPoint = myBulkSectionPoint.number();
// 	    
// 	    for (int elem = 0, ipPosition=0, dataPosition=0; elem<numValues; elem+=numIP) {
// 		for (int ip = 0; ip<numIP; ip++) {
// 		   Vec<double> components ;
// 		   double c0 = data[dataPosition];
// 		   double c1 = data[dataPosition+1];
// 		   double c2 = data[dataPosition+3];
// 		   res[num_step].elem_list[elem*numIP+ip]->set_field( "sigma", Vec<double,3>(c0, c1, c2));
// 		   for (int comp = 0; comp<numComp; comp++){
// 		      dataPosition++;
// 		   }
// 		}
// 	    }
// 	}
// 	for (int jblock=0; jblock<numStrainBlocks; jblock++) {
// 	    const odb_FieldBulkData& bulkData = seqStrainBulkData[jblock];
// 	    int numValues = bulkData.length();
// 	    int numComp = bulkData.width();
// 	    float* data = bulkData.data();
// 	    int nElems = bulkData.numberOfElements();
// 	    int numIP = numValues/nElems;
// 	    int* elementLabels = bulkData.elementLabels();
// 	    int* integrationPoints = bulkData.integrationPoints();
// 	    const odb_SectionPoint& myBulkSectionPoint =  bulkData.sectionPoint();
// 	    int sectPoint = myBulkSectionPoint.number();
// 	    
// 	    for (int elem = 0, ipPosition=0, dataPosition=0; elem<numValues; elem+=numIP) {
// 		for (int ip = 0; ip<numIP; ip++) {
// 		   Vec<double> components ;
// 		   double c0 = data[dataPosition];
// 		   double c1 = data[dataPosition+1];
// 		   double c2 = data[dataPosition+3];
// 		   res[num_step].elem_list[elem*numIP+ip]->set_field( "epsilon", Vec<double,3>(c0, c1, c2));
// 		   for (int comp = 0; comp<numComp; comp++){
// 		      dataPosition++;
// 		   }
// 		}
// 	    }
// 	}
        
        num_step += 1;
    }

    odb.close();
    odb_finalizeAPI();
    std::cout << " " << std::endl;
    
}

// Lauch an Abaqus computation and gets the result, based on a LMT::Mesh (and its displacements for boundary conditions)
Vec<TM> calc_abq_into_LMTppMesh(Vec<TM> &m_ref, Vec<double> constrained_nodes, double pix2m, Vec < Vec < std::string > > Prop_Mat , double thickness){
     
    Vec<TM> Vecteur_de_maillages_output = m_ref;
    char* HomeDir;
    HomeDir = getenv ("HOME");
    std::string root_dir = std::string(HomeDir) + "/aaa_test";
    
    Write2DINP (m_ref, root_dir, constrained_nodes, pix2m, Prop_Mat, thickness);
    
    std::string nom_fic_lck = root_dir + ".lck";if (exists(nom_fic_lck)) remove(nom_fic_lck.c_str());
    std::string nom_fic_odb = root_dir + ".odb";if (exists(nom_fic_odb)) remove(nom_fic_odb.c_str());
    std::string thelaw = Prop_Mat[0][1];
    
    char* UserName;
    UserName = getenv ("USER");
    std::string scratch = root_dir + "/";
    std::string scratch2 = root_dir + "/" + std::string(UserName) + "_" + std::string(HomeDir) +  "/";
    bool success = create_dir(scratch2);
    
    if ((thelaw == "UMAT") or (thelaw == "UMAT_Lem_diccit")){
        std::string umatname = Prop_Mat[Prop_Mat.size()-2][1];
        //system(("abaqus interactive job=" + root_dir + ".inp user=" + umatname + ".f double > res_s.txt").c_str() );
        std::cout << "/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" << root_dir << ".inp scratch=" << scratch << " user=" << umatname << " double" << std::endl;
         int vide = system(("/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" + root_dir + ".inp scratch=" + scratch + " user=" + umatname + " double").c_str() );
    }
    else if (thelaw == "dpc"){
        //system(("abaqus interactive job=" + root_dir + ".inp user=elasticity_variation_epsvol.f double > res_s.txt").c_str() );
        int vide = system(("/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" + root_dir + ".inp user=elasticity_variation_epsvol.f double").c_str() );
    }
    else{
        //system(("abaqus interactive job=" + root_dir + ".inp cpus=6 double > res_s.txt").c_str() );
        std::cout << "/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" << root_dir << ".inp cpus=8 scratch=" << scratch << std::endl;
        int vide = system(("/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" + root_dir + ".inp cpus=8 scratch=" + scratch).c_str() );
    }
    
    std::string nom_fic = root_dir + ".odb";
    //Vec<TM> Vecteur_de_maillages_output = load_abq_res_odb(nom_fic, m_ref);
    
    load_abq_res_odb(nom_fic, Vecteur_de_maillages_output);
    
    return Vecteur_de_maillages_output;
}
