/**************************************************************
                Pontificia Universidad Javeriana
        Autor: Adriana Salazar
        Fecha: Febrero 2024
        Materia: Sistemas Operativos
        Tema: Taller de Evaluación de Rendimiento
        Fichero: fuente de multiplicación de matrices NxN por hilos.
        Objetivo: Evaluar el tiempo de ejecución del
                  algoritmo clásico de multiplicación de matrices.
                  Se implementa con la Biblioteca POSIX Pthreads
****************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Define el tamaño del buffer de memoria para las matrices
#define DATA_SIZE (1024*1024*64*3)

// Declaración del mutex para sincronización
pthread_mutex_t MM_mutex;

// Reservas estáticas de memoria para las matrices
static double MEM_CHUNK[DATA_SIZE];
double *mA, *mB, *mC;

// Estructura que contiene los parámetros para cada hilo
struct parametros{
        int nH;  // Número de hilos
        int idH; // ID del hilo actual
        int N;   // Dimensión de la matriz
};

// Variables para medir el tiempo
struct timeval start, stop;

/**
 * Nombre: llenar_matriz.
 * descripcion: Rellena las matrices mA y mB con valores predefinidos.
 * Argumentos SZ que es el tamaño de la matriz .
 */
void llenar_matriz(int SZ){
        srand48(time(NULL)); // Inicializa el generador de números aleatorios
        for(int i = 0; i < SZ*SZ; i++){
                mA[i] = 1.1 * i;  // Valores para matriz A
                mB[i] = 2.2 * i;  // Valores para matriz B
                mC[i] = 0;        // Inicializa matriz C en cero
        }
}

/**
 * Nombre: print_matrix
 * Descripcion: Imprime una matriz en la consola si el tamaño es pequeño.
 * Argumentos: sz que es el tamaño de la matriz y matriz que es un puntero a la matriz a imprimir.
*/
void print_matrix(int sz, double *matriz){
        if(sz < 12){ // Solo imprime si el tamaño es menor a 12x12
                for(int i = 0; i < sz*sz; i++){
                        if(i % sz == 0) printf("\n");
                        printf(" %.3f ", matriz[i]);
                }
                printf("\n>-------------------->\n");
        }
}

/*
 * Nombre: inicial_tiempo.
 * Descripcion: Inicia la medición del tiempo.
*/
void inicial_tiempo(){
        gettimeofday(&start, NULL);
}

/*
 * Nombre: final_tiempo.
 * Descripcion: Finaliza la medición del tiempo e imprime el resultado.
*/
void final_tiempo(){
        gettimeofday(&stop, NULL);
        stop.tv_sec -= start.tv_sec;
        printf("\n:-> %9.0f µs\n", (double) (stop.tv_sec * 1000000 + stop.tv_usec));
}

/*
 * Nombre: mult_thread.
 * Descripcion: Multiplicación de matrices por hilos utilizando metodo clasico.
 * Argumentos: variables Puntero a la estructura de parámetros.
*/
void *mult_thread(void *variables){
        struct parametros *data = (struct parametros *)variables;

        int idH = data->idH;  // ID del hilo
        int nH  = data->nH;   // Número de hilos
        int N   = data->N;    // Dimensión de la matriz

        // Determina el rango de filas que este hilo debe procesar
        int ini = (N / nH) * idH;
        int fin = (N / nH) * (idH + 1);

        for (int i = ini; i < fin; i++){
                for (int j = 0; j < N; j++){
                        double *pA = mA + (i * N);
                        double *pB = mB + j;
                        double sumaTemp = 0.0;

                        // Calcula el producto escalar de la fila i de mA con la columna j de mB
                        for (int k = 0; k < N; k++, pA++, pB += N){
                                sumaTemp += (*pA * *pB);
                        }
                        mC[i * N + j] = sumaTemp; // Almacena el resultado en mC
                }
        }

        // Bloquea y desbloquea el mutex.
        pthread_mutex_lock(&MM_mutex);
        pthread_mutex_unlock(&MM_mutex);
        pthread_exit(NULL);
}


int main(int argc, char *argv[]){
        if (argc < 3){
                printf("Ingreso de argumentos \n $./ejecutable tamMatriz numHilos\n");
                return -1;
        }

        int SZ = atoi(argv[1]);      // Tamaño de la matriz
        int n_threads = atoi(argv[2]); // Número de hilos

        pthread_t p[n_threads];       // Array de identificadores de hilos
        pthread_attr_t atrMM;         // Atributos de los hilos

        // Inicializa las matrices en un bloque de memoria compartido
        mA = MEM_CHUNK;
        mB = mA + SZ * SZ;
        mC = mB + SZ * SZ;

        // Rellena las matrices con datos
        llenar_matriz(SZ);
        print_matrix(SZ, mA);
        print_matrix(SZ, mB);

        // Inicia la medición del tiempo
        inicial_tiempo();

        // Inicializa los atributos y el mutex
        pthread_mutex_init(&MM_mutex, NULL);
        pthread_attr_init(&atrMM);
        pthread_attr_setdetachstate(&atrMM, PTHREAD_CREATE_JOINABLE);

        // Crea los hilos para la multiplicación de matrices
        for (int j = 0; j < n_threads; j++){
                struct parametros *datos = (struct parametros *) malloc(sizeof(struct parametros));
                datos->idH = j;
                datos->nH  = n_threads;
                datos->N   = SZ;
                pthread_create(&p[j], &atrMM, mult_thread, (void *)datos);
        }

        // Espera que todos los hilos terminen
        for (int j = 0; j < n_threads; j++)
                pthread_join(p[j], NULL);

        // Finaliza la medición del tiempo
        final_tiempo();

        // Imprime la matriz resultante
        print_matrix(SZ, mC);

        // Limpia recursos
        pthread_attr_destroy(&atrMM);
        pthread_mutex_destroy(&MM_mutex);
        pthread_exit(NULL);
}
