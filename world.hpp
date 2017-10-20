/*
 *  Mrtp: A simple raytracing tool.
 *  Copyright (C) 2017  Mikolaj Feliks <mikolaj.feliks@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _WORLD_H
#define _WORLD_H

#include <cstddef>  /* NULL pointer. */
#include <cmath>
#include <string>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif /* _OPENMP */

#include "vector.hpp"
#include "actors.hpp"
#include "camera.hpp"
#include "parser.hpp"
#include "buffer.hpp"
#include "color.hpp"
#include "texture.hpp"

enum HitCode_t {hitNull, hitPlane, hitSphere, 
    hitCylinder};

enum LightModel_t {lightNone, lightLinear, lightQuadratic};


class World {
    Buffer  *buffer_;
    Camera  *camera_;
    Light   *light_;

    /*
     * Model to quench light with increasing
     * distance.
     *
     * At maxdist, the light is fully quenched,
     * unless the model is "none". 
     */
    LightModel_t model_;
    double  maxdist_;

    /*
     * Shadow factor between <0..1>, defines how
     * "deep" shadows are.
     */
    double  shadow_;

    /*
     * Maximum distance reached by rays.
     */
    double  cutoff_;

    /*
     * Planes, spheres and cylinders are one-way
     * linked lists.
     */
    Plane        *planes_;
    unsigned int  nplanes_;

    Sphere       *spheres_;
    unsigned int  nspheres_;

    Cylinder     *cylinders_;
    unsigned int  ncylinders_;

    Texture      *textures_;
    unsigned int  ntextures_;

    Parser    *parser_;
    double     fov_;
    unsigned int width_, height_, nthreads_;

public:
    World (Parser *parser, unsigned int width,
        unsigned int height, double fov,
        double distance, double shadowfactor, 
        LightModel_t lightmodel, unsigned int nthreads);
    ~World ();
    void Initialize ();

    /*
     * Manipulation of objects, adding, removing, etc.
     */
    void AddPlane (Vector *center, Vector *normal, double texscale, 
        Color *color, Texture *texture);
    void AddSphere (Vector *center, double radius, Vector *axis, 
        Color *color, Texture *texture);
    void AddCylinder (Vector *center, Vector *direction, double radius, 
        double span, Color *color, Texture *texture);
    Texture *AddTexture (std::string *filename);

    unsigned int PopPlane ();
    unsigned int PopSphere ();
    unsigned int PopCylinder ();
    unsigned int PopTexture ();
    /*
     * Rendering, writing output, etc.
     *
     */
    void TraceRay (Vector *origin, Vector *direction,
        Color *color);
    void RenderBlock (Vector *vw, Vector *vh, Vector *vo, Vector *eye,
            unsigned int block, unsigned int nlines);
    void Render ();
    void WritePNG (std::string *filename);
};

#endif /* _WORLD_H */
