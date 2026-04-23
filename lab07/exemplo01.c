#include <stdio.h>
#include <omp.h>

int main() {
	printf("--- Fora da regiao paralela ---\n");
	printf("Numero de processadores disponiveis: %d\n", omp_get_num_procs());
	printf("Numero de threads (antes de definir): %d\n", omp_get_num_threads());
	
	// Define o numero de threads para a proxima regiao paralela
	omp_set_num_threads(4);
	
	#pragma omp parallel
	{
		// Este bloco sera executado por 4 threads
		
		// A diretiva 'master' garante que este trecho so seja executado pela thread 0
		#pragma omp master
		{
			printf("\n--- Dentro da regiao paralela ---\n");
			printf("Numero de threads na equipe: %d\n", omp_get_num_threads());
		}
		
		// Garante que todas as threads esperem a master imprimir antes de continuar
		#pragma omp barrier 
		
		// Cada thread imprime seu proprio ID
		// Usamos 'critical' para evitar que as saidas se misturem no console
		#pragma omp critical
		{
			printf("Ola! Eu sou a thread %d de %d.\n", 
			omp_get_thread_num(), omp_get_num_threads());
		}
	}
	
	printf("\n--- Fora da regiao paralela de novo ---\n");
	printf("Numero de threads: %d\n", omp_get_num_threads());
	
	return 0;
}