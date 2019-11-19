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

int counter = 0;

typedef struct cb_data {int x; int y;} CB_Data; // this is just to experiment with passing data into callback


//my Datamap struct
typedef struct datamapline {
    int row;
    int col;
    char* filename;
    char* sheet;
    char* value;
}Datamapline;


Datamapline* init_dml_data() {
    Datamapline *dmls = malloc(sizeof(Datamapline) * 500);
    return dmls;
}

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
//  printf("At the end of the row.\n");
  return 0;
}

//callback function for cell data
int sheet_cell_callback (size_t row, size_t col, const XLSXIOCHAR* value, void* callbackdata)
{
  // TODO this needs to get sent in inside collbackdata
  Datamapline *dmls = init_dml_data();
  // Just testing that we can pass in callbackdata from outside
  CB_Data *d = (CB_Data *)callbackdata;
  if (value) {
      if (counter < 450) {
          Datamapline dml;
          dml.filename = "fulf";
          dml.col = col;
          dml.row = row;
          dml.value = value;
          dml.sheet = "SHEET";
          dmls[counter] = dml;
      } else {
          dmls = realloc(dmls, (sizeof(Datamapline)) * 500);
      }
      counter++;
      printf("Counter: %d\n", counter);
  }
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
  struct cb_data toss = {.x = 10, .y = 20 };
  sheetdata.firstsheet = NULL;
  printf("Available sheets:\n");
  xlsxioread_list_sheets(xlsxioread, xlsx_list_sheets_callback, &sheetdata);

  //read data
  xlsxioread_process(xlsxioread, sheetdata.firstsheet, XLSXIOREAD_SKIP_EMPTY_ROWS, sheet_cell_callback, sheet_row_callback, &toss);

  //clean up
  free(sheetdata.firstsheet);
  xlsxioread_close(xlsxioread);
  return 0;
}
