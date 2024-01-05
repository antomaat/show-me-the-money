#ifndef PRINT_CLASS_H
#define PRINT_CLASS_H

#include "class_structs.h"

void print_class(ClassFile *classFile);
void print_cp_index(ClassFile *classFile, int index);
void print_all_cp_tags(ClassFile *classFile);
void print_cp_with_tag(ClassFile *classFile, int tag);
void print_code_attribute(ClassFile *classFile, AttributeInfo attributeInfo);

#endif
