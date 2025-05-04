#ifndef GEOM_H
#define GEOM_H

#include "display.h"
#include <stdio.h>
#include <stdint-gcc.h>
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
// Cross-like subdivided plane mesh
SVECTOR cross_plane_verts[] = {
    {-100, 100, 0},
    {100, 100, 0},
    {100, -100, 0},
    {-100, -100, 0},
    {0, 0, 0}
};
DVECTOR cross_uvs[] = {
    {0, 255},
    {255, 255},
    {255, 0},
    {0, 0},
    {127, 127}
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
INDEX simple_plane_indexes = {0, 1, 2, 3};
INDEX cross_plane_indexes[] = {{0, 1, 4}, {1, 2, 4}, {2, 3, 4}, {3, 0, 4}};
INDEX vertical_plane_indexes[] = {{0, 1, 5, 6}, {1, 2, 6, 7}, {2, 3, 7, 8}, {3, 4, 8, 9}};
INDEX subdivided_plane_indexes[] = {
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
    LINE_F3 *line;
    DrawEnv *active_buff = active_buffer(ctx);
    uint16_t scoords[4][2];

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
        &simple_plane_verts[simple_plane_indexes.v0],
        &simple_plane_verts[simple_plane_indexes.v1],
        &simple_plane_verts[simple_plane_indexes.v2]
    );

    // Rotation, Translation and Perspective Triple
    gte_rtpt();

    // Compute normal direction for backface culling
    gte_nclip();

    // Store result of cross product (nclip) in p
    gte_stopz(&p);

    // Initialize a quad primitive
    setPolyFT4(pol4);
    setRGB0(pol4, 127, 127, 127);

    // Set the projected vertices to the primitive
    gte_stsxy0(&pol4->x0);
    gte_stsxy1(&pol4->x1);
    gte_stsxy2(&pol4->x2);

    // Compute the last vertex and set the result
    gte_ldv0(&simple_plane_verts[simple_plane_indexes.v3]);
    gte_rtps();
    gte_stsxy(&pol4->x3);

    // Calculate average Z for depth sorting and store result in p
    gte_avsz4();
    gte_stotz(&p);

    // Skip if clipping off
    // (the shift right operator is to scale the depth precision)
    if (p > OT_LEN || p < 16)
        return;

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
    // pol4++;
    // Update nextpri variable
    ctx->nextpri = (char *)(pol4 + sizeof(POLY_FT4));

    // Draw the wireframe
    line = (LINE_F3 *)ctx->nextpri;
    setLineF3(line);
    setSemiTrans(line, 1);
    setRGB0(line, 40, 10, 10);

    line->x0 = pol4->x0; line->y0 = pol4->y0;
    line->x1 = pol4->x1; line->y1 = pol4->y1;
    line->x2 = pol4->x3; line->y2 = pol4->y3;

    addPrim(active_buff->ot + (p >> 2), line);
    line++;

    setLineF3(line);
    setSemiTrans(line, 1);
    setRGB0(line, 40, 10, 10);

    line->x0 = pol4->x0; line->y0 = pol4->y0;
    line->x1 = pol4->x2; line->y1 = pol4->y2;
    line->x2 = pol4->x3; line->y2 = pol4->y3;

    addPrim(active_buff->ot + (p >> 2), line);
    line++;
    ctx->nextpri = (char *)line;
}

