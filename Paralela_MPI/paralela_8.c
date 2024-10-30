#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Función de comparación para qsort
int comparar(const void* a, const void* b) {
	return (*(int*)a - *(int*)b);
}

// Función para combinar dos arreglos ordenados
void combinar(int* izquierda, int tam_izquierda, int* derecha, int tam_derecha, int* resultado) {
	int i = 0, j = 0, k = 0;
	while (i < tam_izquierda && j < tam_derecha) {
		if (izquierda[i] < derecha[j]) {
			resultado[k++] = izquierda[i++];
		} else {
			resultado[k++] = derecha[j++];
		}
	}
	while (i < tam_izquierda) resultado[k++] = izquierda[i++];
	while (j < tam_derecha) resultado[k++] = derecha[j++];
}

int main(int argc, char* argv[]) {
	int rango, tam_comunicacion;
	int n, n_local;
	int *datos_globales = NULL, *datos_locales = NULL;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rango);
	MPI_Comm_size(MPI_COMM_WORLD, &tam_comunicacion);
	
	if (rango == 0) {
		// El proceso 0 lee el número total de elementos
		printf("Ingrese el numero de elementos (n): ");
		fflush(stdout); // Asegura que el texto se imprima inmediatamente
		scanf("%d", &n);
		
		// Verificar si n es divisible por tam_comunicacion
		if (n % tam_comunicacion != 0) {
			fprintf(stderr, "El numero de elementos debe ser divisible por el número de procesos.\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		
		// Asignar e inicializar el arreglo de datos globales
		datos_globales = malloc(n * sizeof(int));
		printf("Datos no ordenados:\n");
		fflush(stdout); // Asegura que el texto se imprima inmediatamente
		for (int i = 0; i < n; i++) {
			datos_globales[i] = rand() % 100;  // Valores aleatorios entre 0 y 99
			printf("%d ", datos_globales[i]);
		}
		printf("\n");
		fflush(stdout); // Asegura que el texto se imprima inmediatamente
	}
	
	// Transmitir n a todos los procesos
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	n_local = n / tam_comunicacion;
	
	// Asignar memoria para datos locales
	datos_locales = malloc(n_local * sizeof(int));
	
	// Distribuir los datos globales a cada arreglo local de los procesos
	MPI_Scatter(datos_globales, n_local, MPI_INT, datos_locales, n_local, MPI_INT, 0, MPI_COMM_WORLD);
	
	// Cada proceso ordena su arreglo local
	qsort(datos_locales, n_local, sizeof(int), comparar);
	
	// Combinación en estructura de árbol
	int paso = 1;
	while (paso < tam_comunicacion) {
		if (rango % (2 * paso) == 0) {
			if (rango + paso < tam_comunicacion) {
				int tam_recibir = n_local * paso;
				int* datos_recibir = malloc(tam_recibir * sizeof(int));
				MPI_Recv(datos_recibir, tam_recibir, MPI_INT, rango + paso, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				
				// Combinar los datos recibidos con los datos locales
				int* datos_combinados = malloc((n_local + tam_recibir) * sizeof(int));
				combinar(datos_locales, n_local, datos_recibir, tam_recibir, datos_combinados);
				
				// Actualizar los datos locales y el tamaño
				free(datos_locales);
				datos_locales = datos_combinados;
				n_local += tam_recibir;
				free(datos_recibir);
			}
		} else {
			int destino = rango - paso;
			MPI_Send(datos_locales, n_local, MPI_INT, destino, 0, MPI_COMM_WORLD);
			break;
		}
		paso *= 2;
	}
	
	// El proceso 0 tiene los datos totalmente combinados y ordenados
	if (rango == 0) {
		printf("Datos ordenados:\n");
		fflush(stdout); // Asegura que el texto se imprima inmediatamente
		for (int i = 0; i < n; i++) {
			printf("%d ", datos_locales[i]);
		}
		printf("\n");
		fflush(stdout); // Asegura que el texto se imprima inmediatamente
		
		// Liberar el arreglo de datos globales
		free(datos_globales);
	}
	
	// Liberar datos locales
	free(datos_locales);
	
	MPI_Finalize();
	return 0;
}


