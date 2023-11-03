#include <string.h>
#include <stdio.h>

#include "class_structs.h"
#include "print_class.h"
#include "query_utils.h"



#define CONSTANT_UTF8 1
#define CONSTANT_INTEGER 3
#define CONSTANT_FLOAT 4
#define CONSTANT_LONG 5
#define CONSTANT_DOUBLE 6
#define CONSTANT_CLASS 7
#define CONSTANT_STRING 8
#define CONSTANT_FIELDREF 9
#define CONSTANT_METHODREF 10
#define CONSTANT_INTERFACE_METHODREF 11
#define CONSTANT_NAME_AND_TYPE 12

char *get_string(ClassFile *classFile, int index);
char *get_class_name(ClassFile *classFile, int index);
char *get_name_and_type_name(ClassFile *classFile, int index);
char *get_name_and_type_type(ClassFile *classFile, int index);


void print_class(ClassFile *classFile) {
    printf("-----------------------------\n");
    printf("magic number: %x \n", classFile->magic);
    printf("version: %d.%d \n", classFile->version_major, classFile->version_minor);
    printf("constant_pool_count: %d\n", classFile->constant_pool_count);
    printf("access_flags %d \n", classFile->access_flags);
    printf("this_class %s \n",  get_class_name(classFile, classFile->this_class));
    printf("super_class %s \n", get_class_name(classFile, classFile->super_class));

    print_cp_with_tag(classFile, CONSTANT_CLASS);
    //print_all_cp_tags(classFile);
    //print_all_cp_tags(classFile);
    //for (size_t i = 0; i < classFile->constant_pool_count; i++) {
    //    print_cp_index(classFile, i);
    //}
//    printf("interface count %d \n", classFile->interfaces_count);
//    printf("interfaces %d \n", classFile->interfaces);
//    printf("method count %d \n", classFile->methods_count);
//    printf("attributes count %d \n", classFile->attributes_count);
}

void print_cp_index(ClassFile *classFile, int index) {
    ConstantPoolInfo cp_info = classFile->constant_pool[index];
    switch (cp_info.tag) {
        case CONSTANT_METHODREF:
            printf("tag is CONSTANT_METHODREF \n");
            printf("    class_name: %s\n", get_class_name(classFile, cp_info.class_index));
            printf("    name and type: <%s : %s>\n", 
                   get_name_and_type_name(classFile, cp_info.name_and_type_index), 
                   get_name_and_type_type(classFile, cp_info.name_and_type_index));
            break;
        case CONSTANT_CLASS:
            printf("tag is CONSTANT_CLASS \n");
            printf("    class name: %s \n", get_string(classFile, cp_info.name_index));
            break;
        case CONSTANT_UTF8:
                printf("tag is CONSTANT_UTF8 \n");
                printf("    string: %s \n", cp_info.bytes);
                break;
        case CONSTANT_STRING:
            printf("tag is CONSTANT_STRING \n");
            printf("    string: %s\n", get_string(classFile, cp_info.string_index));
            break;
        case CONSTANT_FIELDREF:
            ;
            printf("tag is CONSTANT_FIELDREF \n");
            printf("    class_name: %s\n", get_class_name(classFile, cp_info.class_index));
            printf("    name and type: <%s : %s>\n", 
                   get_name_and_type_name(classFile, cp_info.name_and_type_index), 
                   get_name_and_type_type(classFile, cp_info.name_and_type_index));
            break;
        case CONSTANT_NAME_AND_TYPE:
            printf("tag is CONSTANT_NAME_AND_TYPE \n");
            printf("    name and type: <%s : %s>\n", 
                   get_string(classFile, cp_info.name_index), 
                   get_string(classFile, cp_info.descriptor_index));
            break;
        default:
            break;
            //printf("tag %d in index: %d \n", cp_info.tag, cp_info.class_index);
    }
}

void print_all_cp_tags(ClassFile *classFile) {
    for (size_t i = 0; i < classFile->constant_pool_count; i++) {
        print_cp_index(classFile, i);
    }
}

void print_cp_with_tag(ClassFile *classFile, int tag) {
    for (size_t i = 0; i < classFile->constant_pool_count; i++) {
        if (classFile->constant_pool[i].tag == tag) {
            print_cp_index(classFile, i);
        }
    }
}

