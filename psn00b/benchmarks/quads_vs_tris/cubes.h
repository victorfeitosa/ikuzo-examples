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

// Cube mesh
SVECTOR cube_verts[] = {{-40, -40, -40, 0}, {40, -40, -40, 0}, {-40, 40, -40, 0}, {40, 40, -40, 0},
                       {40, -40, 40, 0},   {-40, -40, 40, 0}, {40, 40, 40, 0},   {-40, 40, 40, 0}};

// Cube face normals
SVECTOR cube_norms[] = {{0, 0, -ONE, 0}, {0, 0, ONE, 0},  {0, -ONE, 0, 0},
                        {0, ONE, 0, 0},  {-ONE, 0, 0, 0}, {ONE, 0, 0, 0}};

SVECTOR *scratch_verts = (SVECTOR*)0x1F800000;

// Cube vertex indexes
INDEX cube_indices[] = {{0, 1, 2, 3}, {4, 5, 6, 7}, {5, 4, 0, 1}, {6, 7, 3, 2}, {0, 2, 5, 7}, {3, 1, 6, 4}};

// Number of cubes to render, number of faces rendered, rotation vector, camera position, rendering method flag
uint32_t num_cubes = 240;
uint32_t num_faces = 0;
SVECTOR rot = {0, 0};
VECTOR pos = {-1700, -800, 2000};
uint8_t method = 0;

// Number of faces of a cube
#define CUBE_FACES 6

// Light matrix
// Each row represents one light source position
MATRIX LIGHT_MTX = {
    //  X,     Y,     Z
    -2048, -2048, -2048, 0, 0, 0, 0, 0, 0};

// World and Light matrices, polygon colors
MATRIX *LIGHT_SPACE;
MATRIX *WORLD_SPACE;
MATRIX poly_colors[] = {
    {ONE, 0, 0, 0, 0, 0, 0, 0, 0},   {0, 0, 0, ONE, 0, 0, 0, 0, 0},   {0, 0, 0, 0, 0, 0, ONE, 0, 0},
    {0, 0, 0, ONE, 0, 0, ONE, 0, 0}, {ONE, 0, 0, 0, 0, 0, ONE, 0, 0}, {ONE, 0, 0, ONE, 0, 0, 0, 0, 0},
};

void InitCubeDefs()
{
  scratch_verts = malloc(sizeof(cube_verts));
}

// Triangle definitions
void SortTrisCube(RenderContext *ctx, VECTOR pos, SVECTOR rot)
{
    int i, p;

    POLY_F3 *pol3 = (POLY_F3 *)ctx->nextpri;
    DrawEnv *active_buff = active_buffer(ctx);

    RotMatrix(&rot, WORLD_SPACE);
    TransMatrix(WORLD_SPACE, &pos);
    MulMatrix0(&LIGHT_MTX, WORLD_SPACE, LIGHT_SPACE);

    gte_SetRotMatrix(WORLD_SPACE);
    gte_SetTransMatrix(WORLD_SPACE);
    gte_SetLightMatrix(LIGHT_SPACE);

    for (i = 0; i < CUBE_FACES; i++)
    {
        // Draw the first triangle
        gte_SetColorMatrix(&poly_colors[i]);

        // Load the first 3 vertices of a quad to the GTE
        gte_ldv3(&cube_verts[cube_indices[i].v0], &cube_verts[cube_indices[i].v1], &cube_verts[cube_indices[i].v2]);

        // Rotation, Translation and Perspective Triple
        gte_rtpt();

        // Compute normal clip for backface culling
        gte_nclip();

        // Get result
        gte_stopz(&p);

        // Skip this face if backfaced
        if (p < 0)
            continue;

        // Calculate average Z for depth sorting
        gte_avsz3();
        gte_stotz(&p);

        // Skip if outside of far plane with a precision of 1/2
        if ((p >> 1) > OT_LEN)
            continue;

        // Set poly primitive
        setPolyF3(pol3);

        // Set the projected vertices to the primitive
        gte_stsxy0(&pol3->x0);
        gte_stsxy1(&pol3->x1);
        gte_stsxy2(&pol3->x2);

        // Load primitive color even though gte_ncs() doesn't use it.
        // This is so the GTE will output a color result with the
        // correct primitive code.
        gte_ldrgb(&pol3->r0);

        // Load the face normal vector
        gte_ldv0(&cube_norms[i]);

        // Normal Color Single
        gte_ncs();

        // Store color result to the primitive
        gte_strgb(&pol3->r0);

        // Sort primitive to the ordering table with a precision of 1/2
        addPrim(active_buff->ot + (p >> 1), pol3);
        // Advance primitive pointer
        pol3++;

        // Update number of faces drawn
        num_faces++;

        // Draws second triangle
        // and skip average Z calculation and color setting to save GTE cycles
        gte_ldv3(&cube_verts[cube_indices[i].v1], &cube_verts[cube_indices[i].v2], &cube_verts[cube_indices[i].v3]);

        gte_rtpt();

        // Skip if triangle is outside of the far plane
        if ((p >> 1) > OT_LEN)
            continue;

        setPolyF3(pol3);

        gte_stsxy0(&pol3->x0);
        gte_stsxy1(&pol3->x1);
        gte_stsxy2(&pol3->x2);

        gte_strgb(&pol3->r0);

        addPrim(active_buff->ot + (p >> 1), pol3);

        // Advance primitive pointer for next primitive
        pol3++;
        // Update number of faces drawn
        num_faces++;
    }

    // Update nextpri variable
    ctx->nextpri = (char *)pol3;
}

