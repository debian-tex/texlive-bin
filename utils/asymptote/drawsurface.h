/*****
 * drawsurface.h
 *
 * Stores a surface that has been added to a picture.
 *****/

#ifndef DRAWSURFACE_H
#define DRAWSURFACE_H

#include "drawelement.h"
#include "triple.h"
#include "arrayop.h"
#include "path3.h"

namespace camp {

#ifdef HAVE_GL
void storecolor(GLfloat *colors, int i, const vm::array &pens, int j);
#endif  

class drawSurface : public drawElement {
protected:
  Triple *controls;
  Triple vertices[4];
  triple center;
  bool straight;
  RGBAColour diffuse;
  RGBAColour ambient;
  RGBAColour emissive;
  RGBAColour specular;
  RGBAColour *colors;
  double opacity;
  double shininess;
  double PRCshininess;
  triple normal;
  bool invisible;
  Interaction interaction;
  
  triple Min,Max;
  bool prc;
  
#ifdef HAVE_GL
  triple d; // Maximum deviation of surface from a quadrilateral.
  triple dperp;
#endif  
  
public:
  drawSurface(const vm::array& g, triple center, bool straight,
              const vm::array&p, double opacity, double shininess,
              double PRCshininess, triple normal, const vm::array &pens,
              Interaction interaction, bool prc) :
    center(center), straight(straight), opacity(opacity), shininess(shininess),
    PRCshininess(PRCshininess), normal(unit(normal)),
    interaction(interaction), prc(prc) {
    const string wrongsize=
      "Bezier surface patch requires 4x4 array of triples and array of 4 pens";
    if(checkArray(&g) != 4 || checkArray(&p) != 4)
      reportError(wrongsize);
    
    bool havenormal=normal != zero;
  
    vm::array *g0=vm::read<vm::array*>(g,0);
    vm::array *g3=vm::read<vm::array*>(g,3);
    if(checkArray(g0) != 4 || checkArray(g3) != 4)
      reportError(wrongsize);
    store(vertices[0],vm::read<triple>(g0,0));
    store(vertices[1],vm::read<triple>(g0,3));
    store(vertices[2],vm::read<triple>(g3,0));
    store(vertices[3],vm::read<triple>(g3,3));
    
    if(!havenormal || !straight) {
      size_t k=0;
      controls=new(UseGC) Triple[16];
      for(size_t i=0; i < 4; ++i) {
        vm::array *gi=vm::read<vm::array*>(g,i);
        if(checkArray(gi) != 4) 
          reportError(wrongsize);
        for(size_t j=0; j < 4; ++j)
          store(controls[k++],vm::read<triple>(gi,j));
      }
    } else controls=NULL;
    
    pen surfacepen=vm::read<camp::pen>(p,0);
    invisible=surfacepen.invisible();
    
    diffuse=rgba(surfacepen);
    ambient=rgba(vm::read<camp::pen>(p,1));
    emissive=rgba(vm::read<camp::pen>(p,2));
    specular=rgba(vm::read<camp::pen>(p,3));
    
    int size=checkArray(&pens);
    if(size > 0) {
      if(size != 4) reportError(wrongsize);
      colors=new(UseGC) RGBAColour[4];
      colors[0]=rgba(vm::read<camp::pen>(pens,0));
      colors[1]=rgba(vm::read<camp::pen>(pens,3));
      colors[2]=rgba(vm::read<camp::pen>(pens,1));
      colors[3]=rgba(vm::read<camp::pen>(pens,2));
    } else colors=NULL;
  }
  
  drawSurface(const double* t, const drawSurface *s) :
    straight(s->straight), diffuse(s->diffuse), ambient(s->ambient),
    emissive(s->emissive), specular(s->specular), opacity(s->opacity),
    shininess(s->shininess), PRCshininess(s->PRCshininess), 
    invisible(s->invisible),
    interaction(s->interaction), prc(s->prc) { 
    
    transformTriples(t,4,vertices,s->vertices);
    
    if(s->controls) {
      controls=new(UseGC) Triple[16];
      transformTriples(t,16,controls,s->controls);
    } else controls=NULL;
    
#ifdef HAVE_GL
    center=t*s->center;
    normal=multshiftless(t,s->normal);
#endif    
    
    if(s->colors) {
      colors=new(UseGC) RGBAColour[4];
      for(size_t i=0; i < 4; ++i)
        colors[i]=s->colors[i];
    } else colors=NULL;
  }
  
