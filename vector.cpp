/*
 *  Mrtp: A simple raytracing tool.
 *  Copyright (C) 2017  Mikolaj Feliks <mikolaj.feliks@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "vector.hpp"


Vector::~Vector () {
}

Vector::Vector () {
    x_ = 0.0;
    y_ = 0.0;
    z_ = 0.0;
}

Vector::Vector (double x,
        double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
}

void Vector::Set (double x,
        double y, double z) {
    x_ = x;
    y_ = y;
    z_ = z;
}

double Vector::Len () {
    return sqrt (x_ * x_ + y_ * y_ + z_ * z_);
}

double Vector::DotSelf () {
    return (x_ * x_ + y_ * y_ + z_ * z_);
}

void Vector::Scale_InPlace (double scale) {
    x_ *= scale;
    y_ *= scale;
    z_ *= scale;
}

void Vector::Normalize_InPlace () {
    double len, scale;

    len = sqrt (x_ * x_ + y_ * y_ + z_ * z_);
    if (len != 0.0) {
        scale = 1.0 / len;
        x_ *= scale;
        y_ *= scale;
        z_ *= scale;
    }
    else {
        x_ = 0.0;
        y_ = 0.0;
        z_ = 0.0;
    }
}

void Vector::CopyTo (Vector *other) {
    other->x_ = x_;
    other->y_ = y_;
    other->z_ = z_;
}

Vector Vector::operator+ (Vector &other) {
    Vector o;

    o.x_ = x_ + other.x_;
    o.y_ = y_ + other.y_;
    o.z_ = z_ + other.z_;
    return o;
}

Vector Vector::operator- (Vector &other) {
    Vector o;

    o.x_ = x_ - other.x_;
    o.y_ = y_ - other.y_;
    o.z_ = z_ - other.z_;
    return o;
}

Vector Vector::operator^ (Vector &other) {
    Vector o;

    o.x_ = y_ * other.z_ - z_ * other.y_;
    o.y_ = z_ * other.x_ - x_ * other.z_;
    o.z_ = x_ * other.y_ - y_ * other.x_;
    return o;
}

double Vector::operator* (Vector &other) {
    return (x_ * other.x_ + y_ * other.y_ + z_ * other.z_);
}

Vector Vector::operator* (double scale) {
    Vector o;

    o.x_ = x_ * scale;
    o.y_ = y_ * scale;
    o.z_ = z_ * scale;
    return o;
}
