#include <sys/types.h>
#include "class_structs.h"

typedef struct {
    unsigned char *buffer;
    unsigned char *moving_buffer;
    int index;
} ClassFileBuffer;

u_int32_t parse_32(ClassFileBuffer *classFileBuffer);
u_int16_t parse_16(ClassFileBuffer *classFileBuffer);
u_int8_t parse_8(ClassFileBuffer *classFileBuffer);

char *convert_to_string(ClassFileBuffer *classFileBuffer, int length, int is_print);
void parse_meta_fields(ClassFileBuffer *fileBuffer, ClassFile *classFile);
void parse_constant_pool(ClassFileBuffer *fileBuffer, ClassFile *classFile);
void parseCodeAttributes(MethodInfo *methodInfo, ClassFileBuffer *fileBuffer, ClassFile *classFile);

