#include "md2.h"

size_t LoadMD2Mem(PSX_MD2 **md2Ptr, const unsigned char data[])
{
    MD2_Header *head = (MD2_Header *)data; // Points header to first byte of MD2 data
    // Allocates enough enough to store all frames and all texture coordinates
    size_t memSize = (sizeof(PSX_MD2_Header) +
                      (head->num_st * sizeof(PSX_MD2_TexCoord)) +
                      (head->num_vertices * sizeof(PSX_MD2_Vertex)) +
                      (head->num_tris * sizeof(PSX_MD2_Tri)) +
                      (head->num_frames * sizeof(PSX_MD2_Frame)));
    uint8_t md2_data[memSize];

    // TODO: load model to memory
    PSX_MD2_Header *md2_head = (PSX_MD2_Header *)&md2_data[0];
    md2_head->skinwidth = (uint16_t)head->skinwidth;
    md2_head->skinheight = (uint16_t)head->skinheight;
    md2_head->framesize = (uint16_t)head->framesize;
    md2_head->num_vertices = (uint16_t)head->num_vertices;
    md2_head->num_st = (uint32_t)head->num_st;
    md2_head->num_tris = (uint32_t)head->num_tris;
    md2_head->num_frames = (uint16_t)head->num_frames;
    md2_head->offset_skins = head->offset_skins;
    md2_head->offset_st = head->offset_st;
    md2_head->offset_skins = head->offset_skins;
    md2_head->offset_frames = head->offset_frames;
    md2_head->eof = head->offset_end;

    *md2Ptr = (PSX_MD2 *)&md2_data;

    // Loads data into pointer and returns the size in memory
    return memSize;
}