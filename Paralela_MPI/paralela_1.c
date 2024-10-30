#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	int cantidad_datos, cantidad_bins;
	float min_medida, max_medida;
	float *datos = NULL;
	int *hist_local = NULL, *histograma = NULL;
	int rank, size, i;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	if (rank == 0) {
		// El proceso 0 lee los datos de entrada
		printf("Ingrese la cantidad de mediciones: ");
		fflush(stdout);
		scanf("%d", &cantidad_datos);
		
		datos = malloc(cantidad_datos * sizeof(float));
		printf("Ingrese los datos (separados por espacios): ");
		fflush(stdout);
		for (i = 0; i < cantidad_datos; i++) {
			scanf("%f", &datos[i]);
		}
		
		printf("Ingrese la medicion minima, maxima y la cantidad de bins (separados por espacios): ");
		fflush(stdout);
		scanf("%f %f %d", &min_medida, &max_medida, &cantidad_bins);
		
		histograma = malloc(cantidad_bins * sizeof(int));
		for (i = 0; i < cantidad_bins; i++) histograma[i] = 0;
	}
	
	// Difunde la cantidad de bins, mínimo, máximo y cantidad de datos a todos los procesos
	MPI_Bcast(&cantidad_datos, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&min_medida, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&max_medida, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&cantidad_bins, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int cantidad_datos_local = cantidad_datos / size;
	float *datos_locales = malloc(cantidad_datos_local * sizeof(float));
	
	// Distribuye los datos entre los procesos
	MPI_Scatter(datos, cantidad_datos_local, MPI_FLOAT, datos_locales, cantidad_datos_local, MPI_FLOAT, 0, MPI_COMM_WORLD);
	
	// Asigna memoria para el histograma local
	hist_local = malloc(cantidad_bins * sizeof(int));
	for (i = 0; i < cantidad_bins; i++) hist_local[i] = 0;
	
	// Calcula el ancho del bin
	float ancho_bin = (max_medida - min_medida) / cantidad_bins;
	
	// Cada proceso calcula su histograma local
	for (i = 0; i < cantidad_datos_local; i++) {
		int indice_bin = (datos_locales[i] - min_medida) / ancho_bin;
		if (indice_bin >= 0 && indice_bin < cantidad_bins) {
			hist_local[indice_bin]++;
		}
	}
	
	// Reúne todos los histogramas locales en el histograma global en el proceso 0
	MPI_Reduce(hist_local, histograma, cantidad_bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	// El proceso 0 imprime el histograma final en formato de texto
	if (rank == 0) {
		printf("\nHistograma:\n");
		
		// Imprimir cada bin con una barra de asteriscos que corresponde al valor exacto
		for (i = 0; i < cantidad_bins; i++) {
			printf("Bin %d | ", i);
			
			// Imprimir un asterisco por cada unidad en el bin
			for (int j = 0; j < histograma[i]; j++) {
				printf("*");
			}
			printf(" (%d)\n", histograma[i]); // Muestra el conteo al final de la barra
		}
		
		free(datos);
		free(histograma);
	}
	
	// Libera la memoria
	free(datos_locales);
	free(hist_local);
	
	MPI_Finalize();
	return 0;
}
