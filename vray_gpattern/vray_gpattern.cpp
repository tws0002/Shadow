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
#include <UT/UT_DSOVersion.h>
#include <VRAY/VRAY_IO.h>
#include <GU/GU_Detail.h>
#include "vray_gpattern.h"
#include "vray_tile_proc.h"
#include <ctime>


static VRAY_ProceduralArg       theArgs[] = {
	VRAY_ProceduralArg("tilegeo", "string", ""),
	VRAY_ProceduralArg("utiles", "real", "1.0"),
	VRAY_ProceduralArg("vtiles", "real", "1.0"),
	VRAY_ProceduralArg("bbox_expand", "real", "1.0"),
	VRAY_ProceduralArg("scale", "real", "1"),
	VRAY_ProceduralArg()
};

const VRAY_ProceduralArg *
getProceduralArgs(const char *)
{
    return theArgs;
}

VRAY_Procedural *
allocProcedural(const char *)
{
    return new arVray_gpattern();
}

arVray_gpattern::arVray_gpattern():
pattern_geo(NULL),
template_geo(NULL),
template_prim(NULL)
{
	mybbox.initBounds(0, 0, 0);
	vtiles = 1;
	utiles = 1;
	scale_compensate = 1;
}

arVray_gpattern::~arVray_gpattern(){

}

const char *
arVray_gpattern::getClassName(){
	return "arVray_gpattern";
}

void
arVray_gpattern::getBoundingBox(UT_BoundingBox &box)
{
    box = mybbox;
}

int
arVray_gpattern::initialize(const UT_BoundingBox *)
{
	UT_String tmp;
	import("utiles", tmp);
	utiles = tmp.toFloat();
	import("vtiles", tmp);
	vtiles = tmp.toFloat();
	import("scale", tmp);
	scale_compensate = tmp.toFloat();
	import("bbox_expand", tmp);
	tile_bbox_expand = tmp.toFloat();

    numtiles = SYSmax(1, int(SYSceil(utiles) * SYSceil(vtiles)));
	// Actually we don't need bbox for this procedural, since we generate child procedurals
	mybbox.expandBounds(0, 50);

	VRAY_ObjectHandle handle = 0;
	handle = queryObject(0);
	template_geo = const_cast<GU_Detail *>(queryGeometry(handle, 0));
	if (template_geo->getNumPrimitives() == 0)
	{
		VRAYerror("Empty template geometry");
		return 0;
	}
	template_prim = template_geo->getGEOPrimitive(template_geo->primitiveOffset(0));
	return 1;
}

// computes u,v based on number of tiles in U direction and current tile number
inline void
computeUVFromTileNumber(fpreal &u, fpreal &v, const fpreal utiles, const fpreal vtiles, const uint tile_number)
{
	uint u_tiles = SYSceil(utiles);
	fpreal du = 0, dv = 0;
    if (tile_number == 0)
        du, dv = 0;
    else if (tile_number > 0 && tile_number < u_tiles)
        du += tile_number;
    else
    {
        du += tile_number % u_tiles;
        dv += SYSfloor(fpreal(tile_number/u_tiles));
    }

	du += 0.5;
	dv += 0.5;
	u = SYSfit((fpreal32)du, (fpreal32)0.0, (fpreal32)utiles, (fpreal32)0.0, (fpreal32)0.99999);
	v = SYSfit((fpreal32)dv, (fpreal32)0.0, (fpreal32)vtiles, (fpreal32)0.0, (fpreal32)0.99999);
}

void
arVray_gpattern::computeTilesCenters()
{
	for (uint i = 0; i < numtiles; i++)
	{
		UT_Vector4F primP;
		fpreal u, v;
		computeUVFromTileNumber(u, v, utiles, vtiles, i);
		template_prim->evaluateInteriorPoint(primP, u, v);
		tiles_centers.append(primP);
	}

}

void
arVray_gpattern::computePatternGeoAttribs()
{
	UT_BoundingBox bbox;
	pattern_geo->getBBox(&bbox);
	UT_Vector3 bbox_min = bbox.minvec();
	UT_Vector3 bbox_max = bbox.maxvec();
	GA_Offset ptoff;
	GA_ROHandleV3 ph = GA_ROHandleV3(pattern_geo->getP());
	auto bbox_uv_hdl = GA_RWHandleV2(pattern_geo->addFloatTuple(GA_ATTRIB_POINT, "bboxuv", 2));
	auto point_dist_hdl = GA_RWHandleR(pattern_geo->addFloatTuple(GA_ATTRIB_POINT, "point_dist", 1));
	UT_Vector2 bbox_uv;
	GA_FOR_ALL_PTOFF(pattern_geo, ptoff)
	{
		const UT_Vector3 ppos = ph.get(ptoff);
		// Point coordinates on XZ palne relative to bounding box
		bbox_uv[0] = SYSfit(ppos[0], bbox_min[0], bbox_max[0], 0, 1);
		bbox_uv[1] = SYSfit(ppos[1], bbox_min[1], bbox_max[1], 0, 1);

		bbox_uv_hdl.set(ptoff, bbox_uv);
		UT_Vector3 pos_on_plane = ppos;
		pos_on_plane[2] = 0; // Position on plane XZ
		fpreal dist = (ppos - pos_on_plane).length(); //Distance from point to plane
		point_dist_hdl.set(ptoff, dist);
	}
}

void
arVray_gpattern::render()
{
	//VRAYprintf(1, "I'm in render()");
	//clock_t begin = clock();
	UT_String tmp = "";
	VRAY_ObjectHandle handle;
	import("tilegeo", tmp);
	handle = queryObject((const char*)tmp);
	if (!handle)
	{
		VRAYerror("Cant find pattern geometry: %s", tmp);
		return;
	}

	pattern_geo = (GU_Detail *)queryGeometry(handle, 0);
	if (!pattern_geo->getNumPrimitives())
	{
		VRAYerror("Empty pattern geometry");
		return;
	}
	// Compute pattern attributes
	computePatternGeoAttribs();
	// Compute tiles centers, used to initialize bounding boxes of child procedurals
	computeTilesCenters();
	for (uint tile_idx = 0; tile_idx < numtiles; tile_idx++)
	{
		arVray_tile *child_proc = new arVray_tile(const_cast<const GU_Detail*>(pattern_geo),
												  const_cast<const GEO_Primitive*>(template_prim),
												  tile_idx,
												  utiles,
												  vtiles,
												  scale_compensate);
		UT_BoundingBoxF child_bbox;
		child_bbox.initBounds(tiles_centers[tile_idx]);
		child_bbox.expandBounds(0, tile_bbox_expand);
		child_proc->initialize(&child_bbox);
		openProceduralObject();
		addProcedural(child_proc);
		closeObject();
	}
    //clock_t end = clock();
	//VRAYprintf(1, "Time: %f", (double(end - begin) / CLOCKS_PER_SEC));

}
