// Copyright (C) 2014 Alexey Rusev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <OP/OP_Node.h>
#include <SOP/SOP_Node.h>
#include <OP/OP_Parameters.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <GU/GU_Detail.h>
#include <GA/GA_Attribute.h>
#include <SYS/SYS_Math.h>
#include <iostream>
#include "sop_gpattern.h"


const char *inputLabels[] = {"Pattern geometry", "Template geometry"};

OP_Node *SOP_Gpattern::makeOP(OP_Network *net, const char *name, OP_Operator *op) {
    return new SOP_Gpattern(net, name, op);
}

SOP_Gpattern::SOP_Gpattern(OP_Network *net, const char *name, OP_Operator *op)
        :SOP_Node(net, name, op)
{
}
static PRM_Name use_uvs_prm("use_uv_attr", "Use UV Attribute");
static PRM_Name num_tiles_prm("tiles", "Num Tiles");
static PRM_Name scale_compensate_prm("scale", "Scale Compensate");
static PRM_Range scalerange_prm(PRM_RANGE_UI, -1, PRM_RANGE_UI, 2);

PRM_Template
SOP_Gpattern::parmsTemplatesList[] = {
        PRM_Template(PRM_TOGGLE_E, 1, &use_uvs_prm, PRMzeroDefaults),
        PRM_Template(PRM_UVW, 2, &num_tiles_prm, PRMoneDefaults),
        PRM_Template(PRM_FLT_E, 1, &scale_compensate_prm, PRMoneDefaults, 0, &scalerange_prm),
        PRM_Template(),
};

bool
SOP_Gpattern::updateParmsFlags()
{
    bool changes = false;
    changes |= enableParm(num_tiles_prm.getToken(), !PRMUseUVs());
    return changes;
}

SOP_Gpattern::~SOP_Gpattern() {
}

void SOP_Gpattern::pointRelativeToBbox(const UT_Vector3 &pt, UT_Vector2 &uv)
{
    uv[0] = SYSfit(pt[0], bbox_min[0], bbox_max[0], 0, 1);
    uv[1] = SYSfit(pt[1], bbox_min[1], bbox_max[1], 0, 1);
}

void computeTemplateMaxUV(const GU_Detail *template_geo, float &max_u, float &max_v)
{
    auto uv_hdl = GA_ROHandleV3(template_geo->findTextureAttribute(GA_ATTRIB_POINT));
    GA_Offset ptoff;
    max_u, max_v = 0;
    GA_FOR_ALL_PTOFF(template_geo, ptoff)
        {
            UT_Vector3 uv = uv_hdl.get(ptoff);
            if (uv[0] > max_u) max_u = uv[0];
            if (uv[1] > max_v) max_v = uv[1];
        }

}
void SOP_Gpattern::computePatternGeoAttibs()
{
    UT_BoundingBox bbox;
    pattern_geo->getBBox(&bbox);
    bbox_min = bbox.minvec();
    bbox_max = bbox.maxvec();
    GA_Offset ptoff;
    GA_ROHandleV3 ph = GA_ROHandleV3(pattern_geo->getP());
    auto bbox_uv_hdl = GA_RWHandleV2(pattern_geo->addFloatTuple(GA_ATTRIB_POINT, "bboxuv", 2));
    auto point_dist_hdl = GA_RWHandleR(pattern_geo->addFloatTuple(GA_ATTRIB_POINT, "point_dist", 1));
    UT_Vector2 bbox_uv;
    GA_FOR_ALL_PTOFF(pattern_geo, ptoff)
    {
        const UT_Vector3 ppos = ph.get(ptoff);
        pointRelativeToBbox(ppos, bbox_uv);
        bbox_uv_hdl.set(ptoff, bbox_uv);

        UT_Vector3 pos_on_plane = ppos;
        pos_on_plane[2] = 0;
        fpreal dist = (ppos - pos_on_plane).length();
        point_dist_hdl.set(ptoff, dist);

    }

}

