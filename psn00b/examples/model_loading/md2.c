#include "md2.h"

size_t LoadMD2Mem(PSX_MD2 *md2Ptr, char **data)
{
    MD2_Header *head = &data[0];
    // Allocates enough enough to store all frames and all texture coordinates
    size_t memSize = (head->framesize * head->num_frames * head->num_vertices) + (head->num_st * sizeof(MD2_TexCoord));
    uint8_t md2[memSize];

    // TODO: load model to memory
    PSX_MD2_Header *md2Head = md2[0];
    md2Head->skinwidth = head->skinwidth;
    md2Head->skinheight = head->skinheight;
    md2Head->framesize = head->framesize;
    md2Head->num_vertices = head->num_vertices;
    md2Head->num_st = head->num_st;
    md2Head->num_tris = head->num_tris;
    md2Head->num_frames = head->num_frames;
    md2Head->offset_skins = head->offset_skins;
    md2Head->offset_st = head->offset_st;
    md2Head->offset_skins = head->offset_skins;
    md2Head->offset_frames = head->offset_frames;
    md2Head->eof = head->offset_end;

    // Loads data into pointer and returns the size in memory
    md2Ptr = &md2;
    return memSize;
}