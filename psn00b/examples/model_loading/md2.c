#include "fixed_point.h"
#include "md2.h"

static MATRIX WORLD_SPACE;

inline SVECTOR mdx_unpack_pos(MDX_M *mdx, uint16_t frame)
{
    return (SVECTOR){0, 0, 0};
}

size_t Loadmdx(MDX_M *mdx, const unsigned char *file)
{
}

void LoadmdxFromMem(MDX_M *mdx, const unsigned char *data)
{
    mdx->head = (mdx_Header *)&data[0];

    size_t offset_st = mdx->head->offset_st;
    size_t offset_tris = mdx->head->offset_tris;
    size_t offset_frames = mdx->head->offset_frames;

    mdx->texcoords = (mdx_TexCoord *)(data + offset_st);
    mdx->tris = (mdx_Tri *)(data + offset_tris);
    mdx->frames = (mdx_FrameI *)(data + offset_frames);
    mdx->current_frame = 0;

    // Transform all frame translation and scale to fiex point numbers
    for (int i = 0; i < mdx->head->num_frames; i++)
    {
        mdx_FrameI *fr = (mdx_FrameI *)((unsigned char *)mdx->frames + i * mdx->head->framesize);
        fr->scale[0] = ftofix(fr->scale[0]);
        fr->scale[1] = ftofix(fr->scale[1]);
        fr->scale[2] = ftofix(fr->scale[2]);
        fr->translate[0] = ftofix(fr->translate[0]);
        fr->translate[1] = ftofix(fr->translate[1]);
        fr->translate[2] = ftofix(fr->scale[2]);
    }
}

void Sortmdx(RenderContext *ctx, MDX_M *mdx, TIM_IMAGE *skin, VECTOR pos, SVECTOR rot, uint32_t scale)
{
    int i, p;
    POLY_FT3 *pol3 = (POLY_FT3 *)ctx->nextpri;
    DrawEnv *active_buff = active_buffer(ctx);

    uint16_t texture_tpage = getTPage(skin->mode, 1, skin->prect->x, skin->prect->y);
    uint16_t texture_clut = getClut(skin->crect->x, skin->crect->y);

    mdx_FrameI *fr = (mdx_FrameI *)((unsigned char *)mdx->frames + mdx->current_frame * mdx->head->framesize);
    mdx_Tri *tris = (mdx_Tri *)mdx->tris;

    FntPrint(-1, "F(%d, %d, %d)", fr->scale[0], fr->scale[1], fr->scale[2]);

    RotMatrix(&rot, &WORLD_SPACE);
    TransMatrix(&WORLD_SPACE, &pos);

    gte_SetRotMatrix(&WORLD_SPACE);
    gte_SetTransMatrix(&WORLD_SPACE);

    for (i = 0; i < mdx->head->num_tris; i++)
    {
        mdx_Tri *tri = (mdx_Tri *)mdx->tris + i;
        unsigned short *vids = tri->vertex; // Vertex ids of each triangle in this frame
        unsigned short *uvids = tri->st;    // UV ids of each triangle in this frame

        mdx_Vertex *verts = (mdx_Vertex *)&fr->verts;       // Position of every vertex in this frame
        mdx_TexCoord *uvs = (mdx_TexCoord *)mdx->texcoords; // Model UVs

        // Vertex position vectors, comes from the unpacked position of the vertices in the frame
        // We convert coords from right-handed orientation to left handed
        SVECTOR v1pos = {verts[vids[0]].v[0], -verts[vids[0]].v[2], verts[vids[0]].v[1]};
        SVECTOR v2pos = {verts[vids[1]].v[0], -verts[vids[1]].v[2], verts[vids[1]].v[1]};
        SVECTOR v3pos = {verts[vids[2]].v[0], -verts[vids[2]].v[2], verts[vids[2]].v[1]};

        // FntPrint(-1, "V(%d,%d,%d) (%d,%d,%d) (%d,%d,%d)\n",
        //     v1pos.vx, v1pos.vy, v1pos.vz,
        //     v2pos.vx, v2pos.vy, v2pos.vz,
        //     v3pos.vx, v3pos.vy, v3pos.vz);

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
        gte_stsxy0(&pol3->x0);
        gte_stsxy1(&pol3->x1);
        gte_stsxy2(&pol3->x2);

        setRGB0(pol3, 140, 140, 140);

        // Set the UVs
        setUV3(pol3, uvs[uvids[0]].s, uvs[uvids[0]].t, uvs[uvids[1]].s, uvs[uvids[1]].t, uvs[uvids[2]].s,
               uvs[uvids[2]].t);

        pol3->tpage = texture_tpage;
        pol3->clut = texture_clut;

        // Sort primitive to the ordering table with a precision of 1/2
        addPrim(active_buff->ot + p, pol3);

        // Advance primitive pointer
        pol3++;
    }
    // Update nextpri variable
    ctx->nextpri = (char *)pol3;
}