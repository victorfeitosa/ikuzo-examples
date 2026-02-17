#include "fixed_point.h"
#include "psxsn.h"
#include "psxcd.h"
#include "md2.h"
#include <stdio.h>

static MATRIX WORLD_SPACE;
static MDX_Material RENDER_MAT = MDX_MAT_GORAUND_TEXTURED;

inline SVECTOR mdx_unpack_pos(MDX *mdx, uint16_t frame_number, uint32_t vertex_index)
{
    MDX_Frame *frame = &mdx->frames[frame_number];
    uint8_t *packed_pos = frame->verts[vertex_index].v;
    SVECTOR pos;

    pos.vx = (packed_pos[0] * frame->scale.vx) + frame->translate.vx;
    pos.vy = (packed_pos[1] * frame->scale.vy) + frame->translate.vy;
    pos.vz = (packed_pos[2] * frame->scale.vz) + frame->translate.vz;

    return pos;
}

size_t LoadMDX(const unsigned char *md2_file, MDX *mdx)
{
    uint32_t fd = PCopen(md2_file, PCDRV_MODE_READ);
    size_t md2_size;
    if (fd < 0)
    {
        return 0;
    }

    md2_size = PClseek(fd, 0, PCDRV_SEEK_END);
    PClseek(fd, 0, PCDRV_SEEK_SET);

    // Read File and MD2 header
    uint8_t md2_data[md2_size];
    PCread(fd, &md2_data, md2_size);

    MD2_Header *md2_h = (MD2_Header *)md2_data;

    mdx->head.num_vertices = md2_h->num_vertices;
    mdx->head.num_uv = md2_h->num_st;
    mdx->head.num_tris = md2_h->num_tris;
    mdx->head.num_frames = md2_h->num_frames;
    mdx->head.framesize = sizeof(SVECTOR) + sizeof(VECTOR) + (sizeof(MDX_Vertex) * md2_h->num_vertices);
    mdx->head.mat = (uint16_t)MDX_MAT_GORAUND_TEXTURED;

    // Read MD2 UV coords
    mdx->head.offset_uv = sizeof(MDX_Header);
    mdx->uvs = (MDX_UV *)((uint8_t *)mdx + mdx->head.offset_uv);
    MD2_TexCoord *md2_st = (MD2_TexCoord *)(md2_data + md2_h->offset_st);

    for (int i = 0; i < mdx->head.num_uv; i++)
    {
        mdx->uvs[i].u = md2_st[i].s;
        mdx->uvs[i].v = md2_st[i].t;
    }

    // Read MD2 triangles
    mdx->head.offset_tris = mdx->head.offset_uv + (mdx->head.num_uv * sizeof(MDX_UV));
    mdx->tris = (MDX_POLY *)((uint8_t *)mdx + mdx->head.offset_tris);
    MD2_Tri *md2_tris = (MD2_Tri *)(md2_data + md2_h->offset_tris);
    for (int i = 0; i < mdx->head.num_tris; i++)
    {
        mdx->tris[i].vertex[0] = md2_tris[i].vertex[0];
        mdx->tris[i].vertex[1] = md2_tris[i].vertex[1];
        mdx->tris[i].vertex[2] = md2_tris[i].vertex[2];

        mdx->tris[i].uv[0] = md2_tris[i].st[0];
        mdx->tris[i].uv[1] = md2_tris[i].st[1];
        mdx->tris[i].uv[2] = md2_tris[i].st[2];
    }

    // Read MD2 frames and convert scale and translate to fixed point
    mdx->head.offset_frames = mdx->head.offset_tris + (mdx->head.num_tris * sizeof(MDX_POLY));
    mdx->frames = (MDX_Frame *)((uint8_t *)mdx + mdx->head.offset_frames);
    printf("Frame size: %d\n", mdx->head.framesize);
    for (uint16_t i = 0; i < mdx->head.num_frames; i++)
    {
        MD2_Frame *md2_frame = (MD2_Frame *)(md2_data + md2_h->offset_frames + (i * md2_h->framesize));
        MDX_Frame *mdx_frame = (MDX_Frame *)((uint8_t *)mdx->frames + (i * mdx->head.framesize));

        printf("%d Scale: (%d, %d, %d) Translate: (%d, %d, %d)\n", i, ftofix(md2_frame->scale[0]), ftofix(md2_frame->scale[1]), ftofix(md2_frame->scale[2]), ftofix(md2_frame->translate[0]), ftofix(md2_frame->translate[1]), ftofix(md2_frame->translate[2]));
        mdx_frame->scale.vx = ftofix(md2_frame->scale[0]);
        mdx_frame->scale.vy = ftofix(md2_frame->scale[1]);
        mdx_frame->scale.vz = ftofix(md2_frame->scale[2]);
        mdx_frame->translate.vx = ftofix(md2_frame->translate[0]);
        mdx_frame->translate.vy = ftofix(md2_frame->translate[1]);
        mdx_frame->translate.vz = ftofix(md2_frame->translate[2]);

        // for (uint16_t j = 0; j < md2_h->num_vertices; j++)
        // {
            // mdx_frame->verts[j].v[0] = md2_frame->verts[j].v[0];
            // mdx_frame->verts[j].v[1] = md2_frame->verts[j].v[1];
            // mdx_frame->verts[j].v[2] = md2_frame->verts[j].v[2];
            // mdx_frame->verts[j].normalIndex = md2_frame->verts[j].normalIndex;
        // }
    }

    // return model_size;
    PCclose(fd);
    return 0;
}

