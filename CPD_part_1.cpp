#include <iostream>
#include <vector>
#include <chrono>

#define MAX 1000

int main() {
	// Crear e inicializar las matrices
	std::vector<std::vector<double>> A(MAX, std::vector<double>(MAX, 1.0));
	std::vector<double> x(MAX, 1.0), y(MAX, 0.0);
	
	// Medir el tiempo del primer par de bucles
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++) {
			y[i] += A[i][j] * x[j];
		}
	}
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "Tiempo del primer par de bucles: " << duration.count() << " ms" << std::endl;
	
	// Reiniciar el vector y
	std::fill(y.begin(), y.end(), 0.0);
	
	// Medir el tiempo del segundo par de bucles
	start = std::chrono::high_resolution_clock::now();
	for (int j = 0; j < MAX; j++) {
		for (int i = 0; i < MAX; i++) {
			y[i] += A[i][j] * x[j];
		}
	}
	stop = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	std::cout << "Tiempo del segundo par de bucles: " << duration.count() << " ms" << std::endl;
	
	return 0;
}