void SortCrossPlane(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    int i, p, l;

    POLY_FT3 *pol3 = (POLY_FT3 *)ctx->nextpri;
    LINE_F3 *line;
    DrawEnv *active_buff = active_buffer(ctx);

    uint16_t texture_tpage = getTPage(texture->mode, 1, texture->prect->x, texture->prect->y);
    uint16_t texture_clut = getClut(texture->crect->x, texture->crect->y);

    // Set rotation and translation to the matrix
    RotMatrix(&rot, WORLD_SPACE);
    TransMatrix(WORLD_SPACE, &pos);

    // Set rotation and translation matrix
    gte_SetRotMatrix(WORLD_SPACE);
    gte_SetTransMatrix(WORLD_SPACE);

    // Draws each triangle
    for(size_t j=0; j < 4; j++)
    {
        // Load the first 3 vertices of a quad to the GTE
        gte_ldv3(
            &cross_plane_verts[cross_plane_indexes[j].v0],
            &cross_plane_verts[cross_plane_indexes[j].v1],
            &cross_plane_verts[cross_plane_indexes[j].v2]
        );

        // Rotation, Translation and Perspective Triple
        gte_rtpt();

        // Compute normal direction for backface culling
        gte_nclip();

        // Store result of cross product (nclip) in p
        gte_stopz(&p);

        // Initialize a tris primitive
        setPolyFT3(pol3);

        // Set the projected vertices to the primitive
        gte_stsxy0(&pol3->x0);
        gte_stsxy1(&pol3->x1);
        gte_stsxy2(&pol3->x2);

        // Calculate average Z for depth sorting and store result in p
        gte_avsz3();
        gte_stotz(&p);

        // Skip if clipping off
        // (the shift right operator is to scale the depth precision)
        if (p > OT_LEN || p < 16) {
            l = 0;
            continue;
        }

        // Load primitive color even though gte_ncs() doesn't use it.
        // This is so the GTE will output a color result with the
        // correct primitive code.
        gte_ldrgb(&pol3->r0);

        setRGB0(pol3, 127, 127, 127);

        // Set the UVs
        INDEX tindex = cross_plane_indexes[j];
        setUV3(
            pol3,
            cross_uvs[tindex.v0].vx, cross_uvs[tindex.v0].vy,
            cross_uvs[tindex.v1].vx, cross_uvs[tindex.v1].vy,
            cross_uvs[tindex.v2].vx, cross_uvs[tindex.v2].vy
        );

        pol3->tpage = texture_tpage;
        pol3->clut = texture_clut;

        // Sort primitive to the ordering table
        addPrim(active_buff->ot + p, pol3);

        // Advance to make another primitive
        // pol3++;
        ctx->nextpri = (char *)(pol3 + sizeof(POLY_FT3));

        // Draw the wireframe
        line = (LINE_F3 *)ctx->nextpri;
        setLineF3(line);
        setSemiTrans(line, 1);
        setRGB0(line, 40, 10, 10);

        line->x0 = pol3->x0; line->y0 = pol3->y0;
        line->x1 = pol3->x1; line->y1 = pol3->y1;
        line->x2 = pol3->x2; line->y2 = pol3->y2;

        addPrim(active_buff->ot + (p >> 2), line);
        line++;

        pol3 = (POLY_FT3 *)line;
        l = 1;
    }

    // Update nextpri variable
    ctx->nextpri = l ? (char *)line : (char*)pol3;
}

