#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void multiplyMatrices(int *matA, int *matB, int *matC, int n) {
	int i, j, k;
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			matC[i * n + j] = 0;
			for (k = 0; k < n; k++) {
				matC[i * n + j] += matA[i * n + k] * matB[k * n + j];
			}
		}
	}
}

int main() {
	int n = 100; // Tamaño de la matriz
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
	}
	
	clock_t start = clock();
	
	// Multiplicación de matrices
	multiplyMatrices(matA, matB, matC, n);
	
	clock_t end = clock();
	double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
	
	printf("Tiempo de ejecucion: %f segundos\n", time_taken);
	
	// Liberar memoria
	free(matA);
	free(matB);
	free(matC);
	
	return 0;
}

