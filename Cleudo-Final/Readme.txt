To run this program, put those file in hydra and write on the terminal :

module load openmpi/1.4.3/gcc/4.6.1 (only the first time)
mpiCC GameMaster.cpp AI.cpp Cleudo.cpp -o game
mpirun -mca btl ^openib -np 6 game
