#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "print_class.h"
#include "class_structs.h"

struct ClassFileBuffer {
    unsigned char *buffer;
    unsigned char *moving_buffer;
    int index;
};

u_int32_t parse_32(struct ClassFileBuffer *classFileBuffer) {
    u_int32_t result =  (classFileBuffer->moving_buffer[0] << 24) + (classFileBuffer->moving_buffer[1] << 16) + (classFileBuffer->moving_buffer[2] << 8) + classFileBuffer->moving_buffer[3];
    classFileBuffer->moving_buffer += 4;
    return result;
}

u_int16_t parse_16(struct ClassFileBuffer *classFileBuffer) {
    u_int16_t result = (classFileBuffer->moving_buffer[0] << 8) + classFileBuffer->moving_buffer[1];
    classFileBuffer->moving_buffer += 2;
    return result;
}

u_int8_t parse_8(struct ClassFileBuffer *classFileBuffer) {
    uint8_t result = classFileBuffer->moving_buffer[0];
    classFileBuffer->moving_buffer += 1;
    return result;
}

char *convert_to_string(struct ClassFileBuffer *classFileBuffer, int length, int is_print) {
    char *message = malloc(length + 1);
    for (size_t i = 0; i < length; i++) {
        message[i] = classFileBuffer->moving_buffer[i];
    }
    message[length] = '\0';
    classFileBuffer->moving_buffer += length;
    return message;
}

void parse_meta_fields(struct ClassFileBuffer *fileBuffer, ClassFile *classFile) {
    classFile->magic = parse_32(fileBuffer);
    classFile->version_minor = parse_16(fileBuffer);
    classFile->version_major = parse_16(fileBuffer);
}

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

void parse_constant_pool(struct ClassFileBuffer *fileBuffer, ClassFile *classFile) {
    classFile->constant_pool = malloc(sizeof(ConstantPoolInfo) * classFile->constant_pool_count);
    for (size_t index = 0; index < classFile->constant_pool_count - 1; index++) {
        ConstantPoolInfo *cp_info = malloc(sizeof(ConstantPoolInfo));
        int tag = parse_8(fileBuffer);
        cp_info->tag = tag;
        switch (tag) {
            case 0:
                printf("tag 0 is wrong \n");
                break;
            case CONSTANT_UTF8:
                ;
                int utf8_length = parse_16(fileBuffer);
                char *message = convert_to_string(fileBuffer, utf8_length, 0);
                cp_info->utf8_length = utf8_length;
                cp_info->bytes = message;
                break;
            case 2:
            case CONSTANT_INTEGER:
            case CONSTANT_FLOAT:
            case CONSTANT_LONG:
            case CONSTANT_DOUBLE:
                printf("tag %d is not implemented yet", tag);
                break;
            case CONSTANT_CLASS:
                ;
                cp_info->name_index = parse_16(fileBuffer);
                break;
            case CONSTANT_STRING:
                ;
                cp_info->string_index = parse_16(fileBuffer);
                break;
            case CONSTANT_FIELDREF:
                ;
                cp_info->class_index = parse_16(fileBuffer);
                cp_info->name_and_type_index = parse_16(fileBuffer);
                break;
            case CONSTANT_METHODREF:
                ;
                cp_info->class_index = parse_16(fileBuffer);
                cp_info->name_and_type_index = parse_16(fileBuffer);
                break;
            case CONSTANT_INTERFACE_METHODREF:
                ;
                cp_info->class_index = parse_16(fileBuffer);
                cp_info->name_and_type_index = parse_16(fileBuffer);
                break;
            case CONSTANT_NAME_AND_TYPE:
                ;
                cp_info->name_index = parse_16(fileBuffer);
                cp_info->descriptor_index = parse_16(fileBuffer);
                break;
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
                printf("tag %d is not implemented yet", tag);
                break;
            default:
                printf("tag is unknown %d \n", tag);
                break;
        }
        classFile->constant_pool[index] = *cp_info;
    }
}

