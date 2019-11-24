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

unsigned counter = 0;

char* sheets[20];

typedef struct cb_data {
    int x;
    char *filename;
    char *sheet;
} CB_Data; // this is just to experiment with passing data into callback


//my Datamap struct
typedef struct datamapline {
    unsigned row;
    unsigned col;
    char* filename;
    char* sheet;
    const char* value;
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
  printf("Sheet: - %s\n", name);
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
  // Current implementation only does a single sheet, so we need to loop this
  Datamapline *dmls = init_dml_data();
  // Just testing that we can pass in callbackdata from outside
  CB_Data *d = (CB_Data *)callbackdata;
  if (value) {
      if (counter < 450) {
          Datamapline dml;
          dml.filename = d->filename;
          dml.col = col;
          dml.row = row;
          dml.value = value;
          dml.sheet = d->sheet;
          dmls[counter] = dml;
      } else {
          dmls = realloc(dmls, (sizeof(Datamapline)) * 500);
      }
      counter++;
      printf("Counter: %d Value: %s Sheet: %s File: %s\n", counter, value, d->sheet,  d->filename);
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
  sheetdata.firstsheet = NULL;
  printf("Available sheets:\n");
  xlsxioread_list_sheets(xlsxioread, xlsx_list_sheets_callback, &sheetdata);

  /* TODO
   * Here we need to loop through each sheet and call xlsxioread_process in
   * each sheet.
   */
  struct cb_data toss = {.x = 10, .filename = filename, .sheet = sheetdata.firstsheet };
  xlsxioread_process(xlsxioread, sheetdata.firstsheet, XLSXIOREAD_SKIP_EMPTY_ROWS, sheet_cell_callback, sheet_row_callback, &toss);

  //clean up
  free(sheetdata.firstsheet);
  xlsxioread_close(xlsxioread);
  return 0;
}
