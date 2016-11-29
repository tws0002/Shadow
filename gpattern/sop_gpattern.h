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
#ifndef sop_gpattern_
#define sop_gpattern_

#include <UT/UT_DSOVersion.h>
#include <OP/OP_Node.h>
#include <SOP/SOP_Node.h>
#include <OP/OP_Parameters.h>
#include <UT/UT_ThreadedAlgorithm.h>
#include <UT/UT_Thread.h>

struct ThreadParms
{
    int numtiles;
    float max_u, max_v, scale;
    GU_Detail *pattern_geo;
    const GEO_Primitive *template_prim;

};

class SOP_Gpattern: public SOP_Node
{
public:
    SOP_Gpattern(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_Gpattern();

    static PRM_Template parmsTemplatesList[];
    static OP_Node *makeOP(OP_Network *net, const char *name, OP_Operator *op);
    OP_ERROR cookMySop(OP_Context &context);

    THREADED_METHOD1(
            SOP_Gpattern,
            parms->numtiles > UT_Thread::getNumProcessors(),
            cookTiles,
            ThreadParms *, parms);


    void cookTilesPartial(ThreadParms *parms, const UT_JobInfo &info);
    virtual bool updateParmsFlags();

private:
    void pointRelativeToBbox(const UT_Vector3 &pt, UT_Vector2 &uv);
    void computePatternGeoAttibs();
    int PRMUseUVs(){return evalInt("use_uv_attr", 0, 0);}
    void PRMNumTiles(fpreal t, UT_Vector2 &tiles){evalFloats("tiles", tiles.data(), t);}
    float PRMScale(fpreal t){return evalFloat("scale", 0, t);}


    ThreadParms parms;
    GU_Detail *pattern_geo;
    UT_Vector3F bbox_min, bbox_max;
};
#endif