void SortVerticalSubPlane(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    int i, p, l;

    POLY_FT4 *pol4 = (POLY_FT4 *)ctx->nextpri;
    LINE_F3 *line;
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
    for(size_t j=0; j < 4; j++)
    {
        // Load the first 3 vertices of a quad to the GTE
        gte_ldv3(
            &vertical_plane_verts[vertical_plane_indexes[j].v0],
            &vertical_plane_verts[vertical_plane_indexes[j].v1],
            &vertical_plane_verts[vertical_plane_indexes[j].v2]
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
        gte_ldv0(&vertical_plane_verts[vertical_plane_indexes[j].v3]);
        gte_rtps();
        gte_stsxy(&pol4->x3);

        // Calculate average Z for depth sorting and store result in p
        gte_avsz3();
        gte_stotz(&p);

        // Skip if clipping off
        // (the shift right operator is to scale the depth precision)
        if (p > OT_LEN || p < 16)
        {
            l = 0;
            continue;
        }

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
        // pol4++;
        ctx->nextpri = (char *)(pol4 + sizeof(POLY_FT4));

        // Draw the wireframe
        line = (LINE_F3 *)ctx->nextpri;
        setLineF3(line);
        setSemiTrans(line, 1);
        setRGB0(line, 40, 10, 10);

        line->x0 = pol4->x0; line->y0 = pol4->y0;
        line->x1 = pol4->x1; line->y1 = pol4->y1;
        line->x2 = pol4->x3; line->y2 = pol4->y3;

        addPrim(active_buff->ot + (p >> 2), line);
        line++;

        setLineF3(line);
        setSemiTrans(line, 1);
        setRGB0(line, 40, 10, 10);

        line->x0 = pol4->x0; line->y0 = pol4->y0;
        line->x1 = pol4->x2; line->y1 = pol4->y2;
        line->x2 = pol4->x3; line->y2 = pol4->y3;

        addPrim(active_buff->ot + (p >> 2), line);
        line++;
        pol4 = (POLY_FT4 *)line;
        l = 1;
    }

    // Update nextpri variable
    ctx->nextpri = l ? (char *)line : (char*)pol4;
}

void SortSubdividedPlane(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    int i, p, l;

    POLY_FT4 *pol4 = (POLY_FT4 *)ctx->nextpri;
    LINE_F3 *line;
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
            &sub_plane_verts[subdivided_plane_indexes[j].v0],
            &sub_plane_verts[subdivided_plane_indexes[j].v1],
            &sub_plane_verts[subdivided_plane_indexes[j].v2]
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
        gte_ldv0(&sub_plane_verts[subdivided_plane_indexes[j].v3]);
        gte_rtps();
        gte_stsxy(&pol4->x3);

        // Calculate average Z for depth sorting and store result in p
        gte_avsz4();
        gte_stotz(&p);

        // Skip if clipping off
        // (the shift right operator is to scale the depth precision)
        if (p > OT_LEN || p < 16)
        {
            l = 0;
            continue;
        }

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
        // pol4++;
        ctx->nextpri = (char *)(pol4 + sizeof(POLY_FT4));

        // Draw the wireframe
        line = (LINE_F3 *)ctx->nextpri;
        setLineF3(line);
        setSemiTrans(line, 1);
        setRGB0(line, 40, 10, 10);

        line->x0 = pol4->x0; line->y0 = pol4->y0;
        line->x1 = pol4->x1; line->y1 = pol4->y1;
        line->x2 = pol4->x3; line->y2 = pol4->y3;

        addPrim(active_buff->ot + (p >> 2), line);
        line++;

        setLineF3(line);
        setSemiTrans(line, 1);
        setRGB0(line, 40, 10, 10);

        line->x0 = pol4->x0; line->y0 = pol4->y0;
        line->x1 = pol4->x2; line->y1 = pol4->y2;
        line->x2 = pol4->x3; line->y2 = pol4->y3;

        addPrim(active_buff->ot + (p >> 2), line);
        line++;
        pol4 = (POLY_FT4 *)line;
        l = 1;
    }

    // Update nextpri variable
    ctx->nextpri = l ? (char *)line : (char *)pol4;
}


static void SortPlanes(RenderContext *ctx, TIM_IMAGE *texture, VECTOR pos, SVECTOR rot)
{
    VECTOR pos_simple = {pos.vx - 300, pos.vy, pos.vz};
    VECTOR pos_cross = {pos.vx - 100, pos.vy, pos.vz};
    VECTOR pos_vert = {pos.vx + 100, pos.vy, pos.vz};
    VECTOR pos_subd = {pos.vx + 300, pos.vy, pos.vz};

    SortSimplePlane(ctx, texture, pos_simple, rot);
    SortCrossPlane(ctx, texture, pos_cross, rot);
    SortVerticalSubPlane(ctx, texture, pos_vert, rot);
    SortSubdividedPlane(ctx, texture, pos_subd, rot);
}

#endif