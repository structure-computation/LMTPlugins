DIR_SOURCES_SOCA = -IServerPlugin -IServerPlugin/src  -I../../Soca/src  -IServerPlugin/LMT/include 
DIR_SOURCES_ABAQUS = -LServerPlugin/ABQ/exec/lbr -IServerPlugin/ABQ/ -IServerPlugin/ABQ/include 


all:
	make -C ../../Soca
	metil_comp -ne -j1 -O3 -g3 $(DIR_SOURCES_SOCA) -fpermissive ServerPlugin/src/main.cpp

gdb:
	metil_comp -ne --gdb -O3 -g3 $(DIR_SOURCES_SOCA) -fpermissive ServerPlugin/src/main.cpp
