/*****
 * triple.h
 * John Bowman
 *
 * Stores a three-dimensional point.
 *
 *****/

#ifndef TRIPLE_H
#define TRIPLE_H

#include <cassert>
#include <iostream>
#include <cmath>
#include <cstring>

#include "common.h"
#include "angle.h"
#include "pair.h"

namespace camp {

typedef double Triple[3];
  
class triple;
  
bool isIdTransform3(const double* t);
void copyTransform3(double*& d, const double* s);
void multiplyTransform3(double*& t, const double* s, const double* r);

void copyTriples(size_t n, Triple* d, const Triple* s);
void transformTriples(const double* t, size_t n, Triple* d, const Triple* s);
void transformshiftlessTriples(const double* t, size_t n, Triple* d, const Triple* s);
void transformNormalsTriples(const double* t, size_t n, Triple* d, const Triple* s);
void unitTriples(size_t n, Triple* d);

void boundsTriples(double& x, double& y, double& z, double& X, double& Y, double& Z, size_t n, const Triple* v);

void ratioTriples(pair &b, double (*m)(double, double), bool &first, size_t n, const Triple* v);

void normalizeTriple(Triple v);
void crossTriple(Triple n, const Triple u, const Triple v);

class triple : virtual public gc {
  double x;
  double y;
  double z;

public:
  triple() : x(0.0), y(0.0), z(0.0) {}
  triple(double x, double y=0.0, double z=0.0) : x(x), y(y), z(z) {}
  triple(const Triple& v) : x(v[0]), y(v[1]), z(v[2]) {}

  virtual ~triple() {}
  
  void set(double X, double Y=0.0, double Z=0.0) { x=X; y=Y; z=Z; }

  double getx() const { return x; }
  double gety() const { return y; }
  double getz() const { return z; }

  // transform by column-major matrix
  friend triple operator* (const double* t, const triple& v)
  {
    if(t == NULL)
      return v;

    double f=t[3]*v.x+t[7]*v.y+t[11]*v.z+t[15];
    if(f == 0.0)
      reportError("division by 0 in transform of a triple");
    f=1.0/f;
    
    return triple((t[0]*v.x+t[4]*v.y+t[8]*v.z+t[12])*f,
                  (t[1]*v.x+t[5]*v.y+t[9]*v.z+t[13])*f,
                  (t[2]*v.x+t[6]*v.y+t[10]*v.z+t[14])*f);
  }
  
  friend triple multshiftless(const double* t, const triple& v)
  {
    if(t == NULL)
      return v;

    double f=t[3]*v.x+t[7]*v.y+t[11]*v.z+t[15];
    if(f == 0.0)
      reportError("division by 0 in shiftless transform of a triple");
    f=1.0/f;
    
    return triple((t[0]*v.x+t[4]*v.y+t[8]*v.z)*f,
                  (t[1]*v.x+t[5]*v.y+t[9]*v.z)*f,
                  (t[2]*v.x+t[6]*v.y+t[10]*v.z)*f);
  }

  friend void transformtriples(const double* t, size_t n, triple* d,
                               const triple* s)
  {
    if(n == 0 || d == NULL || s == NULL)
      return;

    if(isIdTransform3(t)) {
      copytriples(n, d, s);
      return;
    }
    
    for(size_t i=0; i < n; i++) {
      const double& x = s[i].x;
      const double& y = s[i].y;
      const double& z = s[i].z;
      double f=t[3]*x+t[7]*y+t[11]*z+t[15];
      if(f == 0.0)
        reportError("division by 0 in transformtriples");
      f=1.0/f;
      d[i].x=(t[0]*x+t[4]*y+t[8]*z+t[12])*f;      
      d[i].y=(t[1]*x+t[5]*y+t[9]*z+t[13])*f;      
      d[i].z=(t[2]*x+t[6]*y+t[10]*z+t[14])*f;      
    }
  }
  
  friend void copytriples(size_t n, triple* d, const triple* s)
  {
    if(d == NULL || s == NULL)
      return;
    
    for(size_t i=0; i < n; i++) d[i]=s[i];
  }

  friend void boundstriples(triple& Min, triple& Max, size_t n, const triple* v)
  {
    if(n==0 || v==NULL)
      return;

    double x,y,z;
    double X,Y,Z;
    
    X=x=v[0].getx();
    Y=y=v[0].gety();
    Z=z=v[0].getz();
    for(size_t i=1; i < n; ++i) {
      const double vx=v[i].getx();
      x=fmin(x,vx); X=fmax(X,vx);
      const double vy=v[i].gety();
      y=fmin(y,vy); Y=fmax(Y,vy);
      const double vz=v[i].getz();
      z=fmin(z,vz); Z=fmax(Z,vz);
    }
    
    Min.set(x,y,z);
    Max.set(X,Y,Z);
  }

