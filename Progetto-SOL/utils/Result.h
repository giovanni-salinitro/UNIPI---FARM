#ifndef RESULT
#define RESULT


typedef struct {
    long value;
    char *str;
} Element;

typedef struct {
    Element *elements;
    int size;
} ResultStructure;


int compare(const void *a, const void *b);
void addElement(ResultStructure *result, long value, const char *str);
void sortElements(ResultStructure *result);
void freeResultStructure(ResultStructure *result);
void PrintResults(const ResultStructure *result);

#endif