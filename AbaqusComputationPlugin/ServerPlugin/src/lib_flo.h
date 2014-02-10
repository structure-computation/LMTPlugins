#ifndef	LIB_FLO_H
#define	LIB_FLO_H

//#include <mesh/make_mesh_from_mask.h>
//#include <containers/eig_lapack.h>
//#include <correlation/DicCPU.h>
#include <stdlib.h>
#include <dirent.h>
//#include "header_abq.h" // pour interfacer c++ avec abaqus
//#include <odb_API.h> // pour les fonctions odb d'abaqus
#include <iostream>
using namespace LMT;
typedef ImgInterp<double,2> TI;
typedef Mesh<Mesh_carac_Ramberg_Osgood<double,2> > TM;
typedef Formulation<TM,Ramberg_Osgood> TF;
typedef TM::Pvec Pvec;

struct StepDer {
    int nb_steps(std::string thelaw, int umat_nparamid) const {

        int numparam2id;
        if (thelaw == "Elas_iso") numparam2id = 1 ;
        if (thelaw == "Elas_ortho") numparam2id = 1 ;
        else if (thelaw == "RO") numparam2id = 2 ;
        else if (thelaw == "UMAT") numparam2id=umat_nparamid;
        return numparam2id;

    }
    
    template<class TM> void operator()( TM &m, int num_step ) const {
        m.n_sens = num_step-- == 0;
        m.sigma_0_sens = num_step-- == 0;
        // m.poisson_ratio_sens = num_step-- == 0;
        //PRINT(num_step);
        if ( num_step >= 0 ) {
            m.node_list[ constrained_nodes[ num_step / 2 ] ].dep_imp_coef[ 0 ] = num_step % 2 == 0;
            m.node_list[ constrained_nodes[ num_step / 2 ] ].dep_imp_coef[ 1 ] = num_step % 2 == 1;
        }
    }
    
    template<class TM> void add_delta_derivatives( TM &m, int num_step, double val ) const {
        m.n += ( num_step-- == 0 ) * val;
        m.sigma_0 += ( num_step-- == 0 ) * val;
        // m.poisson_ratio += ( num_step-- == 0 ) * val;
        if ( num_step >= 0 ) {
            m.node_list[ constrained_nodes[ num_step / 2 ] ].dep_imp_val[ 0 ] = val * ( num_step % 2 == 0 );
            m.node_list[ constrained_nodes[ num_step / 2 ] ].dep_imp_val[ 1 ] = val * ( num_step % 2 == 1 );
        }
    }
    Vec<int> constrained_nodes;
};

struct GetSig {
    template<class TE>
    void operator()( const TE &e ) {
        for( int i = 0; i < 3; ++i )
            s << e.sigma_p[ i ];
    }
    Vec<double> s;
};

struct SetSpToZ {
    template<class TE>
    void operator()( TE &e ) const {
        e.sigma_p = 0;
    }
};

struct GetEq {
    template<class TE>
    void operator()( const TE &el ) {
        s << el.sigma_eq;
        e << el.epsilon_eq;
    }
    Vec<double> s, e;
};


bool exists( const std::string & file ){
    std::ifstream fichier( file.c_str() );
    return !fichier.fail();
}

double count_lines(std::string nom_fic){
    std::string numlines;
    system("touch res_system.txt");
    system(("wc -l " + nom_fic + "> res_system.txt").c_str());
    std::ifstream fichier("res_system.txt");
    std::string res;
    std::getline(fichier, res);
    for (int i = 0; i<(res.size()-nom_fic.size()-1);i++) numlines+=res[i];
    int numlines_int=atoi(numlines.c_str());
    return numlines_int;
}

double load_param(std::string nom_fic, std::string nom_param){
    std::ifstream strim(nom_fic.c_str());
    std::string ligne;
    double param;
    int numlines=count_lines(nom_fic);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        if (ligne==nom_param){
            std::getline(strim, ligne);
            param = atof(ligne.c_str());
            break;
        }
    }
    return param;
}

std::string load_param_str(std::string nom_fic, std::string nom_param){
    std::ifstream strim(nom_fic.c_str());
    std::string ligne;
    std::string param;
    int numlines=count_lines(nom_fic);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        if (ligne==nom_param){
            std::getline(strim, ligne);
            param = ligne;
            break;
        }
    }
    return param;
}

