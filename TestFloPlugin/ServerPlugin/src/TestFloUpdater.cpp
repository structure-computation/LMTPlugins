
#include <Soca/Com/SodaClient.h>
#include <Soca/Model/TypedArray.h>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QDataStream>
#include <containers/vec.h>
#include "dic/correlation/ImgInterp.h"
#include "mesh/mesh.h"
#include "header_abq.h"
#include "/media/mathieu/Data/Abaqus/include/odb_API.h" // pour les fonctions odb d'abaqus
#include "mesh/write_mesh_vtk.h"
#include "dic/correlation/mesh_carac_correlation.h"
#include <iostream>
#include <boost/graph/graph_concepts.hpp>
#include "TestFloUpdater.h"
#include "FieldSet.h"
//#include "formulation.Ramberg_Osgood.Triangle.name_der_vars=param.h"
//#include "lib_flo.h"

typedef LMT::Mesh<Mesh_carac_pb_correlation_basic<double,2> > TM;
typedef TM::Pvec Pvec;

bool exists( const std::string & file )
{
    std::ifstream fichier( file.c_str() );
    return !fichier.fail();
}

TM load_into_2DLMTpp_Mesh(MP mesh){ 
    TM dic_mesh;
    TypedArray<int> *indices_elem = dynamic_cast<TypedArray<int> *>( mesh[ "_elements[0].indices" ].model() );
    const unsigned nb_elems = indices_elem->size(1);
    
    qDebug() << "Nb de noeuds : " << mesh[ "points" ].size() << ", Nb d'éléments : " << nb_elems;
    
    for( int i = 0, n = mesh[ "points" ].size(); i < n; ++i ) {
        MP pos = mesh[ "points" ][ i ][ "pos" ];
        dic_mesh.add_node( Pvec( pos[ 0 ], pos[ 1 ], pos[ 2 ] ) );
    }
    for( int nel = 0, mel = nb_elems; nel < mel; ++nel ) {
        MP el = mesh[ "_elements" ][ nel ];
        // if ( el.type() == "Element_TriangleList" and dim == 2 ) {
        if ( el.type() == "Element_TriangleList"  ) {
            if ( TypedArray<int> *indices = dynamic_cast<TypedArray<int> *>( el[ "indices" ].model() ) ) {
                for( int nt = 0, ct = 0; nt < indices->size( 1 ); ++nt ) {
                    unsigned o[ 3 ];
                    for( int j = 0; j < 3; ++j ){
                        o[ j ] = qMin( indices->operator[]( ct++ ), (int)dic_mesh.node_list.size() - 1 );
                    }
                    dic_mesh.add_element( Triangle(), DefaultBehavior(), o );
                }
            }
        }
    }
    dic_mesh.remove_unused_nodes();
    return dic_mesh;
}

void Write2DINP (Vec<TM> &m, std::string racine_fic, Vec<double> constrained_nodes, double pix2m, Vec < Vec < std::string > > Prop_Mat){
    std::string nom_fic = ( racine_fic + ".inp" );
    for (int z = 0; z <1; ++z) { // fausse boucle pour écrire l'inp
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string thelaw;
        double elastic_modulus, poisson_ratio, rapport, sigma_0, n;
        
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
        }
        
        if (exists(nom_fic)) remove(nom_fic.c_str());
        int vide = system(("touch " + nom_fic).c_str());
        
        if (thelaw == "UMAT"){
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            //             std::string umatname = load_param_str(nom_fic_param,"umatname");
            //             std::string nom_fic_param_umat = umatname + "_param.txt";
            //             double preamble = load_param(nom_fic_param_umat, "umat_preamble");
            //             if (preamble) {
            //                 add_to_file(nom_fic, umatname + "_1.txt");
            //             }
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string def_mode = "CP";
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        inp << "          1.,           0.,           0.,           0.,           1.,           0.\n";
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
            //       std::string umatname = load_param_str(nom_fic_param,"umatname");
            //       inp << umatname << "\n";
        }
        else if (thelaw == "dpc"){
            //////////////////////////////////////////////////////////////////////////////////////////////////////////
            inp << "MATERIAL-cg\n";
        }
        double thickness = 1;
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
        else if (thelaw == "UMAT"){/*
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
            inp.open(nom_fic.c_str(), std::ios::app);*/
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
            inp << "** STEP: Step-" << i+1 << "\n";
            inp << "** \n";
            inp << "*Step, name=Step-" << i+1 << "\n";
            inp << "*Static\n";
            inp << "0.0001, 1, 1e-03, 0.7\n";
            inp << "** \n";
            inp << "** BOUNDARY CONDITIONS\n";
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
    std::remove((racine_fic +".odb").c_str());
    std::remove((racine_fic +".sta").c_str());
    std::remove((racine_fic +".sim").c_str());
    std::remove((racine_fic +".prt").c_str());
    std::remove((racine_fic +".dat").c_str());
    std::remove((racine_fic +".msg").c_str());
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
    
    
    
            std::cout << nodeListSize << std::endl;
            
            
            
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
        int numValues = displacements.size();
        int numComp = 0;
        for (int i=0; i<numValues; i++) {
            const odb_FieldValue valU = displacements[i];
            const odb_FieldValue valRF = reac_forces[i];
            const float* const U = valU.data(numComp);
            const float* const RF = valRF.data(numComp);
            
            for (int comp=0; comp < numComp; comp++) {
                res[num_step].node_list[i].dep[comp] = U[comp];
                //res[num_step].node_list[i].f_nodal[comp] = RF[comp];
                //PRINT(res[num_step].node_list[i].dep[comp]);
            }
        }
        num_step += 1;
    }
    
    std::cout << " " << std::endl;
    return res;
    
    //  odb.close();
    //  odb_finalizeAPI();  // terminer l'interface c++ abaqus
    
    //  return res;
}




