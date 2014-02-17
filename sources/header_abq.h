  	  /*************************************************************************************************************************/
                  /*                             fichier header a inclure pour interfacer c++ avec abaqus                                  */
	          /*  ce fichier est compatible qu'avec la compilation et l'exécution à l'aide de metil_comp de l'utilitaire Metil         */
		  /*  si Metil est installée sur votre machine faites un git pull pour avoir les nouvelles modifications apportées à Metil */ 	     
		  /*  sinon consultez la doc de Metil dans  WIKI_Git_LMT à travers Intranet pour l'installer                               */ 
                  /*************************************************************************************************************************/

#ifndef HEADER_ABAQUS_H
#ifdef METIL_COMP_DIRECTIVE

// le chemin des .h de la librairie abaqus
// le chemin des libraries abaqus
#pragma inc_path /media/mathieu/Data/Abaqus/6.11-2_odb_api/include
#pragma lib_path /media/mathieu/Data/Abaqus/6.11-2_odb_api/lib
#pragma lib_path /media/mathieu/Data/Abaqus/exec/lbr
#pragma lib_path /media/mathieu/Data/Abaqus/External

//  les lignes ci_dessous représentent les librairies abaqus nécessaire à interfacer du c++ 
#pragma lib_name irc 
#pragma lib_name ABQDDB_Odb 
#pragma lib_name ABQSMAOdbApi
#pragma lib_name ABQSMAOdbCore
#pragma lib_name ABQSMAOdbAttrEO
#pragma lib_name ABQSMAOdbCoreGeom
#pragma lib_name ABQSMAAspSupport
#pragma lib_name ABQSMAAspDiagExtractor
#pragma lib_name ABQSMARomDiagEx
#pragma lib_name ABQSMARfmInterface
#pragma lib_name ABQSMAAbuGeom
#pragma lib_name ABQSMAAbuBasicUtils
#pragma lib_name ABQSMABasShared
#pragma lib_name ABQSMABasCatia
#pragma lib_name ABQSMABasCoreUtils
#pragma lib_name ABQSMABasMem
#pragma lib_name ABQJS0GROUP
#pragma lib_name imf
#pragma lib_name svml
#pragma lib_name intlc
#pragma lib_name ABQSMAShpCore
#pragma lib_name ABQSMASimInterface
#pragma lib_name ABQSMABasGenericsLib
#pragma lib_name ABQSMAAspCommunications
#pragma lib_name ABQSMAFeoModules
#pragma lib_name ABQSMAMtxCoreModule
#pragma lib_name ABQSMAElkCore
#pragma lib_name ABQDMP_Core
#pragma lib_name ABQSMASrvBasic
#pragma lib_name ABQSMAObjSimObjectsMod
#pragma lib_name ABQSMASimContainers
#pragma lib_name ABQSMAAbuLicense
#pragma lib_name ifport
#pragma lib_name ifcoremt
#pragma lib_name ABQSMAUzlZlib
#pragma lib_name ABQSMABasPrfTrkLib
#pragma lib_name ABQSMASimTypeSubcomp
#pragma lib_name ABQSMASimBulkAPI
#pragma lib_name ABQSMASimPoolManager
#pragma lib_name ABQSMABasXmlDocument
#pragma lib_name ABQSMABasXmlParser
#pragma lib_name ABQSMAMsgModules
#pragma lib_name ABQSMABlaModule
#pragma lib_name ABQSMAFsmShared
#pragma lib_name ABQSMASglSimXmlFndLib
#pragma lib_name SMAFeaBackbone
#pragma lib_name CATSysMultiThreading
#pragma lib_name CATSysCommunication
#pragma lib_name JS0GROUP
#pragma lib_name mkl_intel_lp64
#pragma lib_name mkl_sequential
#pragma lib_name mkl_core
#pragma lib_name mkl_lapack
#pragma lib_name ABQSMASglSharedLib

#endif // METIL_COMP_DIRECTIVE
#endif // HEADER_ABAQUS_H
