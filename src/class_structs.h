#ifndef CLASS_STRUCTS_H
#define CLASS_STRUCTS_H

typedef struct {
    int tag;
    int class_index;
    int name_index;
    int descriptor_index;
    int name_and_type_index;
    int string_index;
    int utf8_length;
    char *bytes;
} ConstantPoolInfo;

typedef struct {
    int attribute_name_index;
    int attribute_length;
    unsigned char *info;
} AttributeInfo;

typedef struct  {
    int attribute_name_index;
    int attribute_length;
    int max_stack;
    int max_locals;
    int code_length;
    int exception_table_length;
    int exception_table;
    int attributes_count;
    AttributeInfo attributes;
} CodeAttribute;

typedef struct {
    int access_flags;
    int name_index;
    int descriptor_index;
    int attributes_count;
    AttributeInfo *attribute_info;
    CodeAttribute *code_attributes;
} MethodInfo;


typedef struct  {
    int magic;
    int version_minor;
    int version_major;
    int constant_pool_count;
    ConstantPoolInfo *constant_pool;
    int access_flags;
    int this_class;
    int super_class;
    int interfaces_count;
    int interfaces;
    int methods_count;
    MethodInfo *methods;
    int attributes_count;
} ClassFile;

#endif
