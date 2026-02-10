#include "fixed_point.h"
#include "psxsn.h"
#include "psxcd.h"
#include "md2.h"

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

size_t LoadMDX(const unsigned char *file, MDX *mdx)
{
    // uint32_t fd = PCopen(file, PCDRV_MODE_READ);
    // size_t model_size;
    // if (fd < 0)
    // {
    //     return 0;
    // }

    // model_size = PClseek(fd, 0, PCDRV_SEEK_END);
    // PClseek(fd, 0, PCDRV_SEEK_SET);

    // // Read MD2 header
    // MD2_Header md2_h;
    // PCread(fd, &md2_h, sizeof(MD2_Header));

    // mdx->head.framesize = md2_h.framesize;
    // mdx->head.num_vertices = md2_h.num_vertices;
    // mdx->head.num_uv = md2_h.num_st;
    // mdx->head.num_tris = md2_h.num_tris;
    // mdx->head.num_frames = md2_h.num_frames;
    // mdx->head.mat = (uint16_t)MDX_MAT_GORAUND_TEXTURED;

    // void *data;

    // // Read MD2 vertices
    // data = md2_h.offset_st;
    // for(int i = 0; i < md2_h.num_st; i++)
    // {

    // }

    // Read MD2 triangles



    // return model_size;
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