#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xlsxio_read.h>

char *filename = "test_template.xlsx";

unsigned count = 0;

int list_sheets_callback(const char *sheetname, void *callbackdata) {
    char **d = (char **)callbackdata;
    strcpy(d[count], sheetname); // CRUCIAL - I was mistakenly doing d[count] = sheetname here. WRONG.
    printf("Sheetname in sheets array is: %s\n", d[count]);
    count++;
    return 0;
}

int main(int argc, char *argv[])
{
    char **sheets = malloc(sizeof(char*) * 20);
    // This seems to be one way to do it...
    /*
     * Or we could do it when we want to opulate the array with
     * for...
     *  d[i] = (char *)malloc(sizeof(string)); // when we know the string
     */
    for (int i = 0; i < 20; i++) {
        sheets[i] = malloc((20+1) * sizeof(char));
    }

    xlsxioreader reader;
    if ((reader = xlsxioread_open(filename)) == NULL) {
        fprintf(stderr, "Cannot open file.\n");
        return 1;
    }
    xlsxioread_list_sheets(reader, list_sheets_callback, sheets);
    printf("First entry in sheets in main after xlsxioread_list_sheets is %s\n", sheets[0]);
    for (int i=0; i < 5; i++) { // 5 is wrong here
        printf("%s\n", sheets[i]);
    }
    return 0;
}