  bool is3D() {return true;}
  
  void bounds(const double* t, bbox3& b);
  
  void ratio(const double* t, pair &b, double (*m)(double, double), double fuzz,
             bool &first);
  
  virtual ~drawSurface() {}

  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  void displacement();
  
  void render(GLUnurbs *nurb, double, const triple& Min, const triple& Max,
              double perspective, bool lighton, bool transparent);
  
  drawElement *transformed(const double* t);
};
  
class drawNurbs : public drawElement {
protected:
  size_t udegree,vdegree;
  size_t nu,nv;
  Triple *controls;
  double *weights;
  double *uknots, *vknots;
  RGBAColour diffuse;
  RGBAColour ambient;
  RGBAColour emissive;
  RGBAColour specular;
  double opacity;
  double shininess;
  double PRCshininess;
  triple normal;
  bool invisible;
  
  triple Min,Max;
  
#ifdef HAVE_GL
  GLfloat *colors;
  GLfloat *Controls;
  GLfloat *uKnots;
  GLfloat *vKnots;
#endif  
  
public:
  drawNurbs(const vm::array& g, const vm::array* uknot, const vm::array* vknot,
            const vm::array* weight, const vm::array&p, double opacity,
            double shininess, double PRCshininess, const vm::array &pens)
            : opacity(opacity), shininess(shininess),
              PRCshininess(PRCshininess) {
    size_t weightsize=checkArray(weight);
    
    const string wrongsize="Inconsistent NURBS data";
    nu=checkArray(&g);
    
    if(nu == 0 || (weightsize != 0 && weightsize != nu) || checkArray(&p) != 4)
      reportError(wrongsize);
    
    vm::array *g0=vm::read<vm::array*>(g,0);
    nv=checkArray(g0);
    
    size_t n=nu*nv;
    controls=new(UseGC) Triple[n];
    
    size_t k=0;
    for(size_t i=0; i < nu; ++i) {
      vm::array *gi=vm::read<vm::array*>(g,i);
      if(checkArray(gi) != nv)  
        reportError(wrongsize);
      for(size_t j=0; j < nv; ++j)
        store(controls[k++],vm::read<triple>(gi,j));
    }
      
    if(weightsize > 0) {
      size_t k=0;
      weights=new(UseGC) double[n];
      for(size_t i=0; i < nu; ++i) {
        vm::array *weighti=vm::read<vm::array*>(weight,i);
        if(checkArray(weighti) != nv)  
          reportError(wrongsize);
        for(size_t j=0; j < nv; ++j)
          weights[k++]=vm::read<double>(weighti,j);
      }
    } else weights=NULL;
      
    size_t nuknots=checkArray(uknot);
    size_t nvknots=checkArray(vknot);
    
    if(nuknots <= nu+1 || nuknots > 2*nu || nvknots <= nv+1 || nvknots > 2*nv)
      reportError(wrongsize);

    udegree=nuknots-nu-1;
    vdegree=nvknots-nv-1;
    
    run::copyArrayC(uknots,uknot,0,UseGC);
    run::copyArrayC(vknots,vknot,0,UseGC);
    
    pen surfacepen=vm::read<camp::pen>(p,0);
    invisible=surfacepen.invisible();
    
    diffuse=rgba(surfacepen);
    ambient=rgba(vm::read<camp::pen>(p,1));
    emissive=rgba(vm::read<camp::pen>(p,2));
    specular=rgba(vm::read<camp::pen>(p,3));
    
#ifdef HAVE_GL
    Controls=NULL;
    int size=checkArray(&pens);
    if(size > 0) {
      colors=new(UseGC) GLfloat[16];
      if(size != 4) reportError(wrongsize);
      storecolor(colors,0,pens,0);
      storecolor(colors,8,pens,1);
      storecolor(colors,12,pens,2);
      storecolor(colors,4,pens,3);
    } else colors=NULL;
#endif  
  }
  