void SortMDX(RenderContext *ctx, MDX *mdx, VECTOR pos, SVECTOR rot, uint32_t scale, MDX_Tex *tex)
{
    // int i, p;
    // POLY_FT3 *pol3 = (POLY_FT3 *)ctx->nextpri;
    // DrawEnv *active_buff = active_buffer(ctx);

    // uint16_t texture_tpage = getTPage(skin->mode, 1, skin->prect->x, skin->prect->y);
    // uint16_t texture_clut = getClut(skin->crect->x, skin->crect->y);

    // mdx_FrameI *fr = (mdx_FrameI *)((unsigned char *)mdx->frames + mdx->current_frame * mdx->head->framesize);
    // mdx_Tri *tris = (mdx_Tri *)mdx->tris;

    // FntPrint(-1, "F(%d, %d, %d)", fr->scale[0], fr->scale[1], fr->scale[2]);

    // RotMatrix(&rot, &WORLD_SPACE);
    // TransMatrix(&WORLD_SPACE, &pos);

    // gte_SetRotMatrix(&WORLD_SPACE);
    // gte_SetTransMatrix(&WORLD_SPACE);

    // for (i = 0; i < mdx->head->num_tris; i++)
    // {
    //     mdx_Tri *tri = (mdx_Tri *)mdx->tris + i;
    //     unsigned short *vids = tri->vertex; // Vertex ids of each triangle in this frame
    //     unsigned short *uvids = tri->st;    // UV ids of each triangle in this frame

    //     mdx_Vertex *verts = (mdx_Vertex *)&fr->verts;       // Position of every vertex in this frame
    //     mdx_TexCoord *uvs = (mdx_TexCoord *)mdx->texcoords; // Model UVs

    //     // Vertex position vectors, comes from the unpacked position of the vertices in the frame
    //     // We convert coords from right-handed orientation to left handed
    //     SVECTOR v1pos = {verts[vids[0]].v[0], -verts[vids[0]].v[2], verts[vids[0]].v[1]};
    //     SVECTOR v2pos = {verts[vids[1]].v[0], -verts[vids[1]].v[2], verts[vids[1]].v[1]};
    //     SVECTOR v3pos = {verts[vids[2]].v[0], -verts[vids[2]].v[2], verts[vids[2]].v[1]};

    //     // FntPrint(-1, "V(%d,%d,%d) (%d,%d,%d) (%d,%d,%d)\n",
    //     //     v1pos.vx, v1pos.vy, v1pos.vz,
    //     //     v2pos.vx, v2pos.vy, v2pos.vz,
    //     //     v3pos.vx, v3pos.vy, v3pos.vz);

    //     // Load vertex positions
    //     gte_ldv3(&v1pos, &v2pos, &v3pos);

    //     gte_rtpt();

    //     // Compute normal clip for backface culling
    //     gte_nclip();

    //     // Get result
    //     gte_stopz(&p);

    //     // Skip this face if backfaced
    //     if (p < 0)
    //         continue;

    //     // Calculate average Z for depth sorting
    //     gte_avsz3();
    //     gte_stotz(&p);

    //     // Skip if outside of far plane or behind near plane to avoid projection squishing
    //     if (p > OT_LEN || p < 20)
    //         continue;

    //     // Set poly primitive
    //     setPolyFT3(pol3);

    //     // Set the projected vertices to the primitive
    //     gte_stsxy0(&pol3->x0);
    //     gte_stsxy1(&pol3->x1);
    //     gte_stsxy2(&pol3->x2);

    //     setRGB0(pol3, 140, 140, 140);

    //     // Set the UVs
    //     setUV3(pol3, uvs[uvids[0]].s, uvs[uvids[0]].t, uvs[uvids[1]].s, uvs[uvids[1]].t, uvs[uvids[2]].s,
    //            uvs[uvids[2]].t);

    //     pol3->tpage = texture_tpage;
    //     pol3->clut = texture_clut;

    //     // Sort primitive to the ordering table with a precision of 1/2
    //     addPrim(active_buff->ot + p, pol3);

    //     // Advance primitive pointer
    //     pol3++;
    // }
    // // Update nextpri variable
    // ctx->nextpri = (char *)pol3;
}