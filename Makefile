LIBRARIES = \
	AbaqusComputationPlugin \
	DicUncertaintyPlugin \
	IdentificationWithAbaqusPlugin \
	Code_Aster_ComputationPlugin \
	FieldExportPlugin \
	IdentificationWithCode_AsterPlugin \
	VirtualGaugePlugin \
	CorreliPlugin 
	
SHELL = /bin/bash
	
all: 
	for i in ${LIBRARIES}; do \
		rm $$i/ServerPlugin/dic; \
		ln -s ../../../dic $$i/ServerPlugin/dic; \
		rm $$i/ServerPlugin/LMT; \
		ln -s ../../../LMTpp $$i/ServerPlugin/LMT; \
		make -C $$i; \
	done
	