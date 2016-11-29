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


// Child vray_procedural

class arVray_tile : public VRAY_Procedural
{
public:
	arVray_tile(const GU_Detail *pattern_geo, 
				const GEO_Primitive *template_prim,
				const uint tile_number,
				const fpreal utiles,
				const fpreal vtiles,
				const fpreal scale_compensate);
	virtual ~arVray_tile();

	virtual int initialize(const UT_BoundingBox *);
	virtual void getBoundingBox(UT_BoundingBox &box);
	virtual const char* getClassName(){ return "arVray_tile"; }
	virtual void render();
private:
	UT_BoundingBox myBox;
	const GU_Detail *pattern_geo;
	const GEO_Primitive *template_prim;
	const uint tile_number;
	const fpreal utiles, vtiles;
	const fpreal scale_compensate;
};