Vec<double> load_res(std::string nom_fic){
    std::ifstream strim(nom_fic.c_str());
    std::string ligne;
    Vec<double> resultat;
    
    int numlines=count_lines(nom_fic);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        resultat.push_back(atof(ligne.c_str()));
    }
    return resultat;
}

void write_mat (Mat<double> M, std::string name){
    std::ofstream mat (name.c_str());
    for (int ii = 0; ii < M.col(0).size(); ii++){
        for (int jj = 0; jj < M.row(0).size(); jj++){
            mat << double(M(ii,jj)) << " " ;
        }
        mat << "\n";
    }
}

void write_vec (Vec<double> V, std::string name){
    std::ofstream mat (name.c_str());
    for (int ii = 0; ii < V.size(); ii++){
        mat << double(V[ii]) << "\n" ;
    }
}

void add_to_file(std::string newfilename, std::string to_add){

    std::ofstream ofstrim(newfilename.c_str(), std::ios::app);
    int numlines=count_lines(to_add);
    std::ifstream ifstrim(to_add.c_str());
    for (int ii=0; ii<numlines; ii++){
        std::string ligne;
        std::getline(ifstrim, ligne);
        ofstrim << ligne << "\n";
    }
    ofstrim.close();

}

Mat<double,Sym<>,SparseLine<> > load_mat(std::string nom_fic){
    std::ifstream strim(nom_fic.c_str());
    std::string ligne;
    Mat<double,Sym<>,SparseLine<> > resultat;

    int numlines=count_lines(nom_fic);
    resultat.resize(numlines);
    for (int i = 0; i<numlines; i++){
        std::getline(strim, ligne);
        int kk=-1;
        int jj=0;
        std::string mot="";
        for (int ii = 0; ii<ligne.size(); ii++){

            kk++;

            if ( (ligne.compare(ii,1," ") == 0) or (ii==ligne.size()-1)) {
                std::string tmp = ligne.substr(ii-kk,kk);
                kk=0;
                resultat(i,jj)=atof(tmp.c_str());
                jj++;
            }

        }
    }
    return resultat;
}

