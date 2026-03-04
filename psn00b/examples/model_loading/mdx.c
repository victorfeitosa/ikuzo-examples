#include "mdx.h"
#include "fixed_point.h"
#include "psxcd.h"
#include "psxsn.h"
#include <stdio.h>
#include <stdlib.h>

static MATRIX WORLD_SPACE;

MDX_Material RENDER_MAT = MDX_MAT_GORAUND_TEXTURED;

inline SVECTOR mdx_unpack_pos(MDX *mdx, uint16_t f, uint16_t v)
{
    MDX_Frame *frame = (MDX_Frame *)((uint8_t *)mdx->frames + f * mdx->head.framesize);
    MDX_Vertex *vert = (MDX_Vertex *)&frame->verts + v;

    SVECTOR pos = {vert->v[0], vert->v[1], vert->v[2]};
    MATRIX scaleM = {
        .m =
            {
                {frame->scale.vx, 0, 0},
                {0, frame->scale.vy, 0},
                {0, 0, frame->scale.vz},
            },
    };

    gte_SetRotMatrix(&scaleM);
    gte_ldv0(&pos);

    gte_mvmva(0, 0, 0, 3, 1);
    VECTOR full_pos;
    gte_stlvnl(&full_pos);
    pos.vx = (full_pos.vx + frame->translate.vx) >> 8;
    pos.vy = (full_pos.vy + frame->translate.vy) >> 8;
    pos.vz = (full_pos.vz + frame->translate.vz) >> 8;

    return pos;
}

MDX *loadMDX(const unsigned char *file)
{
    uint32_t fd = PCopen(file, PCDRV_MODE_READ);
    size_t model_size = 0;
    if (fd < 0)
    {
        return NULL;
    }

    model_size = PClseek(fd, 0, PCDRV_SEEK_END);
    MDX *mdx = (MDX *)malloc(sizeof(MDX));
    PClseek(fd, 0, PCDRV_SEEK_SET);

    if (model_size <= 0 || !mdx)
    {
        printf("Failed to load model %s\n", file);
        return NULL;
    }

    // Read MDX header
    PCread(fd, &mdx->head, sizeof(MDX_Header));

    // Allocate memory for UVs, triangles and frames based on header info
    mdx->uvs = (MDX_UV *)malloc(sizeof(MDX_UV) * mdx->head.num_uv);
    mdx->tris = (MDX_POLY *)malloc(sizeof(MDX_POLY) * mdx->head.num_tris);
    mdx->frames = (MDX_Frame *)malloc(sizeof(MDX_Frame) * mdx->head.num_frames);

    PCread(fd, mdx->uvs, sizeof(MDX_UV) * mdx->head.num_uv);
    PCread(fd, mdx->tris, sizeof(MDX_POLY) * mdx->head.num_tris);
    PCread(fd, mdx->frames, mdx->head.framesize * mdx->head.num_frames);

    return mdx;
}

void printMDXInfo(MDX *mdx)
{
    printf("MDX Model Info:\n");
    printf("Framesize: %d bytes\n", mdx->head.framesize);
    printf("Vertices per frame: %d\n", mdx->head.num_vertices);
    printf("Number of UVs: %d\n", mdx->head.num_uv);
    printf("Number of triangles: %d\n", mdx->head.num_tris);
    printf("Number of frames: %d\n", mdx->head.num_frames);
    printf("Material type: %d\n", mdx->head.mat);
    printf("CLUT: %d\n", mdx->head.clut);
    printf("Texture page: %d\n", mdx->head.tpage);

    printf("Offsets - UV: %d, Tris: %d, Frames: %d, End: %d\n", mdx->head.offset_uv, mdx->head.offset_tris,
           mdx->head.offset_frames, mdx->head.offset_end);

    // for (int i = 0; i < mdx->head.num_uv; i++)
    // {
    //     printf("UV %d: (u: %d, v: %d)\n", i, mdx->uvs[i].u, mdx->uvs[i].v);
    // }

    // for (int i = 0; i < mdx->head.num_tris; i++)
    // {
    //     printf("Triangle %d: Vertices(%d, %d, %d) UVs(%d, %d, %d)\n", i, mdx->tris[i].vertex[0], mdx->tris[i].vertex[1],
    //            mdx->tris[i].vertex[2], mdx->tris[i].uv[0], mdx->tris[i].uv[1], mdx->tris[i].uv[2]);
    // }

    for (int i = 0; i < 1; i++)
    {
        MDX_Frame *frame = &mdx->frames[i];
        printf("Frame %d: Scale(%d, %d, %d) Translate(%d, %d, %d)\n", i, frame->scale.vx, frame->scale.vy,
               frame->scale.vz, frame->translate.vx, frame->translate.vy, frame->translate.vz);
        for (int j = 0; j < mdx->head.num_vertices; j++)
        {
            MDX_Vertex *verts = (MDX_Vertex *)&frame->verts + j;
            printf("\tVertex %d: (%d, %d, %d) N: %d\n", j, verts[j].v[0], verts[j].v[1], verts[j].v[2], verts[j].normalIndex);
        }
    }
}

