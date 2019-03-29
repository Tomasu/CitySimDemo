//
// Created by moose on 29/03/19.
//

#ifndef CITYSIM_GEOMETRYBUILDER_H
#define CITYSIM_GEOMETRYBUILDER_H

// was going to have a fancy geometry builder class
// that could take commands to build various types of geometry and build a vertex buffer
//  I think it might be better to have a way to /build/ and /modify/ Qt3D Geometry/GeometryRenderer
//  objects rather than a bare vertex buffer builder like this was going to be...
class GeometryBuilder
{
	public:
		GeometryBuilder();
		~GeometryBuilder();


};


#endif //CITYSIM_GEOMETRYBUILDER_H
