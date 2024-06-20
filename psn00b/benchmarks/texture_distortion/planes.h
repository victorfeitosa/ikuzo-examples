#ifndef GEOM_H
#define GEOM_H

#include "display.h"
#include <stdio.h>
#include <stdlib.h>

// Cube defaults
// Vertices struct
typedef struct
{
    short v0, v1, v2, v3;
} INDEX;

// Simple plane mesh
SVECTOR simple_plane_verts[] = {
    {-100, -100, 0},
    {100,  -100, 0},
    {-100,  100, 0},
    {100,   100, 0}
};
// Vertically subdivided plane mesh
SVECTOR vertical_plane_verts[] = {
    {-100, -100, 0}, {-50, -100, 0}, {0, -100, 0}, {50, -100, 0}, {100, -100, 0},
    {-100, 100, 0},  {-50, 100, 0},  {0, 100, 0},  {50, 100, 0},  {100, 100, 0}    
};
// Subdivided plane mesh
SVECTOR sub_plane_verts[] = {
    {-100, -100, 0}, {-50, -100, 0}, {0, -100, 0}, {50, -100, 0}, {100, -100, 0},
    {-100, -50, 0},  {-50, -50, 0},  {0, -50, 0},  {50, -50, 0},  {100, -50, 0},
    {-100, 0, 0},    {-50, 0, 0},    {0, 0, 0},    {50, 0, 0},    {100, 0, 0},
    {-100, 50, 0},   {-50, 50, 0},   {0, 50, 0},   {50, 50, 0},   {100, 50, 0},
    {-100, 100, 0},  {-50, 100, 0},  {0, 100, 0},  {50, 100, 0},  {100, 100, 0}
};

SVECTOR *scratch_verts = (SVECTOR*)0x1F800000;

// Plane vertex indexes
INDEX simple_plane_indices = {0, 1, 2, 3};
INDEX vertical_plane_indices[] = {{0, 1, 5, 6}, {1, 2, 6, 7}, {2, 3, 7, 8}, {3, 4, 8, 9}};
INDEX subdivided_plane_indices[] = {
    {0, 1, 5, 6},     {1, 2, 6, 7},     {2, 3, 7, 8},     {3, 4, 8, 9},
    {5, 6, 10, 11},   {6, 7, 11, 12},   {7, 8, 12, 13},   {8, 9, 13, 14},
    {10, 11, 15, 16}, {11, 12, 16, 17}, {12, 13, 17, 18}, {13, 14, 18, 19},
    {15, 16, 20, 21}, {16, 17, 21, 22}, {17, 18, 22, 23}, {18, 19, 23, 24}
};

SVECTOR rot = {0, 0};
VECTOR pos = {0, 0, 400};

MATRIX *WORLD_SPACE;

void SortSimplePlane(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    int i, p;

    POLY_FT4 *pol4 = (POLY_FT4 *)ctx->nextpri;
    DrawEnv *active_buff = active_buffer(ctx);

    uint16_t texture_tpage = getTPage(texture->mode, 1, texture->prect->x, texture->prect->y);
    uint16_t texture_clut = getClut(texture->crect->x, texture->crect->y);

    // Set rotation and translation to the matrix
    RotMatrix(&rot, WORLD_SPACE);
    TransMatrix(WORLD_SPACE, &pos);

    // Set rotation and translation matrix
    gte_SetRotMatrix(WORLD_SPACE);
    gte_SetTransMatrix(WORLD_SPACE);

    // Load the first 3 vertices of a quad to the GTE
    gte_ldv3(
        &simple_plane_verts[simple_plane_indices.v0],
        &simple_plane_verts[simple_plane_indices.v1],
        &simple_plane_verts[simple_plane_indices.v2]
    );

    // Rotation, Translation and Perspective Triple
    gte_rtpt();

    // Compute normal direction for backface culling
    gte_nclip();

    // Store result of cross product (nclip) in p
    gte_stopz(&p);

    // Initialize a quad primitive
    setPolyFT4(pol4);

    // Set the projected vertices to the primitive
    gte_stsxy0(&pol4->x0);
    gte_stsxy1(&pol4->x1);
    gte_stsxy2(&pol4->x2);

    // Compute the last vertex and set the result
    gte_ldv0(&simple_plane_verts[simple_plane_indices.v3]);
    gte_rtps();
    gte_stsxy(&pol4->x3);

    // Calculate average Z for depth sorting and store result in p
    gte_avsz4();
    gte_stotz(&p);

    // Skip if clipping off
    // (the shift right operator is to scale the depth precision)
    if (p > OT_LEN || p < 25)
        return;

    // Load primitive color even though gte_ncs() doesn't use it.
    // This is so the GTE will output a color result with the
    // correct primitive code.
    gte_ldrgb(&pol4->r0);

    setRGB0(pol4, 127, 127, 127);

    // Set the UVs
    setUV4(pol4,
        0, 0,
        255, 0,
        0, 255,
        255, 255);

    pol4->tpage = texture_tpage;
    pol4->clut = texture_clut;

    // Sort primitive to the ordering table
    addPrim(active_buff->ot + p, pol4);

    // Advance to make another primitive
    pol4++;

    // Update nextpri variable
    ctx->nextpri = (char *)pol4;
}

