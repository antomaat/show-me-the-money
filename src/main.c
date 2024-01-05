#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "print_class.h"
#include "class_structs.h"
#include "parser.h"


int main(int argc, char *argv[]) {
    FILE *java_class_pointer;
    long filelen;

    java_class_pointer = fopen("../Main.class", "rb");
    fseek(java_class_pointer, 0, SEEK_END);
    filelen = ftell(java_class_pointer);
    rewind(java_class_pointer);

    ClassFileBuffer *fileBuffer = malloc(sizeof(ClassFileBuffer));
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
