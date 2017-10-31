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
#include "actors.hpp"

static double SolveQuadratic (double a, double b, double c, 
    double mint, double maxt);


/*****************************
 *          Planes           *
 *****************************/
Plane::Plane () {
    next_ = NULL;
}

Plane::~Plane () {
}

Plane::Plane (Vector *center, Vector *normal, double scale, 
        Color *color, Texture *texture) {
    center->CopyTo (&center_);
    texture_ = texture;
    scale_   = scale;
    next_    = NULL;

    normal->CopyTo (&normal_);
    normal_.Normalize_InPlace ();
    /*
     * Prepare texturing.
     *
     */
    if (texture_ == NULL) {
        color->CopyTo (&color_);
    }
    else {
        Vector T;
        normal_.GenerateUnitVector (&T);
        
        texturex_ = T ^ normal_;
        texturex_.Normalize_InPlace ();
        
        texturey_ = normal_ ^ texturex_;
        texturey_.Normalize_InPlace ();
    }
}

void Plane::DetermineColor (Vector *hit, Color *color) {
    Vector  V;
    double  vx, vy;
    Color  *cp;

    if (texture_ == NULL) {
        cp = &color_;
    }
    else {
        V = (*hit) - center_;
        /*
         * Calculate components of V (dot products).
         * 
         */
        vx = V * texturex_;
        vy = V * texturey_;
        cp = texture_->GetColor (vx, vy, scale_);
    }
    cp->CopyTo (color);
}

double Plane::Solve (Vector *origin, Vector *direction, 
        double mind, double maxd) {
    double bar, d = -1.0f;
    Vector T;

    bar = (*direction) * normal_;
    if (bar != 0.0f) {
        T = (*origin) - center_;
        d = -(T * normal_) / bar;
        if ((d < mind) || (d > maxd)) {
            d = -1.0f;
        }
    }
    return d;
}

void Plane::GetNormal (Vector *normal) {
    normal_.CopyTo (normal);
}

Plane *Plane::Next () {
    return next_;
}

void Plane::SetNext (Plane *plane) {
    next_ = plane;
}


/*****************************
 *          Spheres          *
 *****************************/
Sphere::Sphere () {
    next_ = NULL;
}

Sphere::~Sphere () {
}

Sphere::Sphere (Vector *center, double radius, Vector *axis, 
        Color *color, Texture *texture) {
    center->CopyTo (&center_);
    radius_  = radius;
    texture_ = texture;
    next_    = NULL;
    /*
     * Prepare texturing.
     *
     */
    if (texture_ == NULL) {
        color->CopyTo (&color_);
    }
    else {
        axis->CopyTo (&texturey_);
        texturey_.Normalize_InPlace ();
    
        Vector T;
        texturey_.GenerateUnitVector (&T);
        texturex_ = T ^ texturey_;
        texturex_.Normalize_InPlace ();
    
        texturez_ = texturey_ ^ texturex_;
        texturez_.Normalize_InPlace ();
    }
}

double Sphere::Solve (Vector *origin, Vector *direction, 
        double mind, double maxd) {
    Vector T;
    T = (*origin) - center_;

    double a, b, c;
    a  = (*direction) * (*direction);
    b  = 2.0f * (*direction * T);
    c  = (T * T) - (radius_ * radius_);

    return SolveQuadratic (a, b, c, mind, maxd);
}

void Sphere::GetNormal (Vector *hit, Vector *normal) {
    (*normal) = (*hit) - center_;
    normal->Normalize_InPlace ();
}

void Sphere::DetermineColor (Vector *normal, Color *color) {
    Color  *cp;
    double  phi, theta, dot, 
        fracx, fracy;

    if (texture_ == NULL) {
        cp = &color_;
    }
    else {
        /*
         * Guidelines from:
         * https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html
         *
         */
        dot   = texturey_ * (*normal);
        phi   = acos (-dot);
        fracy = phi / M_PI;
        
        dot   = (*normal) * texturex_;
        theta = acos (dot / sin (phi)) / (2.0f * M_PI);
        dot   = texturez_ * (*normal);
        if (dot > 0.0f) {
            fracx = theta;
        }
        else {
            fracx = 1.0f - theta;
        }
        cp = texture_->GetColor (fracx, fracy, 1.0f);
    }
    cp->CopyTo (color);
}

Sphere *Sphere::Next () {
    return next_;
}

void Sphere::SetNext (Sphere *sphere) {
    next_ = sphere;
}


/*****************************
 *         Cylinders         *
 *****************************/
Cylinder::Cylinder () {
    next_ = NULL;
}

Cylinder::~Cylinder () {
}

