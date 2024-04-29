#ifndef _MD2_H_
#define _MD2_H_

#include <psxgpu.h>
#include <psxgte.h>
#include <stdint.h>

// Original Quake II MD2 structure
/**
 * @brief MD2 Header structure
 * Defines all base properties on a MD2 file
 */
typedef struct _MD2_Header
{
    int ident;   /* magic number: "IDP2" */
    int version; /* version: must be 8 */

    int skinwidth;  /* texture width */
    int skinheight; /* texture height */

    int framesize; /* size in bytes of a frame */

    int num_skins;    /* number of skins */
    int num_vertices; /* number of vertices per frame */
    int num_st;       /* number of texture coordinates */
    int num_tris;     /* number of triangles */
    int num_glcmds;   /* number of opengl commands */
    int num_frames;   /* number of frames */

    int offset_skins;  /* offset skin data */
    int offset_st;     /* offset texture coordinate data */
    int offset_tris;   /* offset triangle data */
    int offset_frames; /* offset frame data */
    int offset_glcmds; /* offset OpenGL command data */
    int offset_end;    /* offset end of file */
} MD2_Header;

typedef float MD2_Vec3[3];
typedef char MD2_SkinName[64];

typedef struct _MD2_Tri
{
    unsigned short vertex[3]; /* vertex indices of the triangle */
    unsigned short st[3];     /* tex. coord. indices */
} MD2_Tri;

typedef struct _MD2_TexCoord
{
    short s; // U coord
    short t; // V coord
} MD2_TexCoord;

typedef struct _MD2_Vertex
{
    unsigned char v[3];        /* position, compressed in a 256x256 grid */
    unsigned char normalIndex; /* normal vector index */
} MD2_Vertex;

// MD2 animation frame
typedef struct _MD2_Frame
{
    MD2_Vec3 scale;     /* scale factor of each frame */
    MD2_Vec3 translate; /* translation vector */
    char name[16];      /* frame name */
    MD2_Vertex *verts;  /* list of frame's vertices */
} MD2_Frame;

//----------------------------
// Memory Optimized MD2 defs
//----------------------------
typedef struct _PSX_MD2_Header
{
    uint8_t skinwidth; // Textures can only be a max of 256x256 pixels
    uint8_t skinheight;
    uint16_t framesize;
    uint16_t num_vertices;
    uint32_t num_st;
    uint32_t num_tris;
    uint16_t num_frames;
    size_t offset_skins;
    size_t offset_st;
    size_t offset_tris;
    size_t offset_frames;
    size_t eof;
} PSX_MD2_Header;

typedef VECTOR PSX_MD2_Vec3;

typedef struct _PSX_MD2_TexCoord
{
    uint8_t s; // PSX Texture coords can algo only go up to 256
    uint8_t t;
} PSX_MD2_TexCoord;

typedef struct _PSX_MD2_Tri
{
    uint16_t vertex[3]; // Indexes of a face's vertices
    uint16_t st[3];     // Indexes of a face vertices' texture coords
} PSX_MD2_Tri;

typedef struct _PSX_MD2_Vertex
{
    uint8_t v[3];           // Compressed vertex position [0-255, 0-255, 0-255]
    uint8_t normalIndex;    // Vertex normal index (0-255)
} PSX_MD2_Vertex;

typedef struct _PSX_MD2_Frame
{
    PSX_MD2_Vec3 scale;     // How much to scale a frame by (to decompress the vertex coordinates)
    PSX_MD2_Vec3 translate; // How much to translate the frame by
    char name[16];          // Frame name
    // PS: all frames must contain all vertices
} PSX_MD2_Frame;

typedef struct _PSX_MD2
{
    PSX_MD2_Header head;
    PSX_MD2_TexCoord *texcoords;
    PSX_MD2_Vertex *verts;
    PSX_MD2_Tri *tris;
    PSX_MD2_Frame *frames;
} PSX_MD2;

/**
 * @brief Loads an MD2 model from a space in memory
 *
 * @param MD2* md2Ptr Pointer that will reference the loaded model
 * @param char* md2File MD2 model file
 *
 * @return size_t Size in bytes of the allocated chunk for the model
 */
size_t LoadMD2Mem(PSX_MD2 *md2Ptr, char **data);

/**
 * @brief Sorts a loaded MD2 model
 *
 */
void SortMD2(PSX_MD2 *md2Ptr, TIM_IMAGE *skin, VECTOR pos, SVECTOR rot, VECTOR scale);

#endif // _MD2_H_
