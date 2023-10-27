#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    classFileBuffer->moving_buffer += length;
    return message;
}

struct ConstantPoolInfo {
    int tag;
    int class_index;
    int name_index;
    int descriptor_index;
    int name_and_type_index;
    int string_index;
    int utf8_length;
    char *bytes;
};

struct ClassFile {
    int magic;
    int version_minor;
    int version_major;
    int constant_pool_count;
    struct ConstantPoolInfo *constant_pool;
    int access_flags;
    int this_class;
    int super_class;
    int interfaces_count;
    int interfaces;
    int methods_count;
    struct MethodInfo *methods;
    int attributes_count;
};


struct AttributeInfo {
    int attribute_name_index;
    int attribute_length;
    unsigned char *info;
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
    struct AttributeInfo *attribute_info;
    struct CodeAttribute *code_attributes;
};


void parse_meta_fields(struct ClassFileBuffer *fileBuffer, struct ClassFile *classFile) {
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

void parse_constant_pool(struct ClassFileBuffer *fileBuffer, struct ClassFile *classFile) {
    classFile->constant_pool = malloc(sizeof(struct ConstantPoolInfo) * classFile->constant_pool_count);
    for (size_t index = 0; index < classFile->constant_pool_count - 1; index++) {
        struct ConstantPoolInfo *cp_info = malloc(sizeof(struct ConstantPoolInfo));
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

void parseCodeAttributes(struct MethodInfo *methodInfo, struct ClassFileBuffer *fileBuffer) {
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
    
            methodInfo->code_attributes[attribute_i] = *attribute;
        }
}

char *get_string(struct ClassFile *classFile, int index) {
    return classFile->constant_pool[index-1].bytes;
}

char *get_class_name(struct ClassFile *classFile, int index) {
    return get_string(classFile, classFile->constant_pool[index-1].name_index);
}

char *get_name_and_type_name(struct ClassFile *classFile, int index) {
    int method_ref_name_and_type_i = classFile->constant_pool[index-1].name_index;
    return classFile->constant_pool[method_ref_name_and_type_i-1].bytes;
}

char *get_name_and_type_type(struct ClassFile *classFile, int index) {
    int method_ref_descriptor_i = classFile->constant_pool[index-1].descriptor_index;
    return classFile->constant_pool[method_ref_descriptor_i-1].bytes;
}


void print_cp_index(struct ClassFile *classFile, int index) {
    struct ConstantPoolInfo cp_info = classFile->constant_pool[index];
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

void print_all_cp_tags(struct ClassFile *classFile) {
    for (size_t i = 0; i < classFile->constant_pool_count; i++) {
        print_cp_index(classFile, i);
    }
}

void print_cp_with_tag(struct ClassFile *classFile, int tag) {
    for (size_t i = 0; i < classFile->constant_pool_count; i++) {
        if (classFile->constant_pool[i].tag == tag) {
            print_cp_index(classFile, i);
        }
    }
}

void print_class(struct ClassFile *classFile) {
    printf("-----------------------------\n");
    printf("magic number: %x \n", classFile->magic);
    printf("version: %d.%d \n", classFile->version_major, classFile->version_minor);
    printf("constant_pool_count: %d\n", classFile->constant_pool_count);
    printf("access_flags %d \n", classFile->access_flags);
    printf("this_class %s \n",  get_class_name(classFile, classFile->this_class));
    printf("super_class %s \n", get_class_name(classFile, classFile->super_class));

    print_cp_with_tag(classFile, CONSTANT_CLASS);
    //print_all_cp_tags(classFile);
    //print_all_cp_tags(classFile, 0);
    //for (size_t i = 0; i < classFile->constant_pool_count; i++) {
    //    print_cp_index(classFile, i);
    //}
//    printf("interface count %d \n", classFile->interfaces_count);
//    printf("interfaces %d \n", classFile->interfaces);
//    printf("method count %d \n", classFile->methods_count);
//    printf("attributes count %d \n", classFile->attributes_count);
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
    classFile->methods = malloc(sizeof(struct MethodInfo) * classFile->methods_count);

    for (size_t method_index = 0; method_index < classFile->methods_count; method_index++) {
        struct MethodInfo *methodInfo = malloc(sizeof(struct MethodInfo));
        methodInfo->access_flags = parse_16(fileBuffer);
        methodInfo->name_index = parse_16(fileBuffer);
        methodInfo->descriptor_index = parse_16(fileBuffer);
        methodInfo->attributes_count = parse_16(fileBuffer);
        methodInfo->attribute_info = malloc(sizeof(struct AttributeInfo *) * methodInfo->attributes_count);
        //methodInfo->code_attributes = malloc(sizeof(struct CodeAttribute) * methodInfo->attributes_count);

        printf("method: access_flags %d; name_index %d; descriptor_index %d; attributes_count %d \n", methodInfo->access_flags, methodInfo->name_index, methodInfo->descriptor_index, methodInfo->attributes_count);
        
        for(size_t attr_i = 0; attr_i < methodInfo->attributes_count; attr_i++) {
            struct AttributeInfo *attribute = malloc(sizeof(struct AttributeInfo));
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
