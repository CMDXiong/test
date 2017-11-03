#ifndef QLAYER
#define QLAYER

#include <QFile>
#include <QDebug>
#include <QString>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define NO_SPACES_TO_INDENT 2

void find_ascii_record_type (int numeric, char *record_name, char *record_description, int *expected_data_type);
void find_ascii_data_type (int numeric, char *data_name, char *data_description);
void print_bit_array_comments (char *ascii_record_type, int bit_array, char *indent_string);
QString qLayer_num();

#endif // QLAYER

