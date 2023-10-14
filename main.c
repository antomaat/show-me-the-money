#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    FILE *java_class_pointer;
    char *buffer;
    long filelen;

    java_class_pointer = fopen("Main.class", "rb");
    fseek(java_class_pointer, 0, SEEK_END);
    filelen = ftell(java_class_pointer);
    rewind(java_class_pointer);

    buffer = (char *)malloc(filelen * sizeof(char));
    fread(buffer, filelen, 1, java_class_pointer);
    fclose(java_class_pointer);

    printf("file length: %ld \n", filelen);
    int i = 0;
    for (i = 0; i < 10; i++) {
        printf("%x \n", buffer[i]);
    }

    puts("Hello world");
    return 0;
}
