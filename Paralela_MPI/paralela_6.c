#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
	int rango, tamano_comunicador, raiz_p;
	int N, N_local;
	double *matriz = NULL, *vector = NULL, *matriz_local = NULL, *vector_local = NULL;
	double *resultado_local = NULL, *resultado = NULL;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rango);
	MPI_Comm_size(MPI_COMM_WORLD, &tamano_comunicador);
	
	// Determinar la raíz cuadrada de tamano_comunicador
	raiz_p = (int)sqrt(tamano_comunicador);
	if (raiz_p * raiz_p != tamano_comunicador) {
		if (rango == 0) {
			fprintf(stderr, "tamano_comunicador debe ser un cuadrado perfecto\n");
		}
		MPI_Finalize();
		exit(1);
	}
	
	if (rango == 0) {
		// Orden de la matriz N
		printf("Ingrese el orden de la matriz (N): ");
		fflush(stdout); // Asegurarse de que el mensaje se muestre
		scanf("%d", &N);
		
		// Verificar si N es divisible por raiz_p
		if (N % raiz_p != 0) {
			fprintf(stderr, "N debe ser divisible por raiz(tamano_comunicador)\n");
			MPI_Finalize();
			exit(1);
		}
		
		// Inicializar matriz y vector
		matriz = malloc(N * N * sizeof(double));
		vector = malloc(N * sizeof(double));
		resultado = malloc(N * sizeof(double));
		
		printf("Ingrese los elementos de la matriz:\n");
		fflush(stdout); // Asegurarse de que el mensaje se muestre
		for (int i = 0; i < N * N; i++) {
			scanf("%lf", &matriz[i]);
		}
		
		printf("Ingrese los elementos del vector:\n");
		fflush(stdout); // Asegurarse de que el mensaje se muestre
		for (int i = 0; i < N; i++) {
			scanf("%lf", &vector[i]);
		}
	}
	
	// Difundir el orden de la matriz N a todos los procesos
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	N_local = N / raiz_p; // Tamaño del bloque local para cada proceso
	
	// Asignar memoria para la matriz local, vector local y segmento de resultado
	matriz_local = malloc(N_local * N_local * sizeof(double));
	vector_local = malloc(N_local * sizeof(double));
	resultado_local = malloc(N_local * sizeof(double));
	
	// Distribuir los sub-bloques de la matriz a todos los procesos
	if (rango == 0) {
		double *matriz_temp = malloc(N_local * N_local * tamano_comunicador * sizeof(double));
		for (int i = 0; i < raiz_p; i++) {
			for (int j = 0; j < raiz_p; j++) {
				for (int k = 0; k < N_local; k++) {
					for (int l = 0; l < N_local; l++) {
						matriz_temp[((i * raiz_p + j) * N_local * N_local) + k * N_local + l] =
							matriz[(i * N_local + k) * N + j * N_local + l];
					}
				}
			}
		}
		MPI_Scatter(matriz_temp, N_local * N_local, MPI_DOUBLE, matriz_local, N_local * N_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		free(matriz_temp);
	} else {
		MPI_Scatter(NULL, N_local * N_local, MPI_DOUBLE, matriz_local, N_local * N_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	
	// Distribuir los bloques del vector a los procesos diagonales
	if (rango % (raiz_p + 1) == 0) {
		MPI_Scatter(vector, N_local, MPI_DOUBLE, vector_local, N_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	} else {
		MPI_Scatter(NULL, N_local, MPI_DOUBLE, vector_local, N_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	}
	
	// Realizar la multiplicación local matriz-vector
	for (int i = 0; i < N_local; i++) {
		resultado_local[i] = 0.0;
		for (int j = 0; j < N_local; j++) {
			resultado_local[i] += matriz_local[i * N_local + j] * vector_local[j];
		}
	}
	
	// Reunir los resultados parciales en el proceso 0
	MPI_Gather(resultado_local, N_local, MPI_DOUBLE, resultado, N_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	// El proceso 0 imprime el resultado
	if (rango == 0) {
		printf("Resultado de la multiplicacion matriz-vector:\n");
		fflush(stdout); // Asegurarse de que el mensaje se muestre
		for (int i = 0; i < N; i++) {
			printf("%f\n", resultado[i]);
		}
		
		free(matriz);
		free(vector);
		free(resultado);
	}
	
	free(matriz_local);
	free(vector_local);
	free(resultado_local);
	
	MPI_Finalize();
	return 0;
}
