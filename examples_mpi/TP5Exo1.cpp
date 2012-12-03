#include <mpi.h>
#include <iostream>

using namespace std;


int main(int argc, char *argv[]) {
    int id, nb_instance, len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &nb_instance);
    MPI_Get_processor_name(processor_name, &len);
    
    MPI_Status mystatus;
    int var=0;
    while (var >= 0)
    {
    	if (id == 0)
    	{
    		cout << "Enter a positive integer:" ;
    		cin >> var;
    		MPI_Send(&var,1,MPI_INT,1,1,MPI_COMM_WORLD);
    	}
    	else
    	{
    		MPI_Recv(&var,1,MPI_INT,0,1,MPI_COMM_WORLD,&mystatus);
    	}
    	cout <<"My variable is: " << var << "and i'm process " << id << endl;
    }


    MPI_Finalize();
    
    return 0;
}

