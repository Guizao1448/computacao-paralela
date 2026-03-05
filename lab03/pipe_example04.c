#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define VECTOR_SIZE 1000
#define NUM_PROCESSES 4

long long sum_chunk(int *vector, int start, int end) {
    long long sum = 0;
    for (int i = start; i < end; i++) {
        sum += vector[i];
    }
    return sum;
}

int main() {
    int pipes[NUM_PROCESSES][2];
    pid_t pid;
    int vector[VECTOR_SIZE];

    /* Initialize vector */
    for (int i = 0; i < VECTOR_SIZE; i++) {
        vector[i] = 1;
    }

    int chunk_size = VECTOR_SIZE / NUM_PROCESSES;
    long long total_sum = 0;

    for (int i = 0; i < NUM_PROCESSES; i++) {

        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid = fork();

        if (pid == 0) {  // CHILD
            close(pipes[i][0]); // child does not read

            for (int j = 0; j < NUM_PROCESSES; j++) {
                if (j != i) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }

            int start = i * chunk_size;
            int end = (i == NUM_PROCESSES - 1) ? VECTOR_SIZE : (i + 1) * chunk_size;

            long long partial_sum = sum_chunk(vector, start, end);

            printf("[FILHO %d] Soma parcial: %lld\n", getpid(), partial_sum);

            write(pipes[i][1], &partial_sum, sizeof(long long));
            close(pipes[i][1]);

            exit(0);
        }

        else if (pid > 0) {  // PARENT
            close(pipes[i][1]); // parent does not write
        }

        else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    /* Parent reads results */
    for (int i = 0; i < NUM_PROCESSES; i++) {
        long long partial_sum;

        ssize_t bytes_read = read(pipes[i][0], &partial_sum, sizeof(long long));
        if (bytes_read != sizeof(long long)) {
            fprintf(stderr, "Erro na leitura do pipe %d\n", i);
        }

        total_sum += partial_sum;
        close(pipes[i][0]);
    }

    /* Wait for children */
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    printf("Soma total: %lld\n", total_sum);

    return 0;
}