  drawNurbs(const double* t, const drawNurbs *s) :
    udegree(s->udegree), vdegree(s->vdegree), nu(s->nu), nv(s->nv),
    diffuse(s->diffuse), ambient(s->ambient),
    emissive(s->emissive), specular(s->specular), opacity(s->opacity),
    shininess(s->shininess), PRCshininess(s->PRCshininess), 
    invisible(s->invisible) {
    
    const size_t n=nu*nv;
    controls=new(UseGC) Triple[n];
      
    transformTriples(t,n,controls,s->controls);
    
    if(s->weights) {
      weights=new(UseGC) double[n];
      for(size_t i=0; i < n; ++i)
        weights[i]=s->weights[i];
    } else weights=NULL;
    
    size_t nuknots=udegree+nu+1;
    size_t nvknots=vdegree+nv+1;
    uknots=new(UseGC) double[nuknots];
    vknots=new(UseGC) double[nvknots];
    
    for(size_t i=0; i < nuknots; ++i)
      uknots[i]=s->uknots[i];
    
    for(size_t i=0; i < nvknots; ++i)
      vknots[i]=s->vknots[i];
    
#ifdef HAVE_GL
    Controls=NULL;
    if(s->colors) {
      colors=new(UseGC) GLfloat[16];
      for(size_t i=0; i < 16; ++i)
        colors[i]=s->colors[i];
    } else colors=NULL;
#endif    
  }
  
  bool is3D() {return true;}
  
  void bounds(const double* t, bbox3& b);
  
  virtual ~drawNurbs() {}

  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  void displacement();
  void ratio(const double* t, pair &b, double (*m)(double, double), double,
             bool &first);

  void render(GLUnurbs *nurb, double size2, const triple& Min, const triple& Max,
              double perspective, bool lighton, bool transparent);
    
  drawElement *transformed(const double* t);
};
  
// Draw a transformed PRC object.
class drawPRC : public drawElementLC {
protected:
  RGBAColour diffuse;
  RGBAColour ambient;
  RGBAColour emissive;
  RGBAColour specular;
  double opacity;
  double shininess;
  bool invisible;
public:
  drawPRC(const vm::array& t, const vm::array&p, double opacity,
          double shininess) : 
    drawElementLC(t), opacity(opacity), shininess(shininess) {

    string needfourpens="array of 4 pens required";
    if(checkArray(&p) != 4)
      reportError(needfourpens);
    
    pen surfacepen=vm::read<camp::pen>(p,0);
    invisible=surfacepen.invisible();
    
    diffuse=rgba(surfacepen);
    ambient=rgba(vm::read<camp::pen>(p,1));
    emissive=rgba(vm::read<camp::pen>(p,2));
    specular=rgba(vm::read<camp::pen>(p,3));
  }
  
  drawPRC(const double* t, const drawPRC *s) :
    drawElementLC(t, s), diffuse(s->diffuse), ambient(s->ambient),
    emissive(s->emissive), specular(s->specular), opacity(s->opacity),
    shininess(s->shininess), invisible(s->invisible) {
  }
  
  bool write(prcfile *out, unsigned int *, double, groupsmap&) {
    return true;
  }
  virtual void transformedbounds(const double*, bbox3&) {}
  virtual void transformedratio(const double*, pair&, double (*)(double, double),
                                double, bool&) {}

};
  
// Draw a PRC unit sphere.
class drawSphere : public drawPRC {
  bool half;
  int type;
public:
  drawSphere(const vm::array& t, bool half, const vm::array&p, double opacity,
             double shininess, int type) :
    drawPRC(t,p,opacity,shininess), half(half), type(type) {}

  drawSphere(const double* t, const drawSphere *s) :
    drawPRC(t,s), half(s->half), type(s->type) {}
    
  void P(Triple& t, double x, double y, double z);
  
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  drawElement *transformed(const double* t) {
    return new drawSphere(t,this);
  }
};
  
// Draw a PRC unit cylinder.
class drawCylinder : public drawPRC {
public:
  drawCylinder(const vm::array& t, const vm::array&p, double opacity,
               double shininess) :
    drawPRC(t,p,opacity,shininess) {}

  drawCylinder(const double* t, const drawCylinder *s) :
    drawPRC(t,s) {}
    
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  drawElement *transformed(const double* t) {
    return new drawCylinder(t,this);
  }
};
  
// Draw a PRC unit disk.
class drawDisk : public drawPRC {
public:
  drawDisk(const vm::array& t, const vm::array&p, double opacity,
           double shininess) :
    drawPRC(t,p,opacity,shininess) {}