void SortQuadCube(RenderContext *ctx, VECTOR pos, SVECTOR rot)
{
    int i, p;

    POLY_F4 *pol4 = (POLY_F4 *)ctx->nextpri; // Flat shaded textured quad primitive pointer
    DrawEnv *active_buff = active_buffer(ctx);

    // Set rotation and translation to the matrix
    RotMatrix(&rot, WORLD_SPACE);
    TransMatrix(WORLD_SPACE, &pos);

    // Multiply light matrix by rotation matrix so light source
    // won't appear relative to the model's rotation
    MulMatrix0(&LIGHT_MTX, WORLD_SPACE, LIGHT_SPACE);

    // Set rotation and translation matrix
    gte_SetRotMatrix(WORLD_SPACE);
    gte_SetTransMatrix(WORLD_SPACE);

    // Set light matrix
    gte_SetLightMatrix(LIGHT_SPACE);

    for (i = 0; i < CUBE_FACES; i++)
    {
        gte_SetColorMatrix(&poly_colors[i]);
        // Load the first 3 vertices of a quad to the GTE
        gte_ldv3(&cube_verts[cube_indices[i].v0], &cube_verts[cube_indices[i].v1], &cube_verts[cube_indices[i].v2]);

        // Rotation, Translation and Perspective Triple
        gte_rtpt();

        // Compute normal direction for backface culling
        gte_nclip();

        // Store result of cross product (nclip) in p
        gte_stopz(&p);

        // Skip this face if backfaced
        if (p < 0)
            continue;

        // Initialize a quad primitive
        setPolyF4(pol4);

        // Set the projected vertices to the primitive
        gte_stsxy0(&pol4->x0);
        gte_stsxy1(&pol4->x1);
        gte_stsxy2(&pol4->x2);

        // Compute the last vertex and set the result
        gte_ldv0(&cube_verts[cube_indices[i].v3]);
        gte_rtps();
        gte_stsxy(&pol4->x3);

        // Calculate average Z for depth sorting and store result in p
        gte_avsz4();
        gte_stotz(&p);

        // Skip if clipping off
        // (the shift right operator is to scale the depth precision)
        if (p > OT_LEN)
            continue;

        // Load primitive color even though gte_ncs() doesn't use it.
        // This is so the GTE will output a color result with the
        // correct primitive code.
        gte_ldrgb(&pol4->r0);

        // Load the face normal
        gte_ldv0(&cube_norms[i]);

        // Normal Color Single
        gte_ncs();

        // Store result to the primitive
        gte_strgb(&pol4->r0);

        // Sort primitive to the ordering table
        addPrim(active_buff->ot + p, pol4);

        // Advance to make another primitive
        pol4++;
        num_faces++;
    }

    // Update nextpri variable
    ctx->nextpri = (char *)pol4;
}

// Initial draw method is TRIS
void (*draw_method)(RenderContext *ctx, VECTOR pos, SVECTOR rot) = &SortTrisCube;

inline void SortCubes(RenderContext *ctx)
{
    num_faces = 0;
    uint32_t i;
    for (i = 0; i < num_cubes; i++)
    {
        VECTOR cube_pos = {(pos.vx + (i % 30) * 120), (pos.vy + (i / 30) * 120), pos.vz};
        (*draw_method)(ctx, cube_pos, rot);
    }
}

#endif