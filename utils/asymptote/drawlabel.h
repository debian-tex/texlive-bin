/*****
 * drawlabel.h
 * John Bowman 2003/03/14
 *
 * Add a label to a picture.
 *****/

#ifndef DRAWLABEL_H
#define DRAWLABEL_H

#include "drawelement.h"
#include "path.h"
#include "angle.h"
#include "transform.h"

namespace camp {
  
class drawLabel : public virtual drawElement {
protected:
  string label,size;
  transform T;          // A linear (shiftless) transformation.
  pair position;
  pair align;
  pair scale;
  pen pentype;
  double width,height,depth;
  bool havebounds;
  bool suppress;
  pair Align;
  pair texAlign;
  bbox Box;
  bool enabled;
  
public:
  drawLabel(string label, string size, transform T, pair position,
            pair align, pen pentype)
    : label(label), size(size), T(shiftless(T)), position(position),
      align(align), pentype(pentype), width(0.0), height(0.0), depth(0.0),
      havebounds(false), suppress(false), enabled(false) {} 
  
  virtual ~drawLabel() {}

  void getbounds(iopipestream& tex, const string& texengine);
  
  void checkbounds();
    
  void bounds(bbox& b, iopipestream&, boxvector&, bboxlist&);
  
  bool islabel() {
    return true;
  }

  bool write(texfile *out, const bbox&);

  drawElement *transformed(const transform& t);
  
  void labelwarning(const char *action); 
};

class drawLabelPath : public drawLabel, public drawPathPenBase {
private:  
  string justify;
  pair shift;
public:
  drawLabelPath(string label, string size, path src,
                string justify, pair shift, pen pentype) : 
    drawLabel(label,size,identity,pair(0.0,0.0),align,pentype),
    drawPathPenBase(src,pentype), justify(justify), shift(shift) {}
  
  virtual ~drawLabelPath() {}

  bool svg() {return true;}
  bool svgpng() {return true;}
  
  void bounds(bbox& b, iopipestream& tex, boxvector&, bboxlist&);
  
  bool write(texfile *out, const bbox&);
  
  drawElement *transformed(const transform& t);
};

void setpen(iopipestream& tex, const string& texengine, const pen& pentype);
bool texbounds(double& width, double& height, double& depth,
               iopipestream& tex, string& s, const char **abort,
               bool warn, bool Inline=false);

}

#endif
