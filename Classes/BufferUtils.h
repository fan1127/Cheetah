#pragma once

//
// macro for creating char buffer with fixed size
// it generates constant integer with buffer size which can be used with snprintf and/or other standard functions
//
//
// Example:
// CREATE_CHAR_BUFFER(buffer, 128);
// snprintf(buffer, buffer_size, "format your string here: %s");
//
// variables "buffer" and "buffer_size" are generated automatically
//
#define CREATE_CHAR_BUFFER(name, size) const size_t name##_size = size; char name[name##_size] = {0};