//template<unsigned dim>
bool TestFloUpdater::run( MP mp ) {
    
    TM dic_mesh;
    Vec<TM> Vecteur_de_maillages_input; 
    int nb_images =1;
    
    Vec<double> constrained_nodes; 
    
    double pix2m = mp[ "pix2m" ];
    
    MP ch = mp[ "_children" ]; 
    FieldSet fs_input_bckp;
    double Young, Poisson, loi, rapport, sigma_0, n;
    for( int ii = 0; ii < ch.size(); ++ii ) {
        MP c = ch[ ii ];
        if (( c.type() == "FieldSetItem" ) or ( c.type() == "FieldSetCorreliItem" )){
            FieldSet fs_input(c);fs_input_bckp=fs_input;
            Mesh_vecs maillage = fs_input.mesh;
            MP maillage_transfert = maillage.save();
            dic_mesh = load_into_2DLMTpp_Mesh(maillage_transfert);
            
            Vecteur_de_maillages_input.resize(fs_input.fields[0].values.size()); 
            for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_input.size(); num_mesh++){
                Vecteur_de_maillages_input[num_mesh]=dic_mesh;
                fs_input.load_current_time_step(num_mesh);
                for (int no = 0; no < Vecteur_de_maillages_input[num_mesh].node_list.size(); no++ ) {
                    Vecteur_de_maillages_input[num_mesh].node_list[no].dep[0] = fs_input.fields[0].values[0].data[no];
                    Vecteur_de_maillages_input[num_mesh].node_list[no].dep[1] = fs_input.fields[1].values[0].data[no];
                }
                Vecteur_de_maillages_input[num_mesh].update_skin();
                for( int i = 0; i < Vecteur_de_maillages_input[num_mesh].skin.node_list.size(); ++i )
                    constrained_nodes << Vecteur_de_maillages_input[num_mesh].skin.node_list[ i ].number;
                //write_mesh_vtk( "/media/mathieu/Data/test_input_" + to_string(num_mesh) + ".vtu", Vecteur_de_maillages_input[num_mesh]);
            }
        }
        //         else if ( c.type() == "MeshItem" ) {
        //             MP mesh = c["_mesh" ]; 
        //             dic_mesh = load_into_2DLMTpp_Mesh(mesh);
        //             add_message( mp, ET_Info, "Maillage récupéré" );
        //             mp.flush();
        //             //write_mesh_vtk( "/media/mathieu/Data/test.vtu",dic_mesh);
        //             Vecteur_de_maillages_input.resize(1); Vecteur_de_maillages_input[0]=dic_mesh;
        //             constrained_nodes << 0; constrained_nodes << 10;
        //         }
        
        if ( c.type() == "MaterialABQItem" ) {
            loi = c["Comportement.num"];
            std::cout << loi << std::endl;
            Young = c["Young[0]"];
            Poisson = c["Poisson[0]"];
            if (loi == 1)
                rapport = c["rapport[0]"];
            if (loi == 2){
                sigma_0 = c["sigma_0[0]"];
                n = c["n[0]"];
            }
        }
    }
    
    Vec < Vec < std::string > > Prop_Mat ; 
    if (loi == 0)
        Prop_Mat.resize(3);
    if (loi == 1)
        Prop_Mat.resize(4);
    if (loi == 2)
        Prop_Mat.resize(5);
    
    Prop_Mat[0] << "thelaw";
    if (loi == 0)
        Prop_Mat[0] << "Elas_iso";
    else if (loi == 1)
        Prop_Mat[0] << "Elas_ortho";
    else if (loi == 2)
        Prop_Mat[0] << "RO";
    std::string thelaw = Prop_Mat[0][1];
    
    Prop_Mat[1] << "Young";
    Prop_Mat[1] << LMT::to_string(Young*1e9); 
    Prop_Mat[2] << "Poisson";
    Prop_Mat[2] << LMT::to_string(Poisson); 
    if (loi == 1){
        Prop_Mat[3] << "rapport";
        Prop_Mat[3] << LMT::to_string(rapport);
    }
    else if (loi == 2){
        Prop_Mat[3] << "sigma_0";
        Prop_Mat[3] << LMT::to_string(sigma_0*1e6);
        Prop_Mat[4] << "n";
        Prop_Mat[4] << LMT::to_string(n);
    }
    
    std::string racine_fic = ("/media/mathieu/Data/aaa_test_" + LMT::to_string( nb_images ) );
    Write2DINP (Vecteur_de_maillages_input, racine_fic, constrained_nodes, pix2m, Prop_Mat);
    add_message( mp, ET_Info, "Ecriture du fichier .inp terminée" );
    mp.flush();
    
    std::string nom_fic_lck = racine_fic + ".lck";if (exists(nom_fic_lck)) remove(nom_fic_lck.c_str());
    std::string nom_fic_odb = racine_fic + ".odb";if (exists(nom_fic_odb)) remove(nom_fic_odb.c_str());
    
    if (thelaw == "UMAT"){
        // std::string umatname = load_param_str(nom_fic_param,"umatname");
        //system(("abaqus interactive job=" + racine_fic + ".inp user=" + umatname + ".f double > res_s.txt").c_str() );
        // system(("abaqus interactive job=" + racine_fic + ".inp user=" + umatname + ".f double").c_str() );
    }
    else if (thelaw == "dpc"){
        //system(("abaqus interactive job=" + racine_fic + ".inp user=elasticity_variation_epsvol.f double > res_s.txt").c_str() );
        system(("abaqus interactive job=" + racine_fic + ".inp user=elasticity_variation_epsvol.f double").c_str() );
    }
    else{
        //system(("abaqus interactive job=" + racine_fic + ".inp cpus=6 double > res_s.txt").c_str() );
        system(("/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" + racine_fic + ".inp cpus=8 ").c_str() );
        std::cout << "/media/mathieu/Data/Abaqus/exec/abq6112.exe interactive job=" << racine_fic << ".inp cpus=8 " << std::endl;
    }
    
    add_message( mp, ET_Info, "Calcul terminé" );
    mp.flush();
    
    
    std::string nom_fic = racine_fic + ".odb";
    Vec<TM> Vecteur_de_maillages_output = load_abq_res_odb(nom_fic, Vecteur_de_maillages_input);
    
    for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_output.size(); num_mesh++){
        write_mesh_vtk( "/media/mathieu/Data/test_output_" + to_string(num_mesh) + ".vtu", Vecteur_de_maillages_output[num_mesh]);
        for (int no = 0; no < Vecteur_de_maillages_output[num_mesh].node_list.size(); no++ ) {
            //PRINT(Vecteur_de_maillages_output[num_mesh].node_list[no].dep[0]);
          //  PRINT(no);
        }
    }
    
    add_message( mp, ET_Info, "Résultat récupéré" );
    mp.flush();
    
    FieldSet fs_output = fs_input_bckp;
    
    for (int num_mesh = 0; num_mesh < Vecteur_de_maillages_output.size(); num_mesh++){
        fs_output.load_current_time_step(num_mesh);
        for (int no = 0; no < Vecteur_de_maillages_input[num_mesh].node_list.size(); no++ ) {
            fs_output.fields[0].values[0].data[no] = Vecteur_de_maillages_output[num_mesh].node_list[no].dep[0]/pix2m;
            fs_output.fields[1].values[0].data[no] = Vecteur_de_maillages_output[num_mesh].node_list[no].dep[1]/pix2m;
        }
    }
    
    fs_output.save(mp["_output[0]"]);
    add_message( mp, ET_Info, "Résultat renvoyé" );
    mp.flush();
    
    
    
}

