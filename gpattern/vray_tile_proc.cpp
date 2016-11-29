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
#include "vray_tile_proc.h"
#include <VRAY/VRAY_IO.h>

arVray_tile::arVray_tile(const GU_Detail *pattern_geo, 
						const GEO_Primitive *template_prim,
						const uint tile_number,
						const fpreal utiles,
						const fpreal vtiles,
						const fpreal scale)
:template_prim(template_prim),
pattern_geo(pattern_geo),
tile_number(tile_number),
utiles(utiles),
vtiles(vtiles),
scale_compensate(scale)
{
}

arVray_tile::~arVray_tile()
{
}

int
arVray_tile::initialize(const UT_BoundingBox *bbox)
{
	myBox = *bbox;
	return 1;
}

void
arVray_tile::getBoundingBox(UT_BoundingBox &box)
{
	box = myBox;
}

void
arVray_tile::render()
{
	//VRAYprintf(1, "I'm in render() of arVray_tile_%d", tile_number);
	int u_tiles = SYSceil(utiles);
    float du = 0;
    float dv = 0;
    float ru, rv;
    fpreal32 s, t;

    if (tile_number == 0)
        du, dv = 0;
    else if (tile_number > 0 && tile_number < u_tiles)
        du += tile_number;
    else
    {
        du += tile_number % u_tiles;
        dv += SYSfloor(static_cast<fpreal>(tile_number/u_tiles));
    }
	GU_Detail *pattern_copy = allocateGeometry();
	pattern_copy->copy(*pattern_geo);
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

        s = SYSfit(ru, (fpreal32)0.0, (fpreal32)utiles, (fpreal32)0.0, (fpreal32)0.99999);
        t = SYSfit(rv, (fpreal32)0.0, (fpreal32)vtiles, (fpreal32)0.0, (fpreal32)0.99999);
        template_prim->evaluateInteriorPoint(primP, s, t);
        template_prim->evaluateNormalVector(primN, s, t);
        primN.normalize();
        primP = primP + primN * point_dist_hdl.get(*it) * scale_compensate;
        ph.set(*it, primP);
    }
	pattern_copy->normal();
	openGeometryObject();
	addGeometry(pattern_copy, 0);
	closeObject();
}
