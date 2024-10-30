#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	int rango, tam_com, valor_local, suma_global;
	MPI_Init(&argc, &argv);  // Inicializa el entorno MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rango);  // Obtiene el rango del proceso
	MPI_Comm_size(MPI_COMM_WORLD, &tam_com);  // Obtiene el tamaño total de procesos
	
	// Inicializar un valor local (para este ejemplo, cada proceso usa su rango como valor local)
	valor_local = rango;
	
	// Reducción en árbol para un número de procesos potencia de dos
	int paso;
	suma_global = valor_local;
	for (paso = 1; paso < tam_com; paso *= 2) {
		if (rango % (2 * paso) == 0) {
			int valor_recibido;
			MPI_Recv(&valor_recibido, 1, MPI_INT, rango + paso, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			suma_global += valor_recibido;
		} else if ((rango - paso) % (2 * paso) == 0) {
			MPI_Send(&suma_global, 1, MPI_INT, rango - paso, 0, MPI_COMM_WORLD);
			break;
		}
	}
	
	// El proceso 0 tendrá la suma global
	if (rango == 0) {
		printf("Suma global = %d\n", suma_global);
	}
	
	MPI_Finalize();  // Finaliza el entorno MPI
	return 0;
}
