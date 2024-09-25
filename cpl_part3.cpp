#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void multiplyMatricesBlock(int *matA, int *matB, int *matC, int n, int tam_bloque) {
	int i, j, k, ii, jj, kk;
	
	for (ii = 0; ii < n; ii += tam_bloque) {
		for (jj = 0; jj < n; jj += tam_bloque) {
			for (kk = 0; kk < n; kk += tam_bloque) {
				// Multiplicación de bloques
				for (i = ii; i < ii + tam_bloque && i < n; i++) {
					for (j = jj; j < jj + tam_bloque && j < n; j++) {
						int suma = 0;
						for (k = kk; k < kk + tam_bloque && k < n; k++) {
							suma += matA[i * n + k] * matB[k * n + j];
						}
						matC[i * n + j] += suma;
					}
				}
			}
		}
	}
}

int main() {
	int n = 1000; // Tamaño de la matriz
	int tam_bloque = 50; // Tamaño del bloque
	int i, j;
	
	// Asignación dinámica de matrices como bloques contiguos de memoria
	int *matA = (int *)malloc(n * n * sizeof(int));
	int *matB = (int *)malloc(n * n * sizeof(int));
	int *matC = (int *)malloc(n * n * sizeof(int));
	
	if (matA == NULL || matB == NULL || matC == NULL) {
		printf("Error al asignar memoria\n");
		return 1;
	}
	
	// Inicializar matrices con valores aleatorios
	srand(time(NULL));
	for (i = 0; i < n * n; i++) {
		matA[i] = rand() % 10;
		matB[i] = rand() % 10;
		matC[i] = 0; // Inicializamos matC a 0 para evitar valores basura
	}
	
	clock_t start = clock();
	
	// Multiplicación de matrices por bloques
	multiplyMatricesBlock(matA, matB, matC, n, tam_bloque);
	
	clock_t end = clock();
	double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
	
	printf("Tiempo de ejecucion (multiplicación por bloques): %f segundos\n", time_taken);
	
	// Liberar memoria
	free(matA);
	free(matB);
	free(matC);
	
	return 0;
}



