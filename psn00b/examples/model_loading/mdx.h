#ifndef _MDX_H_
#define _MDX_H_

#include "display.h"
#include <psxgpu.h>
#include <psxgte.h>
#include <stdint.h>

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
    uint16_t framesize;    // size in bytes of a frame
    uint16_t num_vertices; // number of vertices per frame

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
}  MDX_Header;

typedef struct _PSX_MD2_POLY
{
    uint16_t vertex[3]; // vertex indices of the poly
    uint16_t uv[3];     // tex coord, vertex colours, clut, tpage
} MDX_POLY;

typedef struct _PSX_MD2_UV
{
    uint16_t u;
    uint16_t v;
} MDX_UV;

typedef struct _PSX_MD2_Vertex
{
    uint8_t v[3];        // position, compressed in a 256x256 grid
    uint8_t normalIndex; // normal vector index
} MDX_Vertex;

// MD2 animation frame
typedef struct _PSX_MD2_Frame
{
    SVECTOR scale;     /* scale factor of each frame */
    SVECTOR translate; /* translation vector */
    MDX_Vertex *verts; /* list of frame's vertices */
} MDX_Frame;

typedef struct _PSX_MD2_SKIN
{
    uint16_t tpage;
    uint16_t clut;
} MDX_Tex;

typedef struct _PSX_MD2_M
{
    MDX_Header head;
    MDX_UV *uvs;
    MDX_POLY *tris;
    MDX_Frame *frames;
} MDX;

typedef struct _PSX_MD2_Animation
{
    uint16_t id;
    uint16_t start_frame;
    uint16_t end_frame;
    uint16_t pad;

} MDX_Animation;

typedef struct _PSX_MD2_Info
{
    uint16_t current_frame;
    uint16_t animation_speed;
    MDX_Animation *animations;
} MDX_Info;

/**
 * @brief Loads an MD2 model from a file
 *
 * @param char* file MD2 model file
 *
 * @return MDX* Pointer to the loaded MDX model, or NULL if loading failed
 */
MDX *loadMDX(const unsigned char *file);

/**
 * @brief Prints MDX info to the console for debugging
 * 
 * @param mdx 
 */
void printMDXInfo(MDX *mdx);

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
void sortMDX(RenderContext *ctx, MDX *mdx, VECTOR pos, SVECTOR rot, uint32_t scale, MDX_Info *info, TIM_IMAGE *tex);

/**
 * @brief Unpacks frame position based on frame translation and scale
 *
 * @param mdx MDX model pointer
 * @param f current frame to unpack
 * @param v current vertex coordinates to unpack
 * @return SVECTOR vector of unpacked positions to be used by the GTE
 */
SVECTOR mdx_unpack_pos(MDX *mdx, uint16_t f, uint16_t v);

#endif // _MDX_H_
