#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

u_int32_t convert_byte_to_int(unsigned char *bytes) {
    return (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
}

u_int16_t convert_16bit_to_int(unsigned char *bytes) {
    return (bytes[0] << 8) + bytes[1];
}

char *convert_to_string(unsigned char *bytes, int length) {
    char *message = malloc(length + 1);
    int i = 0;
    for (i = 0; i < length; i++) {
        message[i] = bytes[i];
    }
    message[length] = '\0';
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
};

struct ConstantPoolInfo {
    int tag;
    int class_index;
    int name_and_type_index;
    int string_index;
};


int main(int argc, char *argv[]) {
    FILE *java_class_pointer;
    unsigned char *buffer;
    unsigned char *moving_buffer;
    long filelen;

    java_class_pointer = fopen("Main.class", "rb");
    fseek(java_class_pointer, 0, SEEK_END);
    filelen = ftell(java_class_pointer);
    rewind(java_class_pointer);

    buffer = (char *)malloc(filelen * sizeof(unsigned char));
    fread(buffer, filelen, 1, java_class_pointer);
    fclose(java_class_pointer);

    
    moving_buffer = &buffer[0];
    struct ClassFile *classFile = malloc(sizeof(struct ClassFile));

    classFile->magic = convert_byte_to_int(moving_buffer);
    moving_buffer += 4;
    classFile->version_minor = convert_16bit_to_int(moving_buffer);
    moving_buffer += 2;
    classFile->version_major = convert_16bit_to_int(moving_buffer);
    moving_buffer += 2;
    classFile->constant_pool_count = convert_16bit_to_int(moving_buffer);
    moving_buffer += 2;

    int index = 0;
    int has_errors = 0;
    //for (index = 0; index > classFile->constant_pool_count; index++) {
    for (index = 0; index < classFile->constant_pool_count - 1; index++) {
        if (has_errors == 1) {
            return 0;
        }
        int tag = moving_buffer[0];
        moving_buffer += 1;
        printf("---------------------\n");
        printf("index %d \n", index + 1);
        switch (tag) {
            case 0:
                printf("tag 0 is wrong \n");
                has_errors = 1;
                break;
            case 1:
                printf("tag is utf8\n");
                int utf8_length = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;
                char *message = convert_to_string(moving_buffer, utf8_length);
                moving_buffer += utf8_length;
                printf("name is: %s \n", message);
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                printf("tag %d is not implemented yet", tag);
                has_errors = 1;
                break;
            case 7:
                printf("tag is class\n");
                int name_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;
                printf("ref tag: %d, name_index: %d\n", 7, name_index);
                break;
            case 8:
                printf("tag is string info\n");
                struct ConstantPoolInfo *stringInfo = malloc(sizeof(struct ConstantPoolInfo));
                stringInfo->tag = 8;
                stringInfo->string_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;

                printf("ref tag: %d, string_index: %d\n", stringInfo->tag, stringInfo->string_index);
                free(stringInfo);
                break;
            case 9:
                printf("tag is fieldref\n");
                struct ConstantPoolInfo *fieldRef = malloc(sizeof(struct ConstantPoolInfo));
                fieldRef->tag = 9;
                fieldRef->class_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;
                fieldRef->name_and_type_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;

                printf("ref tag: %d, class_index: %d, type_index: %d \n", fieldRef->tag, fieldRef->class_index, fieldRef->name_and_type_index);

                free(fieldRef);
                break;
            case 10:
                printf("tag is methodref\n");
                struct ConstantPoolInfo *methodRef = malloc(sizeof(struct ConstantPoolInfo));
                methodRef->tag = 10;
                methodRef->class_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;
                methodRef->name_and_type_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;

                printf("ref tag: %d, class_index: %d, type_index: %d \n", methodRef->tag, methodRef->class_index, methodRef->name_and_type_index);

                free(methodRef);

                break;
            case 11:
                printf("tag is interface methodref\n");
                struct ConstantPoolInfo *interfaceRef = malloc(sizeof(struct ConstantPoolInfo));
                interfaceRef->tag = 11;
                interfaceRef->class_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;
                interfaceRef->name_and_type_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;

                printf("ref tag: %d, class_index: %d, type_index: %d \n", interfaceRef->tag, interfaceRef->class_index, interfaceRef->name_and_type_index);
                free(interfaceRef);

                break;
            case 12:
                printf("tag is NameAndType");
                int name_and_tag_name_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;
                int name_and_tag_descriptor_index = convert_16bit_to_int(moving_buffer);
                moving_buffer += 2;
                printf("ref tag: %d, name index: %d, descriptor index: %d \n", 12, name_and_tag_name_index, name_and_tag_descriptor_index);
                break;
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
                printf("tag %d is not implemented yet", tag);
                has_errors = 1;
                break;
            default:
                printf("tag is unknown %d \n", tag);
                break;
        }
    }

    classFile->access_flags = convert_16bit_to_int(moving_buffer);
    moving_buffer += 2;
    
    classFile->this_class = convert_16bit_to_int(moving_buffer);
    moving_buffer += 2;

    classFile->super_class = convert_16bit_to_int(moving_buffer);
    moving_buffer += 2;

    classFile->interfaces_count = convert_16bit_to_int(moving_buffer);
    moving_buffer += 2;

    printf("-----------------------------\n");
    printf("file length: %ld \n", filelen);

    printf("magic number: %x \n", classFile->magic);
    printf("version: %d.%d \n", classFile->version_major, classFile->version_minor);
    printf("constant_pool_count: %d\n", classFile->constant_pool_count);
    printf("access_flags %d \n", classFile->access_flags);
    printf("this_class %d \n", classFile->this_class);
    printf("super_class %d \n", classFile->super_class);
    printf("interface count %d \n", classFile->interfaces_count);

    return 0;
}