void sortMDX(RenderContext *ctx, MDX *mdx, VECTOR pos, SVECTOR rot, uint32_t scale, MDX_Info *info, TIM_IMAGE *tex)
{
    int i, p;
    POLY_FT3 *pol3 = (POLY_FT3 *)ctx->nextpri;
    DrawEnv *active_buff = active_buffer(ctx);

    uint16_t texture_tpage = getTPage(tex->mode, 1, tex->prect->x, tex->prect->y);
    uint16_t texture_clut = getClut(tex->crect->x, tex->crect->y);

    MDX_Frame *fr = (MDX_Frame *)((uint8_t *)mdx->frames + info->current_frame * mdx->head.framesize);
    MDX_POLY *tris = (MDX_POLY *)mdx->tris;

    RotMatrix(&rot, &WORLD_SPACE);
    TransMatrix(&WORLD_SPACE, &pos);

    for (i = 0; i < mdx->head.num_tris; i++)
    {
        MDX_POLY *tri = (MDX_POLY *)mdx->tris + i;
        unsigned short *vids = tri->vertex; // Vertex ids of each triangle in this frame
        unsigned short *uvids = tri->uv;    // UV ids of each triangle in this frame

        MDX_Vertex *verts = (MDX_Vertex *)fr->verts; // Position of every vertex in this frame
        MDX_UV *uvs = (MDX_UV *)mdx->uvs;            // Model UVs

        // Vertex position vectors, comes from the unpacked position of the vertices in the frame
        // We convert coords from right-handed orientation to left handed
        SVECTOR v1pos = mdx_unpack_pos(mdx, info->current_frame, vids[0]);
        SVECTOR v2pos = mdx_unpack_pos(mdx, info->current_frame, vids[1]);
        SVECTOR v3pos = mdx_unpack_pos(mdx, info->current_frame, vids[2]);

        gte_SetRotMatrix(&WORLD_SPACE);
        gte_SetTransMatrix(&WORLD_SPACE);

        // Load vertex positions
        gte_ldv3(&v1pos, &v2pos, &v3pos);

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

        // Skip if outside of far plane or behind near plane to avoid projection squishing
        if (p > OT_LEN || p < 20)
            continue;

        // Set poly primitive
        setPolyFT3(pol3);

        // Set the projected vertices to the primitive
        gte_stsxy3_ft3(pol3);

        setRGB0(pol3, 140, 140, 140);

        // Set the UVs
        setUV3(pol3, uvs[uvids[0]].u, uvs[uvids[0]].v, uvs[uvids[1]].u, uvs[uvids[1]].v, uvs[uvids[2]].u,
               uvs[uvids[2]].v);

        pol3->tpage = texture_tpage;
        pol3->clut = texture_clut;

        // Sort primitive to the ordering table with a precision of 1/2
        addPrim(active_buff->ot + (p << 2), pol3);

        // Advance primitive pointer
        pol3++;
    }
    // Update nextpri variable
    ctx->nextpri = (char *)pol3;
}
