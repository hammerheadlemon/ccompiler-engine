#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xlsxio_read.h>

#if !defined(XML_UNICODE_WCHAR_T) && !defined(XML_UNICODE)
//UTF-8 version
#define X(s) s
#define XML_Char_printf printf
#else
//UTF-16 version
#define X(s) L##s
#define XML_Char_printf wprintf
#endif

char *filename = "/home/lemon/code/c/xlsx/test_template.xlsx";


//my Datamap struct
typedef struct datamapline {
    char* filename;
    char* sheet;
    char* value;
}Datamapline;

//calback data structure for listing sheets
struct xlsx_list_sheets_data {
  char* firstsheet;
};
//
//
//calback function for listing sheets
int xlsx_list_sheets_callback (const char* name, void* callbackdata)
{
  struct xlsx_list_sheets_data* data = (struct xlsx_list_sheets_data*)callbackdata;
  if (!data->firstsheet)
    data->firstsheet = strdup(name);
  printf(" - %s\n", name);
  return 0;
}


//calback function for end of row
int sheet_row_callback (size_t row, size_t maxcol, void* callbackdata)
{
  printf("\n");
  return 0;
}

//calback function for cell data
int sheet_cell_callback (size_t row, size_t col, const XLSXIOCHAR* value, void* callbackdata)
{
  if (col > 1)
    printf("\t");
  if (value)
    printf("Row: %lu Col:%lu Value:%s",row, col, value);
  return 0;
}

int main (int argc, char* argv[])
{
  xlsxioreader xlsxioread;
  //open .xlsx file for reading
  if ((xlsxioread = xlsxioread_open(filename)) == NULL) {
    fprintf(stderr, "Error opening .xlsx file\n");
    return 1;
  }
  //list available sheets
  struct xlsx_list_sheets_data sheetdata;
  sheetdata.firstsheet = NULL;
  printf("Available sheets:\n");
  xlsxioread_list_sheets(xlsxioread, xlsx_list_sheets_callback, &sheetdata);

  //read data
  xlsxioread_process(xlsxioread, sheetdata.firstsheet, XLSXIOREAD_SKIP_EMPTY_ROWS, sheet_cell_callback, sheet_row_callback, NULL);

  //clean up
  free(sheetdata.firstsheet);
  xlsxioread_close(xlsxioread);
  return 0;
}
