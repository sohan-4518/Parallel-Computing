#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define MIN_SIZE 1000

void merge(int a[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = a[l + i];
    for (int j = 0; j < n2; j++) R[j] = a[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        a[k++] = L[i] <= R[j] ? L[i++] : R[j++];
    while (i < n1) a[k++] = L[i++];
    while (j < n2) a[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSortSequential(int a[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSortSequential(a, l, m);
        mergeSortSequential(a, m + 1, r);
        merge(a, l, m, r);
    }
}

void mergeSortParallel(int a[], int l, int r) {
    if (r - l + 1 <= MIN_SIZE) {
        mergeSortSequential(a, l, r);
        return;
    }

    if (l < r) {
        int m = l + (r - l) / 2;
        #pragma omp parallel sections
        {
            #pragma omp section
            mergeSortParallel(a, l, m);

            #pragma omp section
            mergeSortParallel(a, m + 1, r);
        }
        merge(a, l, m, r);
    }
}

int main(void) {
    int n = 100000;
    int *arr_seq = malloc(n * sizeof(int));
    int *arr_par = malloc(n * sizeof(int));

    if (!arr_seq || !arr_par) {
        printf("Memory allocation failed.\n");
        free(arr_seq);
        free(arr_par);
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < n; i++)
        arr_seq[i] = arr_par[i] = rand() % 10000;

    printf("Sorting %d elements...\n\n", n);

    double start_time = omp_get_wtime();
    mergeSortSequential(arr_seq, 0, n - 1);
    double time_seq = omp_get_wtime() - start_time;
    printf("Sequential Merge Sort Time: %f seconds\n", time_seq);

    start_time = omp_get_wtime();
    mergeSortParallel(arr_par, 0, n - 1);
    double time_par = omp_get_wtime() - start_time;
    printf("Parallel Merge Sort Time: %f seconds\n", time_par);

    printf("\nDifference (Sequential - Parallel): %f seconds\n",
           time_seq - time_par);
    if (time_par > 0)
        printf("Speedup: %.2fx\n", time_seq / time_par);

    printf("\nVerification: First 20 elements of the sorted array:\n");
    for (int i = 0; i < 20 && i < n; i++)
        printf("%d ", arr_par[i]);
    printf("\n");

    free(arr_seq);
    free(arr_par);
    return 0;
}
