
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[]) {
	long long int numero_de_lanzamientos, lanzamiento;
	long long int numero_en_circulo = 0, numero_local_en_circulo = 0;
	int rango, tamano;
	double x, y, distancia_al_cuadrado, estimacion_pi;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rango);
	MPI_Comm_size(MPI_COMM_WORLD, &tamano);
	
	if (rango == 0) {
		// Proceso 0 lee el número de lanzamientos desde la entrada
		printf("Ingresa el numero total de lanzamientos: ");
		fflush(stdout);  // Asegura que el mensaje se muestre inmediatamente
		scanf("%lld", &numero_de_lanzamientos);
	}
	
	// Difunde el número de lanzamientos a todos los procesos
	MPI_Bcast(&numero_de_lanzamientos, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
	
	// Cada proceso calcula su parte de lanzamientos
	long long int lanzamientos_locales = numero_de_lanzamientos / tamano;
	unsigned int semilla = (unsigned int)(rango + 1); // Semilla única para cada proceso
	
	// Simulación de Monte Carlo para el número de lanzamientos locales
	for (lanzamiento = 0; lanzamiento < lanzamientos_locales; lanzamiento++) {
		x = (double)rand_r(&semilla) / RAND_MAX * 2.0 - 1.0;
		y = (double)rand_r(&semilla) / RAND_MAX * 2.0 - 1.0;
		distancia_al_cuadrado = x * x + y * y;
		if (distancia_al_cuadrado <= 1) numero_local_en_circulo++;
	}
	
	// Usa MPI_Reduce para sumar todos los conteos locales en numero_en_circulo en el proceso 0
	MPI_Reduce(&numero_local_en_circulo, &numero_en_circulo, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if (rango == 0) {
		// Calcular la estimación de \u03c0
		estimacion_pi = 4.0 * numero_en_circulo / ((double) numero_de_lanzamientos);
		printf("Valor estimado de \u03c0 = %f\n", estimacion_pi);
	}
	
	MPI_Finalize();
	return 0;
}


