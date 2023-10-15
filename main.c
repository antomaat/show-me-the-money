#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

u_int32_t convert_byte_to_int(unsigned char *bytes) {
    return (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
}

u_int16_t convert_16bit_to_int(unsigned char *bytes) {
    return (bytes[0] << 8) + bytes[1];
}

struct ClassFile {
    int magic;
    int version_minor;
    int version_major;
    int constant_pool_count;
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

    printf("file length: %ld \n", filelen);

    printf("magic number: %x \n", classFile->magic);
    printf("version: %d.%d \n", classFile->version_major, classFile->version_minor);
    printf("constant_pool_count: %d\n", classFile->constant_pool_count);

    return 0;
}
