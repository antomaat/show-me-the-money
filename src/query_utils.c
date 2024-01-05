#include "class_structs.h"
#include <string.h>

char *get_string(ClassFile *classFile, int index) {
    return classFile->constant_pool[index-1].bytes;
}

char *get_class_name(ClassFile *classFile, int index) {
    return get_string(classFile, classFile->constant_pool[index-1].name_index);
}

char *get_name_and_type_name(ClassFile *classFile, int index) {
    int method_ref_name_and_type_i = classFile->constant_pool[index-1].name_index;
    return classFile->constant_pool[method_ref_name_and_type_i-1].bytes;
}

char *get_name_and_type_type(ClassFile *classFile, int index) {
    int method_ref_descriptor_i = classFile->constant_pool[index-1].descriptor_index;
    return classFile->constant_pool[method_ref_descriptor_i-1].bytes;
}

int is_code_attribute(ClassFile *classFile, AttributeInfo attributeInfo) {
    if (strstr(get_string(classFile, attributeInfo.attribute_name_index), "Code") != NULL) {
        return 1;
    }
    return 0;
}