void parseCodeAttributes(MethodInfo *methodInfo, struct ClassFileBuffer *fileBuffer) {
        for (size_t attribute_i = 0; attribute_i < methodInfo->attributes_count; attribute_i++) {
            CodeAttribute *attribute = malloc(sizeof(CodeAttribute));
            attribute->attribute_name_index = parse_16(fileBuffer);
            attribute->attribute_length = parse_32(fileBuffer);
            attribute->max_stack = parse_16(fileBuffer);
            attribute->max_locals = parse_16(fileBuffer);
            attribute->code_length = parse_32(fileBuffer);
            unsigned char attribute_info[attribute->code_length];
            for (size_t code_i = 0; code_i < attribute->code_length; code_i++) {
                attribute_info[code_i] = fileBuffer->moving_buffer[code_i];
            }
            fileBuffer->moving_buffer += attribute->code_length;
            attribute->exception_table_length = parse_16(fileBuffer);
            fileBuffer += attribute->exception_table_length;
            attribute->attributes_count = parse_16(fileBuffer);
    
            methodInfo->code_attributes[attribute_i] = *attribute;
        }
}

int main(int argc, char *argv[]) {
    FILE *java_class_pointer;
    long filelen;

    java_class_pointer = fopen("Main.class", "rb");
    fseek(java_class_pointer, 0, SEEK_END);
    filelen = ftell(java_class_pointer);
    rewind(java_class_pointer);

    struct ClassFileBuffer *fileBuffer = malloc(sizeof(struct ClassFileBuffer));
    fileBuffer->buffer = (unsigned char *)malloc(filelen * sizeof(unsigned char));
    fread(fileBuffer->buffer, filelen, 1, java_class_pointer);
    fileBuffer->moving_buffer = &fileBuffer->buffer[0];

    fclose(java_class_pointer);
    
    ClassFile *classFile = malloc(sizeof(ClassFile));

    parse_meta_fields(fileBuffer, classFile);

    classFile->constant_pool_count = parse_16(fileBuffer);

    parse_constant_pool(fileBuffer, classFile);

    classFile->access_flags = parse_16(fileBuffer);
    
    classFile->this_class = parse_16(fileBuffer);

    classFile->super_class = parse_16(fileBuffer);

    classFile->interfaces_count = parse_16(fileBuffer);

    if (classFile->interfaces_count > 0) {
        printf("interfaces are not supported yet \n");
        return 0;
    }

    classFile->interfaces = parse_16(fileBuffer);

    classFile->methods_count = parse_16(fileBuffer);
    classFile->methods = malloc(sizeof(MethodInfo) *classFile->methods_count);

    for (size_t method_index = 0; method_index < classFile->methods_count; method_index++) {
        MethodInfo *methodInfo = malloc(sizeof(MethodInfo));
        methodInfo->access_flags = parse_16(fileBuffer);
        methodInfo->name_index = parse_16(fileBuffer);
        methodInfo->descriptor_index = parse_16(fileBuffer);
        methodInfo->attributes_count = parse_16(fileBuffer);
        methodInfo->attribute_info = malloc(sizeof(AttributeInfo *) * methodInfo->attributes_count);
        //methodInfo->code_attributes = malloc(sizeof(struct CodeAttribute) * methodInfo->attributes_count);

        printf("method: access_flags %d; name_index %d; descriptor_index %d; attributes_count %d \n", methodInfo->access_flags, methodInfo->name_index, methodInfo->descriptor_index, methodInfo->attributes_count);
        
        for(size_t attr_i = 0; attr_i < methodInfo->attributes_count; attr_i++) {
            AttributeInfo *attribute = malloc(sizeof(AttributeInfo));
            attribute->attribute_name_index = parse_16(fileBuffer);
            attribute->attribute_length = parse_32(fileBuffer);
            attribute->info = malloc(sizeof(char) * attribute->attribute_length);
            memcpy(attribute->info, fileBuffer->moving_buffer, attribute->attribute_length);
            methodInfo->attribute_info[attr_i] = *attribute;
            fileBuffer->moving_buffer += attribute->attribute_length;
        }
        classFile->methods[method_index] = *methodInfo;
    }

    classFile->attributes_count = parse_16(fileBuffer);

    print_class(classFile);

    return 0;
}
