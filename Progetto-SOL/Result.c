#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Result.h>

// Funzione di confronto per l'ordinamento
int compare(const void *a, const void *b) {
    long aValue = ((Element *)a)->value;
    long bValue = ((Element *)b)->value;

    if (aValue < bValue) return -1;
    if (aValue > bValue) return 1;
    return 0;
}

// Funzione per aggiungere un elemento alla struttura dati
void addElement(ResultStructure *result, long value, const char *str) {
    result->size++;
    result->elements = realloc(result->elements, result->size * sizeof(Element));
    if (result->elements == NULL) {
        fprintf(stderr, "Errore: Allocazione di memoria fallita.\n");
        exit(1); 
    }
    result->elements[result->size - 1].value = value;

    // Alloco memoria e copio la stringa
    result->elements[result->size - 1].str = (char *)malloc(strlen(str) + 1);
    if (result->elements[result->size - 1].str == NULL) {
        fprintf(stderr, "Errore: Allocazione di memoria per la stringa fallita.\n");
        exit(1); 
    }
    strcpy(result->elements[result->size - 1].str, str);
}

// Funzione per ordinare gli elementi della struttura dati
void sortElements(ResultStructure *result) {
    qsort(result->elements, result->size, sizeof(Element), compare);
}

// Funzione per liberare la memoria degli elementi
void freeResultStructure(ResultStructure *result) {
    for (int i = 0; i < result->size; i++) {
        free(result->elements[i].str);
    }
    free(result->elements);
}

// Funzione per stampare gli elementi della struttura dati
void PrintResults(const ResultStructure *result) {
    for (int i = 0; i < result->size; i++) {
        printf("%ld %s\n", result->elements[i].value, result->elements[i].str);
    }
}
