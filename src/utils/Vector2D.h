#pragma once

class PolarVector2D;

class Vector2D {
    public:
    Vector2D() {
    }
    Vector2D( double vX, double vY ) {
        x = vX;
        y = vY;
    }
    Vector2D( PolarVector2D pVec );
    ~Vector2D() {
    }

    double x;
    double y;

    double dist2D() {
        return sqrt( x * x + y * y );
    }

    double dist2D( const Vector2D & v ) {
        double dx = v.x - x;
        double dy = v.y - y;
        return sqrt( dx * dx + dy * dy );
    }

    void scale( double mul ) {
        x *= mul;
        y *= mul;
    }

    Vector2D balance( const Vector2D & v, double inertia ) {
        Vector2D ret;
        ret.x = x * ( 1.0 - inertia ) + v.x * inertia;
        ret.y = y * ( 1.0 - inertia ) + v.y * inertia;
        return ret;
    }

};

class PolarVector2D {
    public:
    PolarVector2D() {
    }
    PolarVector2D( Vector2D vector );
    ~PolarVector2D() {
    }

    double r;
    double a;

    void scale( double mul ) {
        r *= mul;
    }
};

Vector2D::Vector2D( PolarVector2D pVec ) {
    x = - sin( pVec.a ) * pVec.r;
    y = cos( pVec.a ) * pVec.r;
}

PolarVector2D::PolarVector2D( Vector2D vec ) {
    a = atan2( - vec.x, vec.y );
    r = sqrt( vec.x * vec.x + vec.y * vec.y );
}