  drawDisk(const double* t, const drawDisk *s) :
    drawPRC(t,s) {}
    
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  drawElement *transformed(const double* t) {
    return new drawDisk(t,this);
  }
};
  
// Draw a PRC tube.
class drawTube : public drawElement {
protected:
  path3 center;
  path3 g;
  RGBAColour diffuse;
  RGBAColour ambient;
  RGBAColour emissive;
  RGBAColour specular;
  double opacity;
  double shininess;
  bool invisible;
public:
  drawTube(path3 center, path3 g, const vm::array&p, double opacity,
           double shininess) : 
    center(center), g(g), opacity(opacity), shininess(shininess) {
    string needfourpens="array of 4 pens required";
    if(checkArray(&p) != 4)
      reportError(needfourpens);
    
    pen surfacepen=vm::read<camp::pen>(p,0);
    invisible=surfacepen.invisible();
    
    diffuse=rgba(surfacepen);
    ambient=rgba(vm::read<camp::pen>(p,1));
    emissive=rgba(vm::read<camp::pen>(p,2));
    specular=rgba(vm::read<camp::pen>(p,3));
  }
  
  drawTube(const double* t, const drawTube *s) :
    center(camp::transformed(t,s->center)), g(camp::transformed(t,s->g)), 
    diffuse(s->diffuse), ambient(s->ambient), emissive(s->emissive),
    specular(s->specular), opacity(s->opacity),
    shininess(s->shininess), invisible(s->invisible) {
  }
  
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
                        
  drawElement *transformed(const double* t) {
    return new drawTube(t,this);
  }
};

// Draw a PRC pixel.
class drawPixel : public drawElement {
  Triple v;
  RGBAColour c;
  double width;
  bool invisible;
public:
  drawPixel(const triple& v0, const pen& p, double width) :
    c(rgba(p)), width(width) {
    store(v,v0);
    invisible=p.invisible();
  }

  drawPixel(const double* t, const drawPixel *s) :
    c(s->c), width(s->width), invisible(s->invisible) {
    transformTriples(t,1,&v,&(s->v));
  }
    
  void bounds(const double* t, bbox3& b) {
    const triple R=0.5*width*triple(1.0,1.0,1.0);
    if (t != NULL) {
      Triple tv;
      transformTriples(t,1,&tv,&v);
      b.add(tv-R);
      b.add(tv+R);
    } else {
      b.add(v-R);
      b.add(v+R);
    }    
  }    
  
  void render(GLUnurbs *nurb, double size2, const triple& Min, const triple& Max,
              double perspective, bool lighton, bool transparent);
  
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  drawElement *transformed(const double* t) {
    return new drawPixel(t,this);
  }
};
  
class drawBaseTriangles : public drawElement {
protected:
  size_t nP;
  Triple* P;
  size_t nN;
  Triple* N;
  size_t nI;
  uint32_t (*PI)[3];
  uint32_t (*NI)[3];
  
  triple Min,Max;

  static const string wrongsize;
  static const string outofrange;
    
public:
  drawBaseTriangles(const vm::array& v, const vm::array& vi,
                    const vm::array& n, const vm::array& ni) {
    nP=checkArray(&v);
    P=new(UseGC) Triple[nP];
    for(size_t i=0; i < nP; ++i)
      store(P[i],vm::read<triple>(v,i));
  
    nI=checkArray(&vi);
    PI=new(UseGC) uint32_t[nI][3];
    for(size_t i=0; i < nI; ++i) {
      vm::array *vii=vm::read<vm::array*>(vi,i);
      if(checkArray(vii) != 3) reportError(wrongsize);
      uint32_t *PIi=PI[i];
      for(size_t j=0; j < 3; ++j) {
        size_t index=unsignedcast(vm::read<Int>(vii,j));
        if(index >= nP) reportError(outofrange);
        PIi[j]=index;
      }
    }
    
    nN=checkArray(&n);
    if(nN) {
      N=new(UseGC) Triple[nN];
      for(size_t i=0; i < nN; ++i)
        store(N[i],vm::read<triple>(n,i));
    
      if(checkArray(&ni) != nI)
        reportError("Index arrays have different lengths");
      NI=new(UseGC) uint32_t[nI][3];
      for(size_t i=0; i < nI; ++i) {
        vm::array *nii=vm::read<vm::array*>(ni,i);
        if(checkArray(nii) != 3) reportError(wrongsize);
        uint32_t *NIi=NI[i];
        for(size_t j=0; j < 3; ++j) {
          size_t index=unsignedcast(vm::read<Int>(nii,j));
          if(index >= nN) reportError(outofrange);
          NIi[j]=index;
        }
      }
    }
  }

