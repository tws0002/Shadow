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
#include <VRAY/VRAY_Procedural.h>
#include <GU/GU_Detail.h>
#include <UT/UT_BoundingBox.h>


class arVray_gpattern : public VRAY_Procedural
{
public:
	arVray_gpattern();
	virtual ~arVray_gpattern();

	virtual int initialize(const UT_BoundingBox *bbox);
	virtual void getBoundingBox(UT_BoundingBox &box);
	virtual const char *getClassName();
	virtual void render();
	void computePatternGeoAttribs();
	void computeTilesCenters();

private:
	GU_Detail *pattern_geo;
	GU_Detail *template_geo;
	GEO_Primitive *template_prim;
	UT_BoundingBox mybbox;
	UT_Vector4FArray tiles_centers; // Array with positions of tiles on template surface

	uint numtiles;
	fpreal utiles;
	fpreal vtiles;
	fpreal tile_bbox_expand;
	fpreal scale_compensate;
};
