#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
	int rango, tam_com, valor_local, suma_global;
	MPI_Init(&argc, &argv);  // Inicializa el entorno MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rango);  // Obtiene el rango del proceso
	MPI_Comm_size(MPI_COMM_WORLD, &tam_com);  // Obtiene el tamaño total de procesos
	
	// Inicializa un valor local (para este ejemplo, cada proceso usa su rango como valor local)
	valor_local = rango;
	
	// Si tam_com no es una potencia de dos, maneja los procesos adicionales
	int potencia_cercana_de_dos = 1;
	while (potencia_cercana_de_dos <= tam_com / 2) {
		potencia_cercana_de_dos *= 2;
	}
	
	suma_global = valor_local;
	
	// Los procesos adicionales envían sus datos al proceso de potencia de dos más baja
	if (rango >= potencia_cercana_de_dos) {
		MPI_Send(&suma_global, 1, MPI_INT, rango - potencia_cercana_de_dos, 0, MPI_COMM_WORLD);
	} else {
		if (rango + potencia_cercana_de_dos < tam_com) {
			int valor_extra;
			MPI_Recv(&valor_extra, 1, MPI_INT, rango + potencia_cercana_de_dos, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			suma_global += valor_extra;
		}
		
		// Reducción estándar en árbol para procesos en potencia de dos
		int paso;
		for (paso = 1; paso < potencia_cercana_de_dos; paso *= 2) {
			if (rango % (2 * paso) == 0) {
				int valor_recibido;
				MPI_Recv(&valor_recibido, 1, MPI_INT, rango + paso, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				suma_global += valor_recibido;
			} else if ((rango - paso) % (2 * paso) == 0) {
				MPI_Send(&suma_global, 1, MPI_INT, rango - paso, 0, MPI_COMM_WORLD);
				break;
			}
		}
	}
	
	// El proceso 0 tendrá la suma global
	if (rango == 0) {
		printf("Suma global = %d\n", suma_global);
	}
	
	MPI_Finalize();  // Finaliza el entorno MPI
	return 0;
}


