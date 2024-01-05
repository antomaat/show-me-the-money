#ifndef QUERY_UTILS_H
#define QUERY_UTILS_H

#include "class_structs.h"

char *get_string(ClassFile *classFile, int index);
char *get_class_name(ClassFile *classFile, int index);
char *get_name_and_type_name(ClassFile *classFile, int index);
char *get_name_and_type_type(ClassFile *classFile, int index);
int is_code_attribute(ClassFile *classFile, AttributeInfo attributeInfo);

#endif
