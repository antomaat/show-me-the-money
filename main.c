#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

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
    printf("msg len: %d, message: %s \n", length, message);
    classFileBuffer->moving_buffer += length;
    return message;
}


struct ClassFile {
    int magic;
    int version_minor;
    int version_major;
    int constant_pool_count;
    int access_flags;
    int this_class;
    int super_class;
    int interfaces_count;
    int interfaces;
    int methods_count;
    int methods;
    int attributes_count;
};

struct ConstantPoolInfo {
    int tag;
    int class_index;
    int name_and_type_index;
    int string_index;
};

struct AttributeInfo {
    int attribute_name_index;
    int attribute_length;
    char info[];
};

struct CodeAttribute {
    int attribute_name_index;
    int attribute_length;
    int max_stack;
    int max_locals;
    int code_length;
    int exception_table_length;
    int exception_table;
    int attributes_count;
    struct AttributeInfo attributes;
};

struct MethodInfo {
    int access_flags;
    int name_index;
    int descriptor_index;
    int attributes_count;
    struct CodeAttribute *code_attributes;
};


void parse_meta_fields(struct ClassFileBuffer *fileBuffer, struct ClassFile *classFile) {
    classFile->magic = parse_32(fileBuffer);
    classFile->version_minor = parse_16(fileBuffer);
    classFile->version_major = parse_16(fileBuffer);
}

void parse_constant_pool(struct ClassFileBuffer *fileBuffer, struct ClassFile *classFile) {
    for (size_t index = 0; index < classFile->constant_pool_count - 1; index++) {
        int tag = parse_8(fileBuffer);
        printf("---------------------\n");
        printf("index %ld \n", index + 1);
        switch (tag) {
            case 0:
                printf("tag 0 is wrong \n");
                break;
            case 1:
                printf("tag is utf8\n");
                int utf8_length = parse_16(fileBuffer);
                char *message = convert_to_string(fileBuffer, utf8_length, 0);
                printf("name is: %s \n", message);
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                printf("tag %d is not implemented yet", tag);
                break;
            case 7:
                printf("tag is class\n");
                int name_index = parse_16(fileBuffer);
                printf("ref tag: %d, name_index: %d\n", 7, name_index);
                break;
            case 8:
                printf("tag is string info\n");
                struct ConstantPoolInfo *stringInfo = malloc(sizeof(struct ConstantPoolInfo));
                stringInfo->tag = 8;
                stringInfo->string_index = parse_16(fileBuffer);

                printf("ref tag: %d, string_index: %d\n", stringInfo->tag, stringInfo->string_index);
                free(stringInfo);
                break;
            case 9:
                printf("tag is fieldref\n");
                struct ConstantPoolInfo *fieldRef = malloc(sizeof(struct ConstantPoolInfo));
                fieldRef->tag = 9;
                fieldRef->class_index = parse_16(fileBuffer);
                fieldRef->name_and_type_index = parse_16(fileBuffer);

                printf("ref tag: %d, class_index: %d, type_index: %d \n", fieldRef->tag, fieldRef->class_index, fieldRef->name_and_type_index);

                free(fieldRef);
                break;
            case 10:
                printf("tag is methodref\n");
                struct ConstantPoolInfo *methodRef = malloc(sizeof(struct ConstantPoolInfo));
                methodRef->tag = 10;
                methodRef->class_index = parse_16(fileBuffer);
                methodRef->name_and_type_index = parse_16(fileBuffer);

                printf("ref tag: %d, class_index: %d, type_index: %d \n", methodRef->tag, methodRef->class_index, methodRef->name_and_type_index);

                free(methodRef);

                break;
            case 11:
                printf("tag is interface methodref\n");
                struct ConstantPoolInfo *interfaceRef = malloc(sizeof(struct ConstantPoolInfo));
                interfaceRef->tag = 11;
                interfaceRef->class_index = parse_16(fileBuffer);
                interfaceRef->name_and_type_index = parse_16(fileBuffer);

                printf("ref tag: %d, class_index: %d, type_index: %d \n", interfaceRef->tag, interfaceRef->class_index, interfaceRef->name_and_type_index);
                free(interfaceRef);

                break;
            case 12:
                printf("tag is NameAndType");
                int name_and_tag_name_index = parse_16(fileBuffer);
                int name_and_tag_descriptor_index = parse_16(fileBuffer);
                printf("ref tag: %d, name index: %d, descriptor index: %d \n", 12, name_and_tag_name_index, name_and_tag_descriptor_index);
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
    
    struct ClassFile *classFile = malloc(sizeof(struct ClassFile));

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

    for (size_t method_index = 0; method_index < classFile->methods_count; method_index++) {
        struct MethodInfo *methodInfo = malloc(sizeof(struct MethodInfo));
        methodInfo->access_flags = parse_16(fileBuffer);
        methodInfo->name_index = parse_16(fileBuffer);
        methodInfo->descriptor_index = parse_16(fileBuffer);
        methodInfo->attributes_count = parse_16(fileBuffer);
        methodInfo->code_attributes = malloc(sizeof(struct CodeAttribute) * methodInfo->attributes_count);

        printf("method: access_flags %d; name_index %d; descriptor_index %d; attributes_count %d \n", methodInfo->access_flags, methodInfo->name_index, methodInfo->descriptor_index, methodInfo->attributes_count);

        //attributes
        for (size_t attribute_i = 0; attribute_i < methodInfo->attributes_count; attribute_i++) {
            struct CodeAttribute *attribute = malloc(sizeof(struct CodeAttribute));
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
    
            printf("attribute name i: %d; len: %d; \n", attribute->attribute_name_index, attribute->code_length);
            methodInfo->code_attributes[attribute_i] = *attribute;

        }

    }

    classFile->attributes_count = parse_16(fileBuffer);

    printf("-----------------------------\n");
    printf("file length: %ld \n", filelen);

    printf("magic number: %x \n", classFile->magic);
    printf("version: %d.%d \n", classFile->version_major, classFile->version_minor);
    printf("constant_pool_count: %d\n", classFile->constant_pool_count);
    printf("access_flags %d \n", classFile->access_flags);
    printf("this_class %d \n", classFile->this_class);
    printf("super_class %d \n", classFile->super_class);
    printf("interface count %d \n", classFile->interfaces_count);
    printf("interfaces %d \n", classFile->interfaces);
    printf("method count %d \n", classFile->methods_count);
    printf("attributes count %d \n", classFile->attributes_count);

    return 0;
}