Cylinder::Cylinder (Vector *center, Vector *direction, 
        double radius, double span, Color *color, Texture *texture) {
    /*
     * Radius, origin, etc.
     */
    center->CopyTo (&A_);
    radius_  = radius;
    span_    = span;
    texture_ = texture;
    next_    = NULL;

    /*
     * Direction of the cylinder.
     */
    direction->CopyTo (&B_);
    B_.Normalize_InPlace ();

    /*
     * Prepare texturing.
     *
     */
    if (texture_ == NULL) {
        color->CopyTo (&color_);
    }
    else {
        B_.GenerateUnitVector (&texturey_);

        texturex_ = texturey_ ^ B_;
        texturex_.Normalize_InPlace ();
    }
}

double Cylinder::Solve (Vector *O, Vector *D,
        double mind, double maxd) {
    /*
     * Capital letters are vectors.
     *   A       Origin    of cylinder
     *   B       Direction of cylinder
     *   O       Origin    of ray
     *   D       Direction of ray
     *   P       Hit point on cylinder's surface
     *   X       Point on cylinder's axis closest to the hit point
     *   t       Distance between ray's      origin and P
     *   alpha   Distance between cylinder's origin and X
     *
     *  (P - X) . B = 0
     *  |P - X| = R  => (P - X) . (P - X) = R^2
     *
     *  P = O + t * D
     *  X = A + alpha * B
     *  T = O - A
     *  ...
     *  2t * (T.D - alpha * D.B)  +  t^2 - 2 * alpha * T.B  +
     *      +  alpha^2  =  R^2 - T.T
     *  a = T.D
     *  b = D.B
     *  d = T.B
     *  f = R^2 - T.T
     *
     *  t^2 * (1 - b^2)  +  2t * (a - b * d)  -
     *      -  d^2 - f = 0    => t = ...
     *  alpha = d + t * b
     *
     */
    Vector T;
    T = (*O) - A_;

    double a, b, d, f;
    a  = T  * (*D);
    b  = B_ * (*D);
    d  = T  * B_;
    f  = (radius_ * radius_) - (T * T);

    /* 
     * Solving a quadratic equation for t. 
     */
    double aa, bb, cc, t, alpha;
    aa = 1.0f - (b * b);
    bb = 2.0f * (a - b * d);
    cc = -(d * d) - f;

    t = SolveQuadratic (aa, bb, cc, mind, maxd);
    if (t > 0.0f) {
        /*
         * Check if the cylinder is finite.
         */
        if (span_ > 0.0f) {
            alpha = d + t * b;
            if ((alpha < -span_) || (alpha > span_)) {
                return -1.0f;
            }
        }
    }
    return t;
}

void Cylinder::GetNormal (Vector *hit, Vector *normal) {
    /*
     * N = Hit - [B . (Hit - A)] * B
     */
    Vector T, Q;
    double alpha;

    T = (*hit) - A_;
    alpha = B_ * T;

    T = B_ * alpha;
    Q = A_ + T;
    (*normal) = (*hit) - Q;
    normal->Normalize_InPlace ();
}

void Cylinder::DetermineColor (Vector *hit, Vector *normal, 
        Color *color) {
    Color  *cp;

    if (texture_ == NULL) {
        cp = &color_;
    }
    else {
        double  dot, fracx, fracy, alpha;
        Vector  T;

        T = (*hit) - A_;
        alpha = B_ * T;
        dot   = texturex_ * (*normal);
        fracx = acos (dot) / M_PI;
        fracy = alpha / (2.0f * M_PI * radius_);
        cp    = texture_->GetColor (fracx, fracy, 1.0f);
    }
    cp->CopyTo (color);
}

Cylinder *Cylinder::Next () {
    return next_;
}

void Cylinder::SetNext (Cylinder *cylinder) {
    next_ = cylinder;
}


/*****************************
 *           Light           *
 *****************************/
Light::Light (Vector *origin) {
    origin->CopyTo (&position_);
}

Light::~Light () {
}

void Light::GetToLight (Vector *hit, Vector *tolight) {
    Vector T = position_ - (*hit);
    T.CopyTo (tolight);
}


/*****************************
 *     Utility functions     *
 *****************************/
double SolveQuadratic (double a, double b, double c, 
        double mint, double maxt) {
    /*
     * Solve a quadratic equation for t.
     *
     * Since t is a scale in: P = O + t*D, return
     * only the smaller t and within the limits of (mint, maxt).
     *
     * Otherwise return -1.
     */
    double delta, sqdelta, ta, tb, t;

    delta = b * b - 4.0f * a * c;
    if (delta < 0.0f) {
        t = -1.0f;
    }
    else {
        if (delta != 0.0f) {
            sqdelta = sqrt (delta);
            t  = 0.5f / a;
            ta = (-b - sqdelta) * t;
            tb = (-b + sqdelta) * t;
            t  = (ta < tb) ? ta : tb;
        }
        else {
            t = -b / (2.0f * a);
        }
        if ((t < mint) || (t > maxt)) {
            t = -1.0f;
        }
    }
    return t;
}