void WriteINP (Vec<TM> &m, std::string racine_fic, std::string nom_fic_param, Vec<double> constrained_nodes, double pix2m){
    std::string nom_fic = ( racine_fic + ".inp" );
    for (int z = 0; z <1; ++z) { // fausse boucle pour écrire l'inp

        std::string thelaw = load_param_str(nom_fic_param,"thelaw");
        if (exists(nom_fic)) remove(nom_fic.c_str());
        system(("touch " + nom_fic).c_str());
        if (thelaw == "UMAT"){
            std::string umatname = load_param_str(nom_fic_param,"umatname");
            std::string nom_fic_param_umat = umatname + "_param.txt";
            double preamble = load_param(nom_fic_param_umat, "umat_preamble");

            if (preamble) {
                add_to_file(nom_fic, umatname + "_1.txt");
            }
        }

        std::ofstream inp( nom_fic.c_str() , std::ios::app);
        inp << "*Heading\n";
        inp << "** Job name: test_florent Model name: Model-1\n";
        inp << "*Preprint, echo=NO, model=NO, history=NO, contact=NO\n";
        inp << "**\n";
        inp << "** PARTS\n";
        inp << "**\n";
        inp << "*Part, name=Part\n";
        inp << "*Node\n";
        for (int i = 0; i < m[0].node_list.size() ; ++i){//liste des noeuds avec leur position dans l'espace
            inp << "     " << i + 1 << ", " << m[0].node_list[i].pos[ 0 ]*pix2m << ", " << m[0].node_list[i].pos[ 1 ]*pix2m << "\n";
        }
        inp << "*Element, type=";

        std::string def_mode = load_param_str(nom_fic_param,"def_mode");
        if (def_mode == "CP") inp << "CPS3";
        else if (def_mode == "DP") inp << "CPE3";

        inp << "\n";
        for (int i = 0; i < m[0].elem_list.size(); ++i){//liste des noeuds avec leur position dans l'espace
            inp << "     " << i + 1 << ", " << m[0].elem_list[i]->node_virtual(0)->number + 1 << ", " << m[0].elem_list[i]->node_virtual(1)->number + 1 << ", " << m[0].elem_list[i]->node_virtual(2)->number + 1 << "\n";
        }
        inp << "*Nset, nset=_PickedSet5, internal, generate\n";
        inp << " 1, " << m[0].node_list.size() << ", 1\n";
        inp << "*Elset, elset=_PickedSet5, internal, generate\n";
        inp << " 1, " << m[0].elem_list.size() << ", 1\n";
        inp << "*Orientation, name=Ori-1\n";
        inp << "	  1.,           0.,           0.,           0.,           1.,           0.\n";
        inp << "1, 0.\n";
        inp << "** Section: Section-1-_PICKEDSET5\n";
        inp << "*Solid Section, elset=_PICKEDSET5, orientation=Ori-1, material=";

        if (thelaw == "Elas_iso"){
            inp << "MATERIAL-1\n";
        }
        if (thelaw == "Elas_ortho"){
            inp << "MATERIAL-1\n";
        }
        else if (thelaw == "RO"){
            inp << "MATERIAL-1\n";
        }
        else if (thelaw == "UMAT"){
            std::string umatname = load_param_str(nom_fic_param,"umatname");
            inp << umatname << "\n";
        }
        else if (thelaw == "dpc"){
            inp << "MATERIAL-cg\n";
        }
        std::string thickness = load_param_str(nom_fic_param,"thickness");
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
        for (int i = 0; i < m[0].node_list.size() ; ++i){//liste des sets de noeuds
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
            inp << "" << m[ 0 ].elastic_modulus << ", " << m[0].poisson_ratio << "\n";
        }
        else if (thelaw == "Elas_ortho"){
            double rapport = load_param(nom_fic_param,"rapport");
            inp << "*Material, name=MATERIAL-1\n";
            inp << "*Elastic, TYPE=ENGINEERING CONSTANTS\n";
            inp << "" << m[ 0 ].elastic_modulus << ", " << m[ 0 ].elastic_modulus*rapport << ", " << m[ 0 ].elastic_modulus*rapport << ", " << m[0].poisson_ratio << ", " << m[0].poisson_ratio << ", " << m[0].poisson_ratio << ", " << m[ 0 ].elastic_modulus/(2 + 2*m[0].poisson_ratio) << ", " << m[ 0 ].elastic_modulus*rapport/(2 + 2*m[0].poisson_ratio) << "\n";
            inp << m[ 0 ].elastic_modulus*rapport/(2 + 2*m[0].poisson_ratio) << ",\n";
        }
        else if (thelaw == "RO"){
            inp << "*Material, name=MATERIAL-1\n";
            inp << "*Deformation Plasticity\n";
            double ys = load_param("ys.txt","ys");
            inp << "" << m[ 0 ].elastic_modulus << ", " << m[0].poisson_ratio << ", " << m[0].sigma_0 << ", " << m[0].n << ", " << ys << "\n";
        }
        else if (thelaw == "UMAT"){
            std::string umatname = load_param_str(nom_fic_param,"umatname");
            //std::string nom_fic_param_umat = umatname + "_param_tmp.txt";
            //int umat_nparam = load_param(nom_fic_param_umat,"umat_nparam");
            //int umat_ndepvar = load_param(nom_fic_param_umat,"umat_ndepvar");
            inp << "*Material, name=";
            inp << umatname;
            inp << "\n";

            //inp << "*Depvar\n";
            //inp << "    " << umat_ndepvar << ",\n";
            //inp << "*User Material, constants=";
            //inp << umat_nparam;
            //inp << "\n";
            //int npl=0;
            //for (int np=1; np<umat_nparam; np++){
            //    npl++;
            //    if (npl>7){
            //        inp << "\n";
            //        npl=1;
            //    }
            //    std::string param_name = "param" + to_string(np);
            //    std::string parameter = load_param_str(nom_fic_param_umat, param_name);
            //    inp << parameter << ", ";
            //}
            //std::string param_name = "param" + to_string(umat_nparam);
            //std::string parameter = load_param_str(nom_fic_param_umat, param_name);
            //inp << parameter << "\n";

            inp.close();
            add_to_file(nom_fic, umatname + "_mat.txt");
            inp.open(nom_fic.c_str(), std::ios::app);
        }
        else if (thelaw == "dpc"){

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
            inp << "*User Defined Field\n";

        }

        for (int i=0; i < m.size(); ++i  ){// steps
            inp << "** \n";
            inp << "** STEP: Step-" << i+1 << "\n";
            inp << "** \n";
            inp << "*Step, name=Step-" << i+1 << "\n";
            inp << "*Static\n";
            inp << "0.0001, 1, 1e-03, 0.7\n";
            inp << "** \n";
            inp << "** BOUNDARY CONDITIONS\n";
            int nbc=TM::dim*i*constrained_nodes.size();
            if (i>0){
                for (int j=0; j<constrained_nodes.size(); ++j ){// CL
                    for( int d = 0; d < TM::dim; ++d ){//direction
                        ++nbc;
                        inp << "** Name: BC-" << nbc - TM::dim*constrained_nodes.size() <<" Type: Displacement/Rotation\n";
                        inp << "*Boundary, op=NEW\n";
                    }
                }
            }
            for (int j=0; j<constrained_nodes.size(); ++j ){// CL
                for( int d = 0; d < TM::dim; ++d ){//direction
                    ++nbc;
                    inp << "** Name: BC-" << nbc - TM::dim*constrained_nodes.size() <<" Type: Displacement/Rotation\n";
                    inp << "*Boundary";
                    if (i>0){
                        inp << ", op=NEW";
                    }
                    inp << "\n";
                    inp << "N_1_" << constrained_nodes[j] + 1 << ", " << d+1 << ", " << d+1 << ", " << m[ i ].node_list[ constrained_nodes[ j ] ].dep[d]*pix2m << "\n";

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
    remove((racine_fic +".odb").c_str());
    remove((racine_fic +".sta").c_str());
    remove((racine_fic +".sim").c_str());
    remove((racine_fic +".prt").c_str());
    remove((racine_fic +".dat").c_str());
    remove((racine_fic +".msg").c_str());
}

void WriteExtract (std::string racine_fic){
    std::string nom_fic = ( racine_fic + ".py" );
    for (int z = 0; z <1; ++z) { // fausse boucle pour écrire le .py
        std::ofstream py( nom_fic.c_str() );
        py << " \n";
        py << "from abaqus import *\n";
        py << "from abaqusConstants import *\n";
        py << "session.Viewport(name='Viewport: 1', origin=(0.0, 0.0), width=291.747924804688, \n";
        py << "height=233.079177856445)\n";
        py << "session.viewports['Viewport: 1'].makeCurrent()\n";
        py << "session.viewports['Viewport: 1'].maximize()\n";
        py << "from caeModules import *\n";
        py << "from driverUtils import executeOnCaeStartup\n";
        py << "executeOnCaeStartup()\n";
        py << "o1 = session.openOdb(name='" << racine_fic << ".odb')\n";
        py << "session.viewports['Viewport: 1'].setValues(displayedObject=o1)\n";
        py << "import visualization\n";
        py << "session.defaultXYReportOptions\n";
        py << "session.xyReportOptions.setValues(numDigits=9)\n";
        py << "\n";
        py << "odb = session.odbs['" << racine_fic << ".odb']\n";
        py << "\n";
        py << "U1_Part1=session.xyDataListFromField(odb=odb, outputPosition=NODAL, variable=(('U', \n";
        py << "NODAL, ((COMPONENT, 'U1'), )), ), nodeSets=('PART-1-1._PICKEDSET5', ))\n";
        py << "session.writeXYReport(fileName='" << racine_fic << "_U1.rpt', xyData=(U1_Part1))\n";
        py << "\n";
        py << "U2_Part1=session.xyDataListFromField(odb=odb, outputPosition=NODAL, variable=(('U', \n";
        py << "NODAL, ((COMPONENT, 'U2'), )), ), nodeSets=('PART-1-1._PICKEDSET5', ))\n";
        py << "session.writeXYReport(fileName='" << racine_fic << "_U2.rpt', xyData=(U2_Part1))\n";
        py << "\n";
        py << "from sys import *\n";
        py << "exit()\n";
    }
}

void Write_UMAT_parameters(std::string nom_fic_param, Vec<double> umat_parameters_tmp){
    std::string umatname = load_param_str(nom_fic_param,"umatname");
    std::string nom_fic_param_umat = umatname + "_param.txt";
    int umat_nparam = load_param(nom_fic_param_umat,"umat_nparam");
    int umat_ndepvar = load_param(nom_fic_param_umat,"umat_ndepvar");
    std::string nom_fic = ( umatname + "_param_tmp.txt" );
    std::ofstream param_tmp( nom_fic.c_str() );
    param_tmp << "umat_ndepvar\n";
    param_tmp << umat_ndepvar << "\n";
    param_tmp << "\n";
    param_tmp << "umat_nparam\n";
    param_tmp << umat_nparam << "\n";
    param_tmp << "\n";
    for (int np=1; np<umat_nparam+1; np++){
        param_tmp << "param" << np << "\n";
        param_tmp << umat_parameters_tmp[ np-1 ];
        if (abs(umat_parameters_tmp[ np-1 ] - (round(umat_parameters_tmp[ np-1 ]))) < 0.000000001) param_tmp << ".";
        param_tmp << "\n";
    }
}

void Write_dpc_parameters(Vec<double> dpc_parameters_tmp){
    std::string nom_fic = "dpc_param_tmp.txt" ;
    std::ofstream param_tmp( nom_fic.c_str() );
    param_tmp << "eps_crit\n";
    param_tmp << dpc_parameters_tmp[0];
    param_tmp << "\n";
    param_tmp << "min_eps\n";
    param_tmp << dpc_parameters_tmp[1];
    param_tmp << "\n";
    param_tmp << "mid_min_eps\n";
    param_tmp << dpc_parameters_tmp[2];
    param_tmp << "\n";
    param_tmp << "mid_max_eps\n";
    param_tmp << dpc_parameters_tmp[3];
    param_tmp << "\n";
    param_tmp << "max_eps\n";
    param_tmp << dpc_parameters_tmp[4];
    param_tmp << "\n";
    param_tmp << "a2\n";
    param_tmp << dpc_parameters_tmp[5];
    param_tmp << "\n";
    param_tmp << "b2\n";
    param_tmp << dpc_parameters_tmp[6];
    param_tmp << "\n";
    param_tmp << "n2\n";
    param_tmp << dpc_parameters_tmp[7];
    param_tmp << "\n";
    param_tmp << "tan_beta\n";
    param_tmp << dpc_parameters_tmp[8];
    param_tmp << "\n";
    param_tmp << "da_a\n";
    param_tmp << dpc_parameters_tmp[9];
    param_tmp << "\n";
    param_tmp << "da_b\n";
    param_tmp << dpc_parameters_tmp[10];
    param_tmp << "\n";
    param_tmp << "da_c\n";
    param_tmp << dpc_parameters_tmp[11];
    param_tmp << "\n";
    param_tmp << "r_a_min\n";
    param_tmp << dpc_parameters_tmp[12];
    param_tmp << "\n";
    param_tmp << "r_a_max\n";
    param_tmp << dpc_parameters_tmp[13];
    param_tmp << "\n";
    param_tmp << "k_p\n";
    param_tmp << dpc_parameters_tmp[14];
    param_tmp << "\n";
    param_tmp << "k_crit\n";
    param_tmp << dpc_parameters_tmp[15];
    param_tmp << "\n";
    param_tmp << "a_k\n";
    param_tmp << dpc_parameters_tmp[16];
    param_tmp << "\n";
    param_tmp << "g_p\n";
    param_tmp << dpc_parameters_tmp[17];
    param_tmp << "\n";
    param_tmp << "g_crit\n";
    param_tmp << dpc_parameters_tmp[18];
    param_tmp << "\n";
    param_tmp << "a_g\n";
    param_tmp << dpc_parameters_tmp[19];
    param_tmp << "\n";
}

Vec<double> load_dpc_parameters(std::string nom_fic_param_dpc, Vec<double> dpc_parameters){

    double eps_crit = load_param(nom_fic_param_dpc,"eps_crit");
    dpc_parameters << eps_crit;
    double min_eps = load_param(nom_fic_param_dpc,"min_eps");
    dpc_parameters << min_eps;
    double mid_min_eps = load_param(nom_fic_param_dpc,"mid_min_eps");
    dpc_parameters << mid_min_eps;
    double mid_max_eps = load_param(nom_fic_param_dpc,"mid_max_eps");
    dpc_parameters << mid_max_eps;
    double max_eps = load_param(nom_fic_param_dpc,"max_eps");
    dpc_parameters << max_eps;
    double a2 = load_param(nom_fic_param_dpc,"a2");
    dpc_parameters << a2;
    double b2 = load_param(nom_fic_param_dpc,"b2");
    dpc_parameters << b2;
    double n2 = load_param(nom_fic_param_dpc,"n2");
    dpc_parameters << n2;
    double tan_beta = load_param(nom_fic_param_dpc,"tan_beta");
    dpc_parameters << tan_beta;
    double da_a = load_param(nom_fic_param_dpc,"da_a");
    dpc_parameters << da_a;
    double da_b = load_param(nom_fic_param_dpc,"da_b");
    dpc_parameters << da_b;
    double da_c = load_param(nom_fic_param_dpc,"da_c");
    dpc_parameters << da_c;
    double r_a_min = load_param(nom_fic_param_dpc,"r_a_min");
    dpc_parameters << r_a_min;
    double r_a_max = load_param(nom_fic_param_dpc,"r_a_max");
    dpc_parameters << r_a_max;
    double k_p = load_param(nom_fic_param_dpc,"k_p");
    dpc_parameters << k_p;
    double k_crit = load_param(nom_fic_param_dpc,"k_crit");
    dpc_parameters << k_crit;
    double a_k = load_param(nom_fic_param_dpc,"a_k");
    dpc_parameters << a_k;
    double g_p = load_param(nom_fic_param_dpc,"g_p");
    dpc_parameters << g_p;
    double g_crit = load_param(nom_fic_param_dpc,"g_crit");
    dpc_parameters << g_crit;
    double a_g = load_param(nom_fic_param_dpc,"a_g");
    dpc_parameters << a_g;

    return dpc_parameters;
}

Vec<double>  load_abq_res(std::string nom_fic, int numsteps){

    int numlines=count_lines(nom_fic)-4;
    int interv=numlines/numsteps;
    std::ifstream fichier;
    fichier.open(nom_fic.c_str());
    std::string ligne;
    Vec<double> valeurs;

    int kk=0;
    std::string mot;
    for (int i=0; i<numlines; i++){
        std::getline(fichier, ligne);
        int nb_mots_ligne = 0;
        if ((i+1)%interv == 0){
            for (int ii = 0; ii<ligne.size(); ii++){
                if ((ligne[ii]==' ') or (ii==ligne.size()-1)){
                    if (mot.size() > 0){
                        mot +=ligne[ii];
                        if (nb_mots_ligne >0) valeurs.push_back(atof(mot.c_str()));
                        mot.clear();
                        nb_mots_ligne++;
                    }
                }
                else {
                    mot +=ligne[ii];
                }
            }
        }
    }
    return valeurs;
}

Vec<TM> load_abq_res_odb(std::string nom_fic, Vec<TM> res){

    std::cout << " " << std::endl;
    std::cout << "Reading ";
    std::cout << nom_fic.c_str() << std::endl;

    odb_initializeAPI();  // pour initialiser l'interface c++ abaqus il faut impérativement appeler cette fonction avant de faire appel à  d'autre fonctions
    odb_Odb& odb = openOdb( nom_fic.c_str() );// appel de la fonction openOdb qui permet d'ouvrir un fichier abaqus odb
    //odb_Odb& odb = openOdb( "viewer_tutorial.odb" );// appel de la fonction openOdb qui permet d'ouvrir un fichier abaqus odb
    odb_Assembly& rootAssy = odb.rootAssembly();
    odb_InstanceRepositoryIT instIter( rootAssy.instances() );
    for ( instIter.first(); !instIter.isDone(); instIter.next() )
        std::cout << instIter.currentKey().CStr() << std::endl;

    // Lecture des coordonnées des noeuds
    odb_Instance& instance1 =rootAssy.instances()[ "PART-1-1" ];
    odb_Enum::odb_DimensionEnum instanceType = instance1.embeddedSpace();
    const odb_SequenceNode& nodeList = instance1.nodes();
    int nodeListSize = nodeList.size();
    //res[0].node_list.resize(nodeList.size());
    if ( instanceType == odb_Enum::THREE_D ) {
        for ( int n = 0; n < nodeListSize; n++ ) {
            const odb_Node node      = nodeList[ n ];
            int nodeLabel            = node.label();
            const float* const coord = node.coordinates();
            //   res[0].node_list[n].pos = node.coordinates();
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
        //std::cout << nstep << std::endl;
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
            //		std::cout << fieldIter.currentKey().CStr() << std::endl;
            odb_FieldOutput& field = fieldOutputRep[fieldIter.currentKey()];
            //const odb_SequenceFieldValue& seqVal = field.values();
            const odb_SequenceFieldLocation& seqLoc = field.locations();
            //		std::cout << field.name().CStr() << " : " << field.description().CStr() << std::endl;
            //		std::cout << "	Type: " << field.type() << std::endl;
            int numLoc = seqLoc.size();
            for (int loc = 0; loc<numLoc; loc++){
                //		    std::cout << "Position: "<<seqLoc.constGet(loc).position();
            }
            //		std::cout << std::endl;
        }

        const odb_SequenceFieldValue& displacements = lastFrame.fieldOutputs()["U"].values();
        const odb_SequenceFieldValue& reac_forces = lastFrame.fieldOutputs()["RF"].values();
        int numValues = displacements.size();
        int numComp = 0;
        for (int i=0; i<numValues; i++) {
            const odb_FieldValue valU = displacements[i];
            const odb_FieldValue valRF = reac_forces[i];
            const float* const U = valU.data(numComp);
            const float* const RF = valRF.data(numComp);

            for (int comp=0; comp < numComp; comp++) {
                res[num_step].node_list[i].dep[comp] = U[comp];
                res[num_step].node_list[i].f_nodal[comp] = RF[comp];
                //PRINT(res[num_step].node_list[i].dep[comp]);
            }
        }
        num_step += 1;
    }

    std::cout << " " << std::endl;
    return res;

    //	odb.close();
    //	odb_finalizeAPI();  // terminer l'interface c++ abaqus

    //	return res;
}

Vec<TM> calc_abq(Vec<TM> &m_ref, std::string racine_fic, std::string nom_fic_param, Vec<double> constrained_nodes, double pix2m){


    Vec<TM> m;
    int prob_size=m_ref.size();
    m.resize(prob_size);

    for (int ii =0; ii < prob_size; ii++)
        m[ii]=m_ref[ii];

    WriteINP (m, racine_fic, nom_fic_param, constrained_nodes, pix2m);

    std::string thelaw = load_param_str(nom_fic_param,"thelaw");

    if (thelaw == "UMAT"){
        std::string umatname = load_param_str(nom_fic_param,"umatname");
        //system(("abaqus_6.11-2 interactive job=" + racine_fic + ".inp user=" + umatname + ".f double > res_s.txt").c_str() );
        system(("abaqus_6.11-2 interactive job=" + racine_fic + ".inp user=" + umatname + ".f double").c_str() );
    }
    else if (thelaw == "dpc"){
        //system(("abaqus_6.11-2 interactive job=" + racine_fic + ".inp user=elasticity_variation_epsvol.f double > res_s.txt").c_str() );
        system(("abaqus_6.11-2 interactive job=" + racine_fic + ".inp user=elasticity_variation_epsvol.f double").c_str() );
    }
    else{
        //system(("abaqus_6.11-2 interactive job=" + racine_fic + ".inp cpus=6 double > res_s.txt").c_str() );
        system(("abaqus_6.11-2 interactive job=" + racine_fic + ".inp cpus=6 ").c_str() );
    }

    //// OLD FASHION RESULT READING
    //WriteExtract(racine_fic);
    //system(("abaqus_6.11-2 cae  > res_s.txt noGUI=" + racine_fic + ".py > res_s.txt").c_str() );
    //system(("sed -e \"1,4d\" " + racine_fic + "_U1.rpt>" + racine_fic + "_U1.txt").c_str());
    //remove((racine_fic + "_U1.rpt").c_str());
    //system(("sed -e \"1,4d\" " + racine_fic + "_U2.rpt>" + racine_fic + "_U2.txt").c_str());
    //remove((racine_fic + "_U2.rpt").c_str());
    //std::string nom_res_U1 = (racine_fic + "_U1.txt");
    //Vec<double> vecteur1 = load_abq_res(nom_res_U1,m.size());
    //std::string nom_res_U2 = (racine_fic + "_U2.txt");
    //Vec<double> vecteur2 = load_abq_res(nom_res_U2,m.size());
    //res.push_back(vecteur1);
    //res.push_back(vecteur2);

    //// NEW FASHION RESULT READING
    Vec<TM> res = m;

    res = load_abq_res_odb ( racine_fic + ".odb" , res);

    remove((racine_fic + ".odb").c_str());
    remove((racine_fic + ".lck").c_str());
    remove((racine_fic + ".com").c_str());
    return res;
}

Vec <Vec <Vec < double > > >  extract_dep ( Vec<TM> res, Vec < Vec <Vec < double > > > dep ){

    Vec <Vec <double> > dep_ref ;
    Vec <double> depU;Vec <double> depV;
    for (int num_mesh = 0; num_mesh < res.size(); ++num_mesh){
        for (int n = 0; n < res[ num_mesh ].node_list.size(); ++n){
            depU << res[ num_mesh ].node_list[ n ].dep[ 0 ];
            depV << res[ num_mesh ].node_list[ n ].dep[ 1 ];
        }
    }
    dep_ref.push_back(depU);dep_ref.push_back(depV);dep.push_back(dep_ref);
    return dep;
}


Vec<double> extract_fnod ( Vec<TM> res_ref, Vec<TM> m, std::string senstrac , bool sortie){

    Pvec tmxmin, tmxmax;
    get_min_max( m[ 0 ].node_list, ExtractDM<pos_DM>(), tmxmin, tmxmax );

    Vec<double> fnodalmin , fnodalmax; fnodalmin.resize(res_ref.size()-1); fnodalmax.resize(res_ref.size()-1);
    for( int num_mesh = 0; num_mesh < res_ref.size()-1; ++num_mesh ) {
        fnodalmin[num_mesh] = 0;
        fnodalmax[num_mesh] = 0;
        for( int n = 0; n < res_ref[ num_mesh ].node_list.size(); ++n ){
            if (senstrac == "hor"){
                if ( m[ 0 ].node_list[ n ].pos[ 0 ] <= tmxmin[ 0 ] + 2 ){
                    fnodalmin[num_mesh] += res_ref[ num_mesh ].node_list[n].f_nodal[0];
                }
                if ( m[ 0 ].node_list[ n ].pos[ 0 ] >= tmxmax[ 0 ] - 2 ){
                    fnodalmax[num_mesh] += res_ref[ num_mesh ].node_list[n].f_nodal[0];
                }
            }
            if (senstrac == "ver"){
                if ( m[ 0 ].node_list[ n ].pos[ 1 ] <= tmxmin[ 1 ] + 2 ){
                    fnodalmin[num_mesh] += res_ref[ num_mesh ].node_list[n].f_nodal[1];
                }
                if(  m[ 0 ].node_list[ n ].pos[ 1 ] >= tmxmax[ 1 ] - 2 ){
                    fnodalmax[num_mesh] += res_ref[ num_mesh ].node_list[n].f_nodal[1];
                }
            }
        }
    }

    if (sortie){
        std::string fminame = "resultats/fnodmin_m" +  to_string( res_ref.size() ) + ".txt";
        std::ofstream fnodmin( fminame.c_str() );
        for (int num = 0; num < fnodalmin.size(); num++)
            fnodmin << fnodalmin[num] << " " ;
        std::string fmaname = "resultats/fnodmax_m" +  to_string( res_ref.size() ) + ".txt";
        std::ofstream fnodmax( fmaname.c_str() );
        for (int num = 0; num < fnodalmax.size(); num++)
            fnodmax << fnodalmax[num] << " " ;
    }
    return fnodalmax;
}

void remove_all_files_beginning_with(std::string debut){

    DIR * rep = opendir(".");

    if (rep != NULL)
    {
        struct dirent * ent;

        while ((ent = readdir(rep)) != NULL)
        {
            //printf("Removing %s\n", ent->d_name);
            std::string nom = ent->d_name;
            std::string nom2=nom.substr(0,debut.size());
            if (nom2==debut)
                system((" rm " + nom).c_str());
        }

        closedir(rep);
    }


}


#endif