  drawBaseTriangles(const double* t, const drawBaseTriangles *s) :
    nP(s->nP), nN(s->nN), nI(s->nI) {
    P=new(UseGC) Triple[nP];
    transformTriples(t,nP,P,s->P);
    
    PI=new(UseGC) uint32_t[nI][3];
    for(size_t i=0; i < nI; ++i) {
      uint32_t *PIi=PI[i];
      uint32_t *sPIi=s->PI[i];
      for(size_t j=0; j < 3; ++j)
        PIi[j]=sPIi[j];
    }

    if(nN) {
      N=new(UseGC) Triple[nN];
      transformNormalsTriples(t,nN,N,s->N);
    
      NI=new(UseGC) uint32_t[nI][3];
      for(size_t i=0; i < nI; ++i) {
        uint32_t *NIi=NI[i];
        uint32_t *sNIi=s->NI[i];
        for(size_t j=0; j < 3; ++j)
          NIi[j]=sNIi[j];
      }
    }
  }
    
  bool is3D() {return true;}
  
  void bounds(const double* t, bbox3& b);
  
  void ratio(const double* t, pair &b, double (*m)(double, double), double fuzz,
             bool &first);
  
  virtual ~drawBaseTriangles() {}
  
  drawElement *transformed(const double* t) {
    return new drawBaseTriangles(t,this);
  }
};
  
class drawTriangles : public drawBaseTriangles {
  size_t nC;
  RGBAColour*C;
  uint32_t (*CI)[3];
   
  // Asymptote material data
  RGBAColour diffuse;
  RGBAColour ambient;
  RGBAColour emissive;
  RGBAColour specular;
  double opacity;
  double shininess;
  double PRCshininess;
  bool invisible;
   
public:
  drawTriangles(const vm::array& v, const vm::array& vi,
                const vm::array& n, const vm::array& ni,
                const vm::array&p, double opacity, double shininess,
                double PRCshininess, const vm::array& c, const vm::array& ci) :
    drawBaseTriangles(v,vi,n,ni),
    opacity(opacity), shininess(shininess), PRCshininess(PRCshininess) {

    const string needfourpens="array of 4 pens required";
    if(checkArray(&p) != 4)
      reportError(needfourpens);
      
    const pen surfacepen=vm::read<camp::pen>(p,0);
    invisible=surfacepen.invisible();
    diffuse=rgba(surfacepen);
    
    nC=checkArray(&c);
    if(nC) {
      C=new(UseGC) RGBAColour[nC];
      for(size_t i=0; i < nC; ++i)
        C[i]=rgba(vm::read<camp::pen>(c,i));
    
      size_t nI=checkArray(&vi);
    
      if(checkArray(&ci) != nI)
        reportError("Index arrays have different lengths");
      CI=new(UseGC) uint32_t[nI][3];
      for(size_t i=0; i < nI; ++i) {
        vm::array *cii=vm::read<vm::array*>(ci,i);
        if(checkArray(cii) != 3) reportError(wrongsize);
        uint32_t *CIi=CI[i];
        for(size_t j=0; j < 3; ++j) {
          size_t index=unsignedcast(vm::read<Int>(cii,j));
          if(index >= nC) reportError(outofrange);
          CIi[j]=index;
        }
      }
    } else {
      ambient=rgba(vm::read<camp::pen>(p,1));
      emissive=rgba(vm::read<camp::pen>(p,2));
    }
    specular=rgba(vm::read<camp::pen>(p,3));
  }
  
  drawTriangles(const double* t, const drawTriangles *s) :
    drawBaseTriangles(t,s), nC(s->nC),
    diffuse(s->diffuse), ambient(s->ambient), emissive(s->emissive),
    specular(s->specular), opacity(s->opacity),
    shininess(s->shininess), PRCshininess(s->PRCshininess),
    invisible(s->invisible) {
    
    if(nC) {
      C=new(UseGC) RGBAColour[nC];
      for(size_t i=0; i < nC; ++i)
        C[i]=s->C[i];
    
      CI=new(UseGC) uint32_t[nI][3];
      for(size_t i=0; i < nI; ++i) {
        uint32_t *CIi=CI[i];
        uint32_t *sCIi=s->CI[i];
        for(size_t j=0; j < 3; ++j)
          CIi[j]=sCIi[j];
      }
    }
  }
 
  virtual ~drawTriangles() {}
 
  void render(GLUnurbs *nurb, double size2, const triple& Min, const triple& Max,
              double perspective, bool lighton, bool transparent);
 
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
 
  drawElement *transformed(const double* t) {
    return new drawTriangles(t,this);
  }
};

}

#endif
