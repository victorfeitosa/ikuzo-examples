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
    MD2_Frame *frames;
    uint16_t current_frame;
} MD2_M;

/**
 * @brief PSX Model format
 *
 */

typedef enum _PSX_MDX_Material
{
    MDX_MAT_FLAT = 0,
    MDX_MAT_GORAUND,
    MDX_MAT_FLAT_TEXTURED,
    MDX_MAT_GORAUND_TEXTURED,
    MDX_MAT_WIREFRAME,
} MDX_Material;

typedef struct _PSX_MD2_Header
{
    uint16_t framesize;     // size in bytes of a frame 
    uint16_t num_vertices;  // number of vertices per frame 

    uint16_t num_uv;   // number of texture coordinates
    uint16_t num_tris; // number of triangles

    uint16_t num_frames; // number of frames
    uint16_t mat;        // type of rendering (F3, F3, FT3, GT3, LINE)
    uint16_t clut;       // texture CLUT
    uint16_t tpage;      // texture page

    uint32_t offset_uv;     // offset texture coordinate data
    uint32_t offset_tris;   // offset triangle data
    uint32_t offset_frames; // offset frame data
    uint32_t offset_end;    // offset end of file
} MDX_Header;

typedef struct _PSX_MD2_POLY
{
    uint16_t vertex[3]; // vertex indices of the poly
    uint16_t uv[3];     // tex coord, vertex colours, clut, tpage
} MDX_POLY;

typedef struct _PSX_MD2_UV
{
    uint8_t u;
    uint8_t v;
} MDX_UV;

typedef struct _PSX_MD2_Vertex
{
    uint8_t v[3];           // position, compressed in a 256x256 grid 
    uint8_t normalIndex;    // normal vector index 
} MDX_Vertex;

// MD2 animation frame
typedef struct _PSX_MD2_Frame
{
    SVECTOR scale;      /* scale factor of each frame */
    SVECTOR translate;  /* translation vector */
    MDX_Vertex *verts;  /* list of frame's vertices */
} MDX_Frame;

typedef struct _PSX_MD2_SKIN
{
    uint16_t tpage;
    uint16_t clut;
} MDX_Tex;

typedef struct _PSX_MD2_M
{
    MDX_Header head;
    uint16_t current_frame;
    uint16_t animation_speed;
    MDX_Vertex *vertices;
    MDX_POLY *polys;
    MDX_Frame *frames;
} MDX;

/**
 * @brief Loads an MD2 model from a file
 *
 * @param char* file MD2 model file
 * @param MDX* mdx Dest pointer that will reference the loaded model
 *
 * @return size_t Size in bytes of the allocated memory for the model
 */
size_t LoadMDX(const unsigned char *file, MDX *mdx);

/**
 * @brief Sorts a loaded MD2 model into the OT
 *
 * @param RenderContext* ctx Render context to sort the model into
 * @param MDX* mdx Pointer to the MDX in memory
 * @param VECTOR pos Position
 * @param SVECTOR rot Rotation
 * @param uint32_t scale Scale
 * @param MDX_Tex* tex Model texture, can be NULL
 */
void SortMDX(RenderContext *ctx, MDX *mdx, VECTOR pos, SVECTOR rot, uint32_t scale, MDX_Tex *tex);

/**
 * @brief Unpacks frame position based on frame translation and scale
 *
 * @param md2 MD2 model pointer
 * @param frame_number current frame coordinates to unpack
 * @param vertex_index vertex index to unpack
 * @return SVECTOR vector of unpacked positions to be used by the GTE
 */
SVECTOR mdx_unpack_pos(MDX *mdx, uint16_t frame_number, uint32_t vertex_index);

#endif // _MD2_H_