void SortVerticalSubPlane(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    int i, p;

    POLY_FT4 *pol4 = (POLY_FT4 *)ctx->nextpri;
    DrawEnv *active_buff = active_buffer(ctx);

    uint16_t texture_tpage = getTPage(texture->mode, 1, texture->prect->x, texture->prect->y);
    uint16_t texture_clut = getClut(texture->crect->x, texture->crect->y);

    // Set rotation and translation to the matrix
    RotMatrix(&rot, WORLD_SPACE);
    TransMatrix(WORLD_SPACE, &pos);

    // Set rotation and translation matrix
    gte_SetRotMatrix(WORLD_SPACE);
    gte_SetTransMatrix(WORLD_SPACE);

    // Draws each row of planes
    for(size_t j=0; j < 5; j++)
    {
        // Load the first 3 vertices of a quad to the GTE
        gte_ldv3(
            &vertical_plane_verts[vertical_plane_indices[j].v0],
            &vertical_plane_verts[vertical_plane_indices[j].v1],
            &vertical_plane_verts[vertical_plane_indices[j].v2]
        );

        // Rotation, Translation and Perspective Triple
        gte_rtpt();

        // Compute normal direction for backface culling
        gte_nclip();

        // Store result of cross product (nclip) in p
        gte_stopz(&p);

        // Initialize a quad primitive
        setPolyFT4(pol4);

        // Set the projected vertices to the primitive
        gte_stsxy0(&pol4->x0);
        gte_stsxy1(&pol4->x1);
        gte_stsxy2(&pol4->x2);

        // Compute the last vertex and set the result
        gte_ldv0(&vertical_plane_verts[vertical_plane_indices[j].v3]);
        gte_rtps();
        gte_stsxy(&pol4->x3);

        // Calculate average Z for depth sorting and store result in p
        gte_avsz4();
        gte_stotz(&p);

        // Skip if clipping off
        // (the shift right operator is to scale the depth precision)
        if (p > OT_LEN || p < 30)
            return;

        // Load primitive color even though gte_ncs() doesn't use it.
        // This is so the GTE will output a color result with the
        // correct primitive code.
        gte_ldrgb(&pol4->r0);

        setRGB0(pol4, 127, 127, 127);

        // Set the UVs
        const uint16_t uv_x = (63 * j);
        setUV4(
            pol4,
            uv_x, 0,
            uv_x + 63, 0,
            uv_x, 255,
            uv_x + 63, 255
        );

        pol4->tpage = texture_tpage;
        pol4->clut = texture_clut;

        // Sort primitive to the ordering table
        addPrim(active_buff->ot + p, pol4);

        // Advance to make another primitive
        pol4++;
    }

    // Update nextpri variable
    ctx->nextpri = (char *)pol4;
}

void SortSubdividedPlane(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    int i, p;

    POLY_FT4 *pol4 = (POLY_FT4 *)ctx->nextpri;
    DrawEnv *active_buff = active_buffer(ctx);

    uint16_t texture_tpage = getTPage(texture->mode, 1, texture->prect->x, texture->prect->y);
    uint16_t texture_clut = getClut(texture->crect->x, texture->crect->y);

    // Set rotation and translation to the matrix
    RotMatrix(&rot, WORLD_SPACE);
    TransMatrix(WORLD_SPACE, &pos);

    // Set rotation and translation matrix
    gte_SetRotMatrix(WORLD_SPACE);
    gte_SetTransMatrix(WORLD_SPACE);

    // Draws each row of planes
    for(size_t j=0; j < 16; j++)
    {
        // Load the first 3 vertices of a quad to the GTE
        gte_ldv3(
            &sub_plane_verts[subdivided_plane_indices[j].v0],
            &sub_plane_verts[subdivided_plane_indices[j].v1],
            &sub_plane_verts[subdivided_plane_indices[j].v2]
        );

        // Rotation, Translation and Perspective Triple
        gte_rtpt();

        // Compute normal direction for backface culling
        gte_nclip();

        // Store result of cross product (nclip) in p
        gte_stopz(&p);

        // Initialize a quad primitive
        setPolyFT4(pol4);

        // Set the projected vertices to the primitive
        gte_stsxy0(&pol4->x0);
        gte_stsxy1(&pol4->x1);
        gte_stsxy2(&pol4->x2);

        // Compute the last vertex and set the result
        gte_ldv0(&sub_plane_verts[subdivided_plane_indices[j].v3]);
        gte_rtps();
        gte_stsxy(&pol4->x3);

        // Calculate average Z for depth sorting and store result in p
        gte_avsz4();
        gte_stotz(&p);

        // Skip if clipping off
        // (the shift right operator is to scale the depth precision)
        if (p > OT_LEN || p < 30)
            return;

        // Load primitive color even though gte_ncs() doesn't use it.
        // This is so the GTE will output a color result with the
        // correct primitive code.
        gte_ldrgb(&pol4->r0);

        setRGB0(pol4, 127, 127, 127);

        // Set the UVs
        const uint16_t uv_x = (63 * (j%4));
        const uint16_t uv_y = (63 * (j/4));
        setUV4(
            pol4,
            uv_x, uv_y,
            uv_x + 63, uv_y,
            uv_x, uv_y + 63,
            uv_x + 63, uv_y + 63
        );

        pol4->tpage = texture_tpage;
        pol4->clut = texture_clut;

        // Sort primitive to the ordering table
        addPrim(active_buff->ot + p, pol4);

        // Advance to make another primitive
        pol4++;
    }

    // Update nextpri variable
    ctx->nextpri = (char *)pol4;
}


static void SortPlanes(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    VECTOR pos_r = {pos.vx + 200, pos.vy, pos.vz};
    VECTOR pos_l = {pos.vx - 200, pos.vy, pos.vz};

    SortSimplePlane(ctx, texture, pos, rot);
    SortVerticalSubPlane(ctx, texture, pos_r, rot);
    SortSubdividedPlane(ctx, texture, pos_l, rot);
}

#endif