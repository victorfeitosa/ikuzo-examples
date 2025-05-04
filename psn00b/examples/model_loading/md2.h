#ifndef _MD2_H_
#define _MD2_H_

#include "display.h"
#include <psxgpu.h>
#include <psxgte.h>
#include <stdint-gcc.h>

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

typedef struct _PSX_MD2_Header
{
    uint32_t framesize;
    uint32_t num_vertices;
    uint32_t num_st;
    uint32_t num_tris;
    uint32_t num_frames;

    uint32_t offset_st;
    uint32_t offset_tris;
    uint32_t offset_frames;
    uint32_t offset_end;
} MDX_Header;

typedef float MD2_Vec3[3];
typedef int MD2_Vec3i[3];
typedef char MD2_SkinName[64];

typedef struct _MD2_Skin
{
    char name[64];
} MD2_Skin;

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

typedef struct _MD2_PSX_Frame
{
    SVECTOR scale;     /* scale factor of each frame */
    SVECTOR translate; /* translation vector */
    char name[16];      /* frame name */
    MD2_Vertex *verts;  /* list of frame's vertices */
} MDX_Frame;

typedef struct _MD2_FrameI
{
    MD2_Vec3i scale;
    MD2_Vec3i translate;
    char name[16];
    MD2_Vertex *verts;
} MD2_FrameI;

typedef struct _MD2_M
{
    MD2_Header *head;
    MD2_TexCoord *texcoords;
    MD2_Tri *tris;
    MDX_Frame *frames;
    uint16_t current_frame;
} MD2_M;

typedef struct _MD2_PSX_M
{
    MDX_Header *head;
    MD2_TexCoord *texcoords;
    MD2_Tri *tris;
    MD2_FrameI *frames;
    uint16_t current_frame;
    uint16_t animation_speed;
} MDX_M;

/**
 * @brief Loads an MD2 model from a file
 *
 * @param MD2* md2Ptr Pointer that will reference the loaded model
 * @param char* md2File MD2 model file
 *
 * @return size_t Size in bytes of the allocated memory for the model
 */
size_t LoadMD2(MD2_M *md2, const unsigned char *file);

/**
 * @brief Loads an MD2 model from stack memory
 *
 * @param MD2* md2Ptr Pointer that will reference the loaded model
 * @param char* md2File MD2 model data
 *
 * @return void
 */
void LoadMD2FromMem(MD2_M *md2, const unsigned char *data);

/**
 * @brief Sorts a loaded MD2 model
 *
 */
void SortMD2(RenderContext *ctx, MD2_M *md2Ptr, TIM_IMAGE *skin, VECTOR pos, SVECTOR rot, uint32_t scale);

/**
 * @brief Unpacks frame position based on frame translation and scale
 *
 * @param md2 MD2 model pointer
 * @param frame current frame coordinates to unpack
 * @return SVECTOR vector of unpacked positions to be used by the GTE
 */
SVECTOR md2_unpack_pos(MD2_M *md2, uint16_t frame);

#endif // _MD2_H_
