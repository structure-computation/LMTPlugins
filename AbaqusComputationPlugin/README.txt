Pour faire les liens vers les librairies dynamiques d'Abaqus (.so) :

dans le /home/user/.bashrc, ajouter des lignes :

export LD_LIBRARY_PATH="/endroit_où_Abaqus_est_installé/exec/lbr:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/endroit_où_Abaqus_est_installé/External:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/endroit_où_Abaqus_est_installé/6.11-2_odb_api/lib:$LD_LIBRARY_PATH"