  friend void ratiotriples(pair &b, double (*m)(double, double), bool &first,
                           size_t n, const triple* v)
  {
    if(n==0 || v==NULL)
      return;

    if(first) {
      first=false;
      const triple& v0=v[0];
      b=pair(v0.x/v0.z,v0.y/v0.z);
    }

    double x=b.getx();
    double y=b.gety();
    for(size_t i=0; i < n; ++i) {
      const triple& vi = v[i];
      x=m(x,vi.x/vi.z);
      y=m(y,vi.y/vi.z);
    }
    b=pair(x,y);
  }
  
  friend triple operator+ (const triple& z, const triple& w)
  {
    return triple(z.x + w.x, z.y + w.y, z.z + w.z);
  }

  friend triple operator- (const triple& z, const triple& w)
  {
    return triple(z.x - w.x, z.y - w.y, z.z - w.z);
  }

  friend triple operator- (const triple& z)
  {
    return triple(-z.x, -z.y, -z.z);
  }

  friend triple operator* (double s, const triple& z)
  {
    return triple(s*z.x, s*z.y, s*z.z);
  }

  friend triple operator* (const triple& z, double s)
  {
    return triple(z.x*s, z.y*s, z.z*s);
  }

  friend triple operator/ (const triple& z, double s)
  {
    if (s == 0.0)
      reportError("division by 0");
    s=1.0/s;
    return triple(z.x*s, z.y*s, z.z*s);
  }

  const triple& operator+= (const triple& w)
  {
    x += w.x;
    y += w.y;
    z += w.z;
    return *this;
  }

  const triple& operator-= (const triple& w)
  {
    x -= w.x;
    y -= w.y;
    z -= w.z;
    return *this;
  }

  friend bool operator== (const triple& z, const triple& w)
  {
    return z.x == w.x && z.y == w.y && z.z == w.z;
  }

  friend bool operator!= (const triple& z, const triple& w)
  {
    return z.x != w.x || z.y != w.y || z.z != w.z;
  }

  double abs2() const
  {
    return x*x+y*y+z*z;
  }
  
  double length() const /* r */
  {
    return sqrt(abs2());
  }
  
  friend double length(const triple& v)
  {
    return v.length();
  }

  double polar() const /* theta */
  {
    double r=length();
    if (r == 0.0)
      reportError("taking polar angle of (0,0,0)");
    return acos(z/r);
  }
  
  double azimuth() const /* phi */
  {
    return angle(x,y);
  }
  
  friend triple unit(const triple& v)
  {
    double scale=v.length();
    if(scale != 0.0) scale=1.0/scale;
    return triple(v.x*scale,v.y*scale,v.z*scale);
  }
  
  friend double dot(const triple& u, const triple& v)
  {
    return u.x*v.x+u.y*v.y+u.z*v.z;
  }

  friend triple cross(const triple& u, const triple& v) 
  {
    return triple(u.y*v.z-u.z*v.y,
                  u.z*v.x-u.x*v.z,
                  u.x*v.y-v.x*u.y);
  }

  // Returns a unit triple in the direction (theta,phi), in radians.
  friend triple expi(double theta, double phi)
  {
    double sintheta=sin(theta);
    return triple(sintheta*cos(phi),sintheta*sin(phi),cos(theta));
  }
  
  friend istream& operator >> (istream& s, triple& z)
  {
    char c;
    s >> std::ws;
    bool paren=s.peek() == '('; // parenthesis are optional
    if(paren) s >> c;
    s >> z.x >> std::ws;
    if(s.peek() == ',') s >> c >> z.y;
    else z.y=0.0;
    if(s.peek() == ',') s >> c >> z.z;
    else z.z=0.0;
    if(paren) {
      s >> std::ws;
      if(s.peek() == ')') s >> c;
    }
    
    return s;
  }

  friend ostream& operator << (ostream& out, const triple& z)
  {
    out << "(" << z.x << "," << z.y << "," << z.z << ")";
    return out;
  }
  
};

triple expi(double theta, double phi);
  
// Return the component of vector v perpendicular to a unit vector u.
inline triple perp(triple v, triple u)
{
  return v-dot(v,u)*u;
}

double xratio(const triple& v);
double yratio(const triple& v);

} //namespace camp

GC_DECLARE_PTRFREE(camp::triple);

#endif