void movePatternToTile(GU_Detail *pattern_copy,
                        const GEO_Primitive *template_prim,
                        const int tile,
                        const float max_u,
                        const float max_v,
                        const float scale)
{
    int u_tiles = SYSceil(max_u);
    float du = 0;
    float dv = 0;
    float ru, rv;
    fpreal32 s, t;

    if (tile == 0)
        du, dv = 0;
    else if (tile > 0 && tile < u_tiles)
        du += tile;
    else
    {
        du += tile % u_tiles;
        dv += SYSfloor(static_cast<fpreal>(tile/u_tiles));
    }
    GA_ROHandleV2 bboxuv_hdl(pattern_copy->findFloatTuple(GA_ATTRIB_POINT, "bboxuv"));
    GA_RWHandleV3 ph = GA_RWHandleV3(pattern_copy->getP());
    GA_RWHandleR point_dist_hdl = GA_RWHandleR(pattern_copy->findFloatTuple(GA_ATTRIB_POINT, "point_dist"));
    for (GA_Iterator it(pattern_copy->getPointRange()); !it.atEnd(); it.advance())
    {
        UT_Vector4 primP;
        UT_Vector3 primN;
        UT_Vector2 bboxuv = bboxuv_hdl.get(*it);
        ru = bboxuv[0] + du;
        rv = bboxuv[1] + dv;

        s = SYSfit(ru, (fpreal32)0.0, max_u, (fpreal32)0.0, (fpreal32)0.99999);
        t = SYSfit(rv, (fpreal32)0.0, max_v, (fpreal32)0.0, (fpreal32)0.99999);
        template_prim->evaluateInteriorPoint(primP, s, t);
        template_prim->evaluateNormalVector(primN, s, t);
        primN.normalize();
        primP = primP + primN * point_dist_hdl.get(*it) * scale;
        ph.set(*it, primP);
    }

}

void
SOP_Gpattern::cookTilesPartial(ThreadParms *parms, const UT_JobInfo &info) {
    int i,n;
    GU_Detail *copy;
    for (info.divideWork(parms->numtiles, i, n); i < n; i++)
    {
        copy = new GU_Detail(parms->pattern_geo);
        movePatternToTile(copy, parms->template_prim, i, parms->max_u, parms->max_v, parms->scale);
        info.lock();
        gdp->merge(*copy);
        info.unlock();

    }
    delete copy;
}

OP_ERROR SOP_Gpattern::cookMySop(OP_Context &context) {
    if (lockInput(0, context) >= UT_ERROR_ABORT)
        return error();
    if (lockInput(1, context) >= UT_ERROR_ABORT) {
        addError(SOP_ERR_INVALID_SRC, "No second input");
        return error();
    }
    gdp->clearAndDestroy();

    const GU_Detail *template_geo = inputGeo(1, context);
    if (template_geo->getNumPrimitives() == 0)
    {
        addError(SOP_ERR_INVALID_SRC, "No primitives in second input");
        return error();
    }
    const GEO_Primitive *template_prim = template_geo->getGEOPrimitive(template_geo->primitiveOffset(0));

    pattern_geo = new GU_Detail(inputGeo(0, context));
    // Compute pattern attribs
    computePatternGeoAttibs();
    // Compute number of tiles from  template uv attrib
    float max_u, max_v = 0;
    if (PRMUseUVs())
    {
		/*auto uv_aref = template_geo->findTextureAttribute(GA_ATTRIB_POINT);
        if (!uv_aref->isValid())
        {
            addError(SOP_ERR_INVALID_SRC, "No uv attribute on points");
            return error();
        }*/
        computeTemplateMaxUV(template_geo, max_u, max_v);
    }
    else
    {
        UT_Vector2 tiles;
        PRMNumTiles(context.getTime(), tiles);
        max_u = SYSmax(1.0, tiles[0]);
        max_v = SYSmax(1.0, tiles[1]);
    }

    int num_tiles = SYSmax(1, int(SYSceil(max_u) * SYSceil(max_v)));
    float scale = PRMScale(context.getTime());

    parms.numtiles = num_tiles;
    parms.max_u = max_u;
    parms.max_v = max_v;
    parms.scale = scale;
    parms.pattern_geo = pattern_geo;
    parms.template_prim = template_prim;

    cookTiles(&parms);

    delete pattern_geo;
    return error();

}

//void
//newSopOperator(OP_OperatorTable *table)
//{
//    table->addOperator(
//            new OP_Operator("gpattern",
//                    "GPattern",
//                    SOP_Gpattern::makeOP,
//                    SOP_Gpattern::parmsTemplatesList,
//                    2,
//                    2,
//                    0,
//                    0,
//                    inputLabels));
//}


void
newSopOperator(OP_OperatorTable *table)
{
	table->addOperator(
		new OP_Operator("gpattern",
			"GPattern",
			SOP_Gpattern::makeOP,
			SOP_Gpattern::parmsTemplatesList,
			1,
			2));
}
