#include "md2.h"

size_t LoadMD2Mem(MD2 **md2Ptr, const unsigned char data[])
{
    MD2_Header *head = (MD2_Header *)data; // Points header to first byte of MD2 data
    // Allocates enough enough to store all frames and all texture coordinates
    size_t memSize = (sizeof(MD2_Header) +                        // Header memory
                      (head->num_st * sizeof(MD2_TexCoord)) +     // Texture coordinates memory
                      (head->num_vertices * sizeof(MD2_Vertex)) + // Vertices memory
                      (head->num_tris * sizeof(MD2_Tri)) +        // Triangles memory
                      (head->num_frames * sizeof(MD2_FrameI)));   // Frames memory
    uint8_t md2[memSize];                                         // MD2 model data

    // TODO: load model to memory
    *md2Ptr = (MD2 *)&md2; // Points to the allocated data

    (*md2Ptr)->head = *head;

    // Loads data into pointer and returns the size in memory
    return memSize;
}