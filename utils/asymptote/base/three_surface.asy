import bezulate;
private import interpolate;

int nslice=12;
real camerafactor=1.2;

string meshname(string name) {return name+" mesh";}

private real Fuzz=10.0*realEpsilon;
private real nineth=1/9;

struct patch {
  triple[][] P;
  triple[] normals; // Optionally specify 4 normal vectors at the corners.
  pen[] colors;     // Optionally specify 4 corner colors.
  bool straight;    // Patch is based on a piecewise straight external path.
  bool3 planar;     // Patch is planar.

  path3 external() {
    return straight ? P[0][0]--P[3][0]--P[3][3]--P[0][3]--cycle :
      P[0][0]..controls P[1][0] and P[2][0]..
      P[3][0]..controls P[3][1] and P[3][2]..
      P[3][3]..controls P[2][3] and P[1][3]..
      P[0][3]..controls P[0][2] and P[0][1]..cycle;
  }

  triple[] internal() {
    return new triple[] {P[1][1],P[2][1],P[2][2],P[1][2]};
  }

  triple cornermean() {
    return 0.25*(P[0][0]+P[0][3]+P[3][0]+P[3][3]);
  }

  triple[] corners() {return new triple[] {P[0][0],P[3][0],P[3][3],P[0][3]};}

  real[] map(real f(triple)) {
    return new real[] {f(P[0][0]),f(P[3][0]),f(P[3][3]),f(P[0][3])};
  }

  triple Bu(int j, real u) {return bezier(P[0][j],P[1][j],P[2][j],P[3][j],u);}
  triple BuP(int j, real u) {return bezierP(P[0][j],P[1][j],P[2][j],P[3][j],u);}
  triple BuPP(int j, real u) {
    return bezierPP(P[0][j],P[1][j],P[2][j],P[3][j],u);
  }
  triple BuPPP(int j) {return bezierPPP(P[0][j],P[1][j],P[2][j],P[3][j]);}

  path3 uequals(real u) {
    triple z0=Bu(0,u);
    triple z1=Bu(3,u);
    return path3(new triple[] {z0,Bu(2,u)},new triple[] {z0,z1},
                 new triple[] {Bu(1,u),z1},new bool[] {straight,false},false);
  }

  triple Bv(int i, real v) {return bezier(P[i][0],P[i][1],P[i][2],P[i][3],v);}
  triple BvP(int i, real v) {return bezierP(P[i][0],P[i][1],P[i][2],P[i][3],v);}
  triple BvPP(int i, real v) {
    return bezierPP(P[i][0],P[i][1],P[i][2],P[i][3],v);
  }
  triple BvPPP(int i) {return bezierPPP(P[i][0],P[i][1],P[i][2],P[i][3]);}

  path3 vequals(real v) {
    triple z0=Bv(0,v);
    triple z1=Bv(3,v);
    return path3(new triple[] {z0,Bv(2,v)},new triple[] {z0,z1},
                 new triple[] {Bv(1,v),z1},new bool[] {straight,false},false);
  }

  triple point(real u, real v) {        
    return bezier(Bu(0,u),Bu(1,u),Bu(2,u),Bu(3,u),v);
  }

  // compute normal vectors for degenerate cases
  private triple normal0(real u, real v, real epsilon) {
    triple n=0.5*(cross(bezier(BuPP(0,u),BuPP(1,u),BuPP(2,u),BuPP(3,u),v),
                        bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u))+
                  cross(bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v),   
                        bezier(BvPP(0,v),BvPP(1,v),BvPP(2,v),BvPP(3,v),u)));
    return abs(n) > epsilon ? n :
      0.25*cross(bezier(BuPP(0,u),BuPP(1,u),BuPP(2,u),BuPP(3,u),v),   
                 bezier(BvPP(0,v),BvPP(1,v),BvPP(2,v),BvPP(3,v),u))+
      1/6*(cross(bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v),   
                 bezier(BvPPP(0),BvPPP(1),BvPPP(2),BvPPP(3),u))+
           cross(bezier(BuPPP(0),BuPPP(1),BuPPP(2),BuPPP(3),v),
                 bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u)))+
      1/12*(cross(bezier(BuPPP(0),BuPPP(1),BuPPP(2),BuPPP(3),v),
                  bezier(BvPP(0,v),BvPP(1,v),BvPP(2,v),BvPP(3,v),u))+
            cross(bezier(BuPP(0,u),BuPP(1,u),BuPP(2,u),BuPP(3,u),v),   
                  bezier(BvPPP(0),BvPPP(1),BvPPP(2),BvPPP(3),u)))+
      1/36*cross(bezier(BuPPP(0),BuPPP(1),BuPPP(2),BuPPP(3),v),   
                 bezier(BvPPP(0),BvPPP(1),BvPPP(2),BvPPP(3),u));
  }

  static real fuzz=1000*realEpsilon;

  triple partialu(real u, real v) {
    return bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v);
  }

  triple partialv(real u, real v) {
    return bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u);
  }

  triple normal(real u, real v) {
    triple n=cross(partialu(u,v),partialv(u,v));
    real epsilon=fuzz*change2(P);
    return (abs(n) > epsilon) ? n : normal0(u,v,epsilon);
  }
  
  triple normal00() {
    triple n=9*cross(P[1][0]-P[0][0],P[0][1]-P[0][0]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(0,0,epsilon);
  }

  triple normal10() {
    triple n=9*cross(P[3][0]-P[2][0],P[3][1]-P[3][0]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(1,0,epsilon);
  }

  triple normal11() {
    triple n=9*cross(P[3][3]-P[2][3],P[3][3]-P[3][2]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(1,1,epsilon);
  }

  triple normal01() {
    triple n=9*cross(P[1][3]-P[0][3],P[0][3]-P[0][2]);
    real epsilon=fuzz*change2(P);
    return abs(n) > epsilon ? n : normal0(0,1,epsilon);
  }

  pen[] colors(material m, light light=currentlight) {
    bool nocolors=colors.length == 0;
    if(normals.length > 0)
      return new pen[] {color(normals[0],nocolors ? m : colors[0],light),
          color(normals[1],nocolors ? m : colors[1],light),
          color(normals[2],nocolors ? m : colors[2],light),
          color(normals[3],nocolors ? m : colors[3],light)};
    if(planar) {
      triple normal=normal(0.5,0.5);
      return new pen[] {color(normal,nocolors ? m : colors[0],light),
          color(normal,nocolors ? m : colors[1],light),
          color(normal,nocolors ? m : colors[2],light),
          color(normal,nocolors ? m : colors[3],light)};
    }
    return new pen[] {color(normal00(),nocolors ? m : colors[0],light),
        color(normal10(),nocolors ? m : colors[1],light),
        color(normal11(),nocolors ? m : colors[2],light),
        color(normal01(),nocolors ? m : colors[3],light)};
  }
  
  triple min3,max3;
  bool havemin3,havemax3;

  void init() {
    havemin3=false;
    havemax3=false;
  }

  triple min(triple bound=P[0][0]) {
    if(havemin3) return minbound(min3,bound);
    havemin3=true;
    return min3=minbezier(P,bound);
  }

  triple max(triple bound=P[0][0]) {
    if(havemax3) return maxbound(max3,bound);
    havemax3=true;
    return max3=maxbezier(P,bound);
  }

  triple center() {
    return 0.5*(this.min()+this.max());
  }

  pair min(projection P, pair bound=project(this.P[0][0],P.t)) {
    triple[][] Q=P.T.modelview*this.P;
    if(P.infinity)
      return xypart(minbezier(Q,(bound.x,bound.y,0)));
    real d=P.T.projection[3][2];
    return maxratio(Q,d*bound)/d; // d is negative
  }

  pair max(projection P, pair bound=project(this.P[0][0],P.t)) {
    triple[][] Q=P.T.modelview*this.P;
    if(P.infinity)
      return xypart(maxbezier(Q,(bound.x,bound.y,0)));
    real d=P.T.projection[3][2];
    return minratio(Q,d*bound)/d; // d is negative
  }

  void operator init(triple[][] P, triple[] normals=new triple[],
                     pen[] colors=new pen[], bool straight=false,
                     bool3 planar=default, bool copy=true) {
    init();
    this.P=copy ? copy(P) : P;
    if(normals.length != 0)
      this.normals=copy(normals);
    if(colors.length != 0)
      this.colors=copy(colors);
    this.planar=planar;
    this.straight=straight;
  }

  void operator init(pair[][] P, triple plane(pair)=XYplane,
                     bool straight=false) {
    triple[][] Q=new triple[4][];
    for(int i=0; i < 4; ++i) {
      pair[] Pi=P[i];
      Q[i]=sequence(new triple(int j) {return plane(Pi[j]);},4);
    }
    operator init(Q,straight);
    planar=true;
  }

  void operator init(patch s) {
    operator init(s.P,s.normals,s.colors,s.straight);
  }
  
  // A constructor for a convex cyclic path3 of length <= 4 with optional
  // arrays of 4 internal points, corner normals, and pens.
  void operator init(path3 external, triple[] internal=new triple[],
                     triple[] normals=new triple[], pen[] colors=new pen[],
                     bool3 planar=default) {
    init();

    if(internal.length == 0 && planar == default)
      this.planar=normal(external) != O;
    else this.planar=planar;

    int L=length(external);
    if(L > 4 || !cyclic(external))
      abort("cyclic path3 of length <= 4 expected");
    if(L == 1) {
      external=external--cycle--cycle--cycle;
      if(colors.length > 0) colors.append(array(3,colors[0]));
      if(normals.length > 0) normals.append(array(3,normals[0]));
    } else if(L == 2) {
      external=external--cycle--cycle;
      if(colors.length > 0) colors.append(array(2,colors[0]));
      if(normals.length > 0) normals.append(array(2,normals[0]));
    } else if(L == 3) {
      external=external--cycle;
      if(colors.length > 0) colors.push(colors[0]);
      if(normals.length > 0) normals.push(normals[0]);
    }
    if(normals.length != 0)
      this.normals=copy(normals);
    if(colors.length != 0)
      this.colors=copy(colors);

    if(internal.length == 0) {
      straight=piecewisestraight(external);
      internal=new triple[4];
      for(int j=0; j < 4; ++j)
        internal[j]=nineth*(-4*point(external,j)
                            +6*(precontrol(external,j)+postcontrol(external,j))
                            -2*(point(external,j-1)+point(external,j+1))
                            +3*(precontrol(external,j-1)+
                                postcontrol(external,j+1))
                            -point(external,j+2));
    } else straight=false;

    P=new triple[][] {
      {point(external,0),precontrol(external,0),postcontrol(external,3),
       point(external,3)},
      {postcontrol(external,0),internal[0],internal[3],precontrol(external,3)},
      {precontrol(external,1),internal[1],internal[2],postcontrol(external,2)},
      {point(external,1),postcontrol(external,1),precontrol(external,2),
       point(external,2)}
    };
  }

  // A constructor for a convex quadrilateral.
  void operator init(triple[] external, triple[] internal=new triple[],
                     triple[] normals=new triple[], pen[] colors=new pen[],
                     bool3 planar=default) {
    init();

    if(internal.length == 0 && planar == default)
      this.planar=normal(external) != O;
    else this.planar=planar;

    if(normals.length != 0)
      this.normals=copy(normals);
    if(colors.length != 0)
      this.colors=copy(colors);

    if(internal.length == 0) {
      internal=new triple[4];
      for(int j=0; j < 4; ++j)
        internal[j]=nineth*(4*external[j]+2*external[(j+1)%4]+
                            external[(j+2)%4]+2*external[(j+3)%4]);
    }

    straight=true;

    triple delta[]=new triple[4];
    for(int j=0; j < 4; ++j)
      delta[j]=(external[(j+1)% 4]-external[j])/3;

    P=new triple[][] {
      {external[0],external[0]-delta[3],external[3]+delta[3],external[3]},
      {external[0]+delta[0],internal[0],internal[3],external[3]-delta[2]},
      {external[1]-delta[0],internal[1],internal[2],external[2]+delta[2]},
      {external[1],external[1]+delta[1],external[2]-delta[1],external[2]}
    };
  }
}

patch operator * (transform3 t, patch s)
{ 
  patch S;
  S.P=new triple[4][4];
  for(int i=0; i < 4; ++i) { 
    triple[] si=s.P[i];
    triple[] Si=S.P[i];
    for(int j=0; j < 4; ++j)
      Si[j]=t*si[j]; 
  }
  
  if(s.normals.length > 0) {
    transform3 t0=shiftless(t);
    t0=determinant(t0) == 0 ? identity4 : transpose(inverse(t0));
    for(int i=0; i < s.normals.length; ++i)
      S.normals[i]=t0*s.normals[i];
  }

  S.colors=copy(s.colors);
  S.planar=s.planar;
  S.straight=s.straight;
  return S;
}
 
patch reverse(patch s) 
{
  patch S;
  S.P=transpose(s.P);
  if(s.normals.length > 0) 
    S.normals=
      new triple[] {s.normals[0],s.normals[3],s.normals[2],s.normals[1]};
  if(s.colors.length > 0) 
    S.colors=new pen[] {s.colors[0],s.colors[3],s.colors[2],s.colors[1]};
  S.planar=s.planar;
  S.straight=s.straight;
  return S;
}

// Return the Coons patch control points corresponding to path p.
pair[][] coons(path p)
{
  int L=length(p);
  if(L == 1)
    p=p--cycle--cycle--cycle;
  else if(L == 2)
    p=p--cycle--cycle;
  else if(L == 3)
    p=p--cycle;
 
  pair[] internal=new pair[4];
  for(int j=0; j < 4; ++j) {
    internal[j]=nineth*(-4*point(p,j)
                        +6*(precontrol(p,j)+postcontrol(p,j))
                        -2*(point(p,j-1)+point(p,j+1))
                        +3*(precontrol(p,j-1)+postcontrol(p,j+1))
                        -point(p,j+2));
  }
    
  return new pair[][] {
    {point(p,0),precontrol(p,0),postcontrol(p,3),point(p,3)},
      {postcontrol(p,0),internal[0],internal[3],precontrol(p,3)},
        {precontrol(p,1),internal[1],internal[2],postcontrol(p,2)},
          {point(p,1),postcontrol(p,1),precontrol(p,2),point(p,2)}
  };
}

// Decompose a possibly nonconvex cyclic path into an array of paths that
// yield nondegenerate Coons patches.
path[] regularize(path p, bool checkboundary=true)
{
  path[] s;

  if(!cyclic(p))
    abort("cyclic path expected");

  int L=length(p);

  if(L > 4) {
    for(path g : bezulate(p))
      s.append(regularize(g,checkboundary));
    return s;
  }
        
  bool straight=piecewisestraight(p);
  if(L <= 3 && straight) {
    return new path[] {p};
  }
    
  // Split p along the angle bisector at t.
  bool split(path p, real t) {
    pair dir=dir(p,t);
    if(dir != 0) {
      path g=subpath(p,t,t+length(p));
      int L=length(g);
      pair z=point(g,0);
      real[] T=intersections(g,z,z+I*dir);
      for(int i=0; i < T.length; ++i) {
        real cut=T[i];
        if(cut > sqrtEpsilon && cut < L-sqrtEpsilon) {
          pair w=point(g,cut);
          if(!inside(p,0.5*(z+w),zerowinding)) continue;
          pair delta=sqrtEpsilon*(w-z);
          if(intersections(g,z-delta--w+delta).length != 2) continue;
          s.append(regularize(subpath(g,0,cut)--cycle,checkboundary));
          s.append(regularize(subpath(g,cut,L)--cycle,checkboundary));
          return true;
        }
      }
    }
    return false;
  }

  // Ensure that all interior angles are less than 180 degrees.
  real fuzz=1e-4;
  int sign=sgn(windingnumber(p,inside(p,zerowinding)));
  for(int i=0; i < L; ++i) {
    if(sign*(conj(dir(p,i,-1))*dir(p,i,1)).y < -fuzz) {
      if(split(p,i)) return s;
    }
  }

  if(straight)
    return new path[] {p};
    
  pair[][] P=coons(p);

  // Check for degeneracy.
  pair[][] U=new pair[3][4];
  pair[][] V=new pair[4][3];

  for(int i=0; i < 3; ++i) {
    for(int j=0; j < 4; ++j)
      U[i][j]=P[i+1][j]-P[i][j];
  }
      
  for(int i=0; i < 4; ++i) {
    for(int j=0; j < 3; ++j)
      V[i][j]=P[i][j+1]-P[i][j];
  }
      
  int[] choose2={1,2,1};
  int[] choose3={1,3,3,1};

  real T[][]=new real[6][6];
  for(int p=0; p < 6; ++p) {
    int kstart=max(p-2,0);
    int kstop=min(p,3);
    real[] Tp=T[p];
    for(int q=0; q < 6; ++q) {
      real Tpq;
      int jstop=min(q,3);
      int jstart=max(q-2,0);
      for(int k=kstart; k <= kstop; ++k) {
        int choose3k=choose3[k];
        for(int j=jstart; j <= jstop; ++j) {
          int i=p-k;
          int l=q-j;
          Tpq += (conj(U[i][j])*V[k][l]).y*
            choose2[i]*choose3k*choose3[j]*choose2[l];
        }
      }
      Tp[q]=Tpq;
    }
  }

  bool3 aligned=default;
  bool degenerate=false;

  for(int p=0; p < 6; ++p) {
    for(int q=0; q < 6; ++q) {
      if(aligned == default) {
        if(T[p][q] > sqrtEpsilon) aligned=true;
        if(T[p][q] < -sqrtEpsilon) aligned=false;
      } else {
        if((T[p][q] > sqrtEpsilon && aligned == false) ||
           (T[p][q] < -sqrtEpsilon && aligned == true)) degenerate=true;
      }
    }
  }

  if(!degenerate) {
    if(aligned == (sign >= 0))
      return new path[] {p};
    return s;
  }

  if(checkboundary) {
    // Polynomial coefficients of (B_i'' B_j + B_i' B_j')/3.
    static real[][][] fpv0={
      {{5, -20, 30, -20, 5},
       {-3, 24, -54, 48, -15},
       {0, -6, 27, -36, 15},
       {0, 0, -3, 8, -5}},
      {{-7, 36, -66, 52, -15},
       {3, -36, 108, -120, 45},
       {0, 6, -45, 84, -45},
       {0, 0, 3, -16, 15}},
      {{2, -18, 45, -44, 15},
       {0, 12, -63, 96, -45},
       {0, 0, 18, -60, 45},
       {0, 0, 0, 8, -15}},
      {{0, 2, -9, 12, -5},
       {0, 0, 9, -24, 15},
       {0, 0, 0, 12, -15},
       {0, 0, 0, 0, 5}}
    };

    // Compute one-ninth of the derivative of the Jacobian along the boundary.
    real[][] c=array(4,array(5,0.0));
    for(int i=0; i < 4; ++i) {
      real[][] fpv0i=fpv0[i];
      for(int j=0; j < 4; ++j) {
        real[] w=fpv0i[j];
        c[0] += w*(conj(P[i][0])*(P[j][1]-P[j][0])).y; // v=0
        c[1] += w*(conj(P[3][j]-P[2][j])*P[3][i]).y;   // u=1
        c[2] += w*(conj(P[i][3])*(P[j][3]-P[j][2])).y; // v=1
        c[3] += w*(conj(P[0][j]-P[1][j])*P[0][i]).y;   // u=0
      }
    }
    
    pair BuP(int j, real u) {
      return bezierP(P[0][j],P[1][j],P[2][j],P[3][j],u);
    }
    pair BvP(int i, real v) {
      return bezierP(P[i][0],P[i][1],P[i][2],P[i][3],v);
    }
    real normal(real u, real v) {
      return (conj(bezier(BuP(0,u),BuP(1,u),BuP(2,u),BuP(3,u),v))*
              bezier(BvP(0,v),BvP(1,v),BvP(2,v),BvP(3,v),u)).y;
    }

    // Use Rolle's theorem to check for degeneracy on the boundary.
    real M=0;
    real cut;
    for(int i=0; i < 4; ++i) {
      if(!straight(p,i)) {
        real[] ci=c[i];
        pair[] R=quarticroots(ci[4],ci[3],ci[2],ci[1],ci[0]);
        for(pair r : R) {
          if(fabs(r.y) < sqrtEpsilon) {
            real t=r.x;
            if(0 <= t && t <= 1) {
              real[] U={t,1,t,0};
              real[] V={0,t,1,t};
              real[] T={t,t,1-t,1-t};
              real N=sign*normal(U[i],V[i]);
              if(N < M) {
                M=N; cut=i+T[i];
              }
            }
          }
        }
      }
    }

    // Split at the worst boundary degeneracy.
    if(M < 0 && split(p,cut)) return s;
  }
    
  // Split arbitrarily to resolve any remaining (internal) degeneracy.
  checkboundary=false;
  for(int i=0; i < L; ++i)
    if(!straight(p,i) && split(p,i+0.5)) return s;

  while(true)
    for(int i=0; i < L; ++i)
      if(!straight(p,i) && split(p,i+unitrand())) return s;

  return s;
}

struct surface {
  patch[] s;
  int index[][];
  bool vcyclic;
  
  bool empty() {
    return s.length == 0;
  }

  void operator init(int n) {
    s=new patch[n];
  }

  void operator init(... patch[] s) {
    this.s=s;
  }

  void operator init(surface s) {
    this.s=new patch[s.s.length];
    for(int i=0; i < s.s.length; ++i)
      this.s[i]=patch(s.s[i]);
    this.index=copy(s.index);
    this.vcyclic=s.vcyclic;
  }

  void operator init(triple[][][] P, triple[][] normals=new triple[][],
                     pen[][] colors=new pen[][], bool3 planar=default) {
    s=sequence(new patch(int i) {
        return patch(P[i],normals.length == 0 ? new triple[] : normals[i],
                     colors.length == 0 ? new pen[] : colors[i],planar);
      },P.length);
  }

  void colors(pen[][] palette) {
    for(int i=0; i < s.length; ++i)
      s[i].colors=copy(palette[i]);
  }

  triple[][] corners() {
    triple[][] a=new triple[s.length][];
    for(int i=0; i < s.length; ++i)
      a[i]=s[i].corners();
    return a;
  }

  real[][] map(real f(triple)) {
    real[][] a=new real[s.length][];
    for(int i=0; i < s.length; ++i)
      a[i]=s[i].map(f);
    return a;
  }

  triple[] cornermean() {
    return sequence(new triple(int i) {return s[i].cornermean();},s.length);
  }

  triple point(real u, real v) {
    int U=floor(u);
    int V=floor(v);
    int index=index.length == 0 ? U+V : index[U][V];
    return s[index].point(u-U,v-V);
  }    

  triple normal(real u, real v) {
    int U=floor(u);
    int V=floor(v);
    int index=index.length == 0 ? U+V : index[U][V];
    return s[index].normal(u-U,v-V);
  }
  
  void ucyclic(bool f) 
  {
    index.cyclic=f;
  }
  
  void vcyclic(bool f) 
  {
    for(int[] i : index)
      i.cyclic=f;
    vcyclic=f;
  }
  
  bool ucyclic() 
  {
    return index.cyclic;
  }
  
  bool vcyclic() 
  {
    return vcyclic;
  }

  path3 uequals(real u) {
    if(index.length == 0) return nullpath3;
    int U=floor(u);
    int[] index=index[U];
    path3 g;
    for(int i : index)
      g=g&s[i].uequals(u-U);
    return vcyclic() ? g&cycle : g;
  }
  
  path3 vequals(real v) {
    if(index.length == 0) return nullpath3;
    int V=floor(v);
    path3 g;
    for(int[] i : index)
      g=g&s[i[V]].vequals(v-V);
    return ucyclic() ? g&cycle : g;
  }
  
  // A constructor for a possibly nonconvex simple cyclic path in a given plane.
  void operator init(path p, triple plane(pair)=XYplane) {
    bool straight=piecewisestraight(p);
    for(path g : regularize(p))
      s.push(patch(coons(g),plane,straight));
  }

  void operator init(explicit path[] g, triple plane(pair)=XYplane) {
    for(path p : bezulate(g))
      s.append(surface(p,plane).s);
  }

  // A general surface constructor for both planar and nonplanar 3D paths.
  void construct(path3 external, triple[] internal=new triple[],
                 triple[] normals=new triple[], pen[] colors=new pen[],
                 bool3 planar=default) {
    int L=length(external);
    if(!cyclic(external)) abort("cyclic path expected");

    if(L <= 3 && piecewisestraight(external)) {
      s.push(patch(external,internal,normals,colors,planar=true));
      return;
    }

    // Construct a surface from a possibly nonconvex planar cyclic path3.
    if(planar != false && internal.length == 0 && normals.length == 0 &&
       colors.length == 0) {
      triple n=normal(external);
      if(n != O) {
        transform3 T=align(n);
        external=transpose(T)*external;
        T *= shift(0,0,point(external,0).z);
        for(patch p : surface(path(external)).s)
          s.push(T*p);
        return;
      }
    }
    
    if(L <= 4 || internal.length > 0) {
      s.push(patch(external,internal,normals,colors,planar));
      return;
    }
      
    // Path is not planar; split into patches.
    real factor=1/L;
    pen[] p;
    triple[] n;
    bool nocolors=colors.length == 0;
    bool nonormals=normals.length == 0;
    triple center;
    for(int i=0; i < L; ++i)
      center += point(external,i);
    center *= factor;
    if(!nocolors)
      p=new pen[] {mean(colors)};
    if(!nonormals)
      n=new triple[] {factor*sum(normals)};
    // Use triangles for nonplanar surfaces.
    int step=normal(external) == O ? 1 : 2;
    int i=0;
    int end;
    while((end=i+step) < L) {
      s.push(patch(subpath(external,i,end)--center--cycle,
                   nonormals ? n : concat(normals[i:end+1],n),
                   nocolors ? p : concat(colors[i:end+1],p),planar));
      i=end;
    }
    s.push(patch(subpath(external,i,L)--center--cycle,
                 nonormals ? n : concat(normals[i:],normals[0:1],n),
                 nocolors ? p : concat(colors[i:],colors[0:1],p),planar));
  }

  void operator init(path3 external, triple[] internal=new triple[],
                     triple[] normals=new triple[], pen[] colors=new pen[],
                     bool3 planar=default) {
    s=new patch[];
    construct(external,internal,normals,colors,planar);
  }

  void operator init(explicit path3[] external,
                     triple[][] internal=new triple[][],
                     triple[][] normals=new triple[][],
                     pen[][] colors=new pen[][], bool3 planar=default) {
    s=new patch[];
    if(planar == true) {// Assume all path3 elements share a common normal.
      if(external.length != 0) {
        triple n=normal(external[0]);
        if(n != O) {
          transform3 T=align(n);
          external=transpose(T)*external;
          T *= shift(0,0,point(external[0],0).z);
          path[] g=sequence(new path(int i) {return path(external[i]);},
                            external.length);
          for(patch p : surface(g).s)
            s.push(T*p);
          return;
        }
      }
    }

    for(int i=0; i < external.length; ++i)
      construct(external[i],
                internal.length == 0 ? new triple[] : internal[i],
                normals.length == 0 ? new triple[] : normals[i],
                colors.length == 0 ? new pen[] : colors[i],planar);
  }

  void push(path3 external, triple[] internal=new triple[],
            triple[] normals=new triple[] ,pen[] colors=new pen[],
            bool3 planar=default) {
    s.push(patch(external,internal,normals,colors,planar));
  }

  // Construct the surface of rotation generated by rotating g
  // from angle1 to angle2 sampled n times about the line c--c+axis.
  // An optional surface pen color(int i, real j) may be specified
  // to override the color at vertex(i,j).
  void operator init(triple c, path3 g, triple axis, int n=nslice,
                     real angle1=0, real angle2=360,
                     pen color(int i, real j)=null) {
    axis=unit(axis);
    real w=(angle2-angle1)/n;
    int L=length(g);
    s=new patch[L*n];
    index=new int[n][L];
    int m=-1;
    transform3[] T=new transform3[n+1];
    transform3 t=rotate(w,c,c+axis);
    T[0]=rotate(angle1,c,c+axis);
    for(int k=1; k <= n; ++k)
      T[k]=T[k-1]*t;

    typedef pen colorfcn(int i, real j);
    bool defaultcolors=(colorfcn) color == null;
    
    for(int i=0; i < L; ++i) {
      path3 h=subpath(g,i,i+1);
      path3 r=reverse(h);
      path3 H=shift(-c)*h;
      real M=0;
      triple perp;
      void test(real[] t) {
        for(int i=0; i < 3; ++i) {
          triple v=point(H,t[i]);
          triple V=v-dot(v,axis)*axis;
          real a=abs(V);
          if(a > M) {M=a; perp=V;}
        }
      }
      test(maxtimes(H));
      test(mintimes(H));
      
      perp=unit(perp);
      triple normal=unit(cross(axis,perp));
      triple dir(real j) {return Cos(j)*normal-Sin(j)*perp;}
      real j=angle1;
      transform3 Tk=T[0];
      triple dirj=dir(j);
      for(int k=0; k < n; ++k, j += w) {
        transform3 Tp=T[k+1];
        triple dirp=dir(j+w);
        path3 G=reverse(Tk*h{dirj}..{dirp}Tp*r{-dirp}..{-dirj}cycle);
        Tk=Tp;
        dirj=dirp;
        s[++m]=defaultcolors ? patch(G) :
          patch(G,new pen[] {color(i,j),color(i,j+w),color(i+1,j+w),
                             color(i+1,j)});
        index[k][i]=m;
      }
      ucyclic((angle2-angle1) % 360 == 0);
      vcyclic(cyclic(g));
    }
  }

  void push(patch s) {
    this.s.push(s);
  }

  void append(surface s) {
    this.s.append(s.s);
  }

  void operator init(... surface[] s) {
    for(surface S : s)
      this.s.append(S.s);
  }
}

surface operator * (transform3 t, surface s)
{ 
  surface S;
  S.s=new patch[s.s.length];
  for(int i=0; i < s.s.length; ++i)
    S.s[i]=t*s.s[i];
  S.index=copy(s.index);
  S.vcyclic=(bool) s.vcyclic;
  
  return S;
}

private string nullsurface="null surface";

triple min(surface s)
{
  if(s.s.length == 0)
    abort(nullsurface);
  triple bound=s.s[0].min();
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].min(bound);
  return bound;
}
  
triple max(surface s)
{
  if(s.s.length == 0)
    abort(nullsurface);
  triple bound=s.s[0].max();
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].max(bound);
  return bound;
}

pair min(surface s, projection P)
{
  if(s.s.length == 0)
    abort(nullsurface);
  pair bound=s.s[0].min(P);
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].min(P,bound);
  return bound;
}
  
pair max(surface s, projection P)
{
  if(s.s.length == 0)
    abort(nullsurface);
  pair bound=s.s[0].max(P);
  for(int i=1; i < s.s.length; ++i)
    bound=s.s[i].max(P,bound);
  return bound;
}

private triple[] split(triple z0, triple c0, triple c1, triple z1, real t=0.5)
{
  triple m0=interp(z0,c0,t);
  triple m1=interp(c0,c1,t);
  triple m2=interp(c1,z1,t);
  triple m3=interp(m0,m1,t);
  triple m4=interp(m1,m2,t);
  triple m5=interp(m3,m4,t);

  return new triple[] {m0,m3,m5,m4,m2};
}

// Return the control points of the subpatches
// produced by a horizontal split of P
triple[][][] hsplit(triple[][] P)
{
  // get control points in rows
  triple[] P0=P[0];
  triple[] P1=P[1];
  triple[] P2=P[2];
  triple[] P3=P[3];

  triple[] c0=split(P0[0],P0[1],P0[2],P0[3]);
  triple[] c1=split(P1[0],P1[1],P1[2],P1[3]);
  triple[] c2=split(P2[0],P2[1],P2[2],P2[3]);
  triple[] c3=split(P3[0],P3[1],P3[2],P3[3]);
  // bottom, top
  return new triple[][][] {
    {{P0[0],c0[0],c0[1],c0[2]},
        {P1[0],c1[0],c1[1],c1[2]},
          {P2[0],c2[0],c2[1],c2[2]},
            {P3[0],c3[0],c3[1],c3[2]}},
      {{c0[2],c0[3],c0[4],P0[3]},
          {c1[2],c1[3],c1[4],P1[3]},
            {c2[2],c2[3],c2[4],P2[3]},
              {c3[2],c3[3],c3[4],P3[3]}}
  };
}

// Return the control points of the subpatches
// produced by a vertical split of P
triple[][][] vsplit(triple[][] P)
{
  // get control points in rows
  triple[] P0=P[0];
  triple[] P1=P[1];
  triple[] P2=P[2];
  triple[] P3=P[3];

  triple[] c0=split(P0[0],P1[0],P2[0],P3[0]);
  triple[] c1=split(P0[1],P1[1],P2[1],P3[1]);
  triple[] c2=split(P0[2],P1[2],P2[2],P3[2]);
  triple[] c3=split(P0[3],P1[3],P2[3],P3[3]);
  // left, right
  return new triple[][][] {
    {{P0[0],P0[1],P0[2],P0[3]},
        {c0[0],c1[0],c2[0],c3[0]},
          {c0[1],c1[1],c2[1],c3[1]},
            {c0[2],c1[2],c2[2],c3[2]}},
      {{c0[2],c1[2],c2[2],c3[2]},
          {c0[3],c1[3],c2[3],c3[3]},
            {c0[4],c1[4],c2[4],c3[4]},
              {P3[0],P3[1],P3[2],P3[3]}}
  };
}

// Return a 2D array of the control point arrays of the subpatches
// produced by horizontal and vertical splits of P at u and v
triple[][][][] split(triple[][] P, real u=1/2, real v=1/2)
{
  triple[] P0=P[0];
  triple[] P1=P[1];
  triple[] P2=P[2];
  triple[] P3=P[3];
  
  // slice horizontally
  triple[] c0=split(P0[0],P0[1],P0[2],P0[3],v);
  triple[] c1=split(P1[0],P1[1],P1[2],P1[3],v);
  triple[] c2=split(P2[0],P2[1],P2[2],P2[3],v);
  triple[] c3=split(P3[0],P3[1],P3[2],P3[3],v);
  
  // bottom patch
  triple[] c4=split(P0[0],P1[0],P2[0],P3[0],u);
  triple[] c5=split(c0[0],c1[0],c2[0],c3[0],u);
  triple[] c6=split(c0[1],c1[1],c2[1],c3[1],u);
  triple[] c7=split(c0[2],c1[2],c2[2],c3[2],u);

  // top patch
  triple[] c8=split(c0[3],c1[3],c2[3],c3[3],u);
  triple[] c9=split(c0[4],c1[4],c2[4],c3[4],u);
  triple[] cA=split(P0[3],P1[3],P2[3],P3[3],u);

  // {{bottom-left, top-left}, {bottom-right, top-right}}
  return new triple[][][][] {
    {{{P0[0],c0[0],c0[1],c0[2]},
          {c4[0],c5[0],c6[0],c7[0]},
            {c4[1],c5[1],c6[1],c7[1]},
              {c4[2],c5[2],c6[2],c7[2]}},
        {{c0[2],c0[3],c0[4],P0[3]},
            {c7[0],c8[0],c9[0],cA[0]},
              {c7[1],c8[1],c9[1],cA[1]},
                {c7[2],c8[2],c9[2],cA[2]}}},
      {{{c4[2],c5[2],c6[2],c7[2]},
            {c4[3],c5[3],c6[3],c7[3]},
              {c4[4],c5[4],c6[4],c7[4]},
                {P3[0],c3[0],c3[1],c3[2]}},
          {{c7[2],c8[2],c9[2],cA[2]},
              {c7[3],c8[3],c9[3],cA[3]},
                {c7[4],c8[4],c9[4],cA[4]},
                  {c3[2],c3[3],c3[4],P3[3]}}}
  };
}

// Return the control points for a subpatch of P on [u,1] x [v,1].
triple[][] subpatchend(triple[][] P, real u, real v)
{
  triple[] P0=P[0];
  triple[] P1=P[1];
  triple[] P2=P[2];
  triple[] P3=P[3];

  triple[] c0=split(P0[0],P0[1],P0[2],P0[3],v);
  triple[] c1=split(P1[0],P1[1],P1[2],P1[3],v);
  triple[] c2=split(P2[0],P2[1],P2[2],P2[3],v);
  triple[] c3=split(P3[0],P3[1],P3[2],P3[3],v);

  triple[] c7=split(c0[2],c1[2],c2[2],c3[2],u);
  triple[] c8=split(c0[3],c1[3],c2[3],c3[3],u);
  triple[] c9=split(c0[4],c1[4],c2[4],c3[4],u);
  triple[] cA=split(P0[3],P1[3],P2[3],P3[3],u);

  return new triple[][] {
    {c7[2],c8[2],c9[2],cA[2]},
      {c7[3],c8[3],c9[3],cA[3]},
        {c7[4],c8[4],c9[4],cA[4]},
          {c3[2],c3[3],c3[4],P3[3]}};
}

// Return the control points for a subpatch of P on [0,u] x [0,v].
triple[][] subpatchbegin(triple[][] P, real u, real v)
{
  triple[] P0=P[0];
  triple[] P1=P[1];
  triple[] P2=P[2];
  triple[] P3=P[3];

  triple[] c0=split(P0[0],P0[1],P0[2],P0[3],v);
  triple[] c1=split(P1[0],P1[1],P1[2],P1[3],v);
  triple[] c2=split(P2[0],P2[1],P2[2],P2[3],v);
  triple[] c3=split(P3[0],P3[1],P3[2],P3[3],v);

  triple[] c4=split(P0[0],P1[0],P2[0],P3[0],u);
  triple[] c5=split(c0[0],c1[0],c2[0],c3[0],u);
  triple[] c6=split(c0[1],c1[1],c2[1],c3[1],u);
  triple[] c7=split(c0[2],c1[2],c2[2],c3[2],u);

  return new triple[][] {
    {P0[0],c0[0],c0[1],c0[2]},
      {c4[0],c5[0],c6[0],c7[0]},
        {c4[1],c5[1],c6[1],c7[1]},
          {c4[2],c5[2],c6[2],c7[2]}};
}

triple[][] subpatch(triple[][] P, pair a, pair b) 
{
  return subpatchend(subpatchbegin(P,b.x,b.y),a.x/b.x,a.y/b.y);
}

patch subpatch(patch s, pair a, pair b)
{
  assert(a.x >= 0 && a.y >= 0 && b.x <= 1 && b.y <= 1 &&
         a.x < b.x && a.y < b.y);
  return patch(subpatch(s.P,a,b),s.straight,s.planar);
}


// return an array containing all intersection times of path p and patch s.
real[][] intersections(path3 p, patch s, real fuzz=-1)
{
  return sort(intersections(p,s.P,fuzz));
}

// return an array containing all intersection times of path p and surface s.
real[][] intersections(path3 p, surface s, real fuzz=-1)
{
  real[][] T;
  if(length(p) < 0) return T;
  for(int i=0; i < s.s.length; ++i)
    for(real[] s: intersections(p,s.s[i].P,fuzz))
      T.push(s);

  static real Fuzz=1000*realEpsilon;
  real fuzz=max(10*fuzz,Fuzz*max(abs(min(s)),abs(max(s))));
  
  // Remove intrapatch duplicate points.
  for(int i=0; i < T.length; ++i) {
    triple v=point(p,T[i][0]);
    for(int j=i+1; j < T.length;) {
      if(abs(v-point(p,T[j][0])) < fuzz)
        T.delete(j);
      else ++j;
    }
  }
  return sort(T);
}

// return an array containing all intersection points of path p and surface s.
triple[] intersectionpoints(path3 p, patch s, real fuzz=-1)
{
  real[][] t=intersections(p,s,fuzz);
  return sequence(new triple(int i) {return point(p,t[i][0]);},t.length);
}

// return an array containing all intersection points of path p and surface s.
triple[] intersectionpoints(path3 p, surface s, real fuzz=-1)
{
  real[][] t=intersections(p,s,fuzz);
  return sequence(new triple(int i) {return point(p,t[i][0]);},t.length);
}

// Return true iff the bounding boxes of patch p and q overlap.
bool overlap(triple[][] p, triple[][] q, real fuzz=-1)
{
  triple p0=p[0][0];
  triple q0=q[0][0];
  triple pmin=minbezier(p,p0);
  triple pmax=maxbezier(p,p0);
  triple qmin=minbezier(q,q0);
  triple qmax=maxbezier(q,q0);

  static real Fuzz=1000*realEpsilon;
  real fuzz=max(10*fuzz,Fuzz*max(abs(pmin),abs(pmax)));
  
  return
    pmax.x+fuzz >= qmin.x &&
    pmax.y+fuzz >= qmin.y &&
    pmax.z+fuzz >= qmin.z &&
    qmax.x+fuzz >= pmin.x &&
    qmax.y+fuzz >= pmin.y &&
    qmax.z+fuzz >= pmin.z; // Overlapping bounding boxes?
}

triple point(patch s, real u, real v)
{
  return s.point(u,v);
}

real PRCshininess(real shininess) 
{
  // Empirical translation table from Phong-Blinn to PRC shininess model:
  static real[] x={0.015,0.025,0.05,0.07,0.1,0.14,0.23,0.5,0.65,0.75,0.85,
                   0.875,0.9,1};
  static real[] y={0.05,0.1,0.15,0.2,0.25,0.3,0.4,0.5,0.55,0.6,0.7,0.8,0.9,1};
  static realfunction s=fspline(x,y,monotonic);
  return s(shininess);
}

struct interaction
{
  int type;
  bool targetsize;
  void operator init(int type, bool targetsize=false) {
    this.type=type;
    this.targetsize=targetsize;
  }
}

restricted interaction Embedded=interaction(0);
restricted interaction Billboard=interaction(1);

interaction LabelInteraction()
{
  return settings.autobillboard ? Billboard : Embedded;
}

material material(material m, light light) 
{
  return light.on() || invisible((pen) m) ? m : emissive(m);
}

void draw3D(frame f, int type=0, patch s, triple center=O, material m,
            light light=currentlight, interaction interaction=Embedded,
            bool prc=true)
{
  if(s.colors.length > 0)
    m=mean(s.colors);
  m=material(m,light);
  real PRCshininess;
  if(prc())
    PRCshininess=PRCshininess(m.shininess);
  
  draw(f,s.P,center,s.straight,m.p,m.opacity,m.shininess,PRCshininess,
       s.planar ? s.normal(0.5,0.5) : O,s.colors,interaction.type,prc);
}

// Draw triangles on a frame.
void draw(frame f, triple[] v, int[][] vi,
          triple[] n={}, int[][] ni={}, material m=currentpen, pen[] p={},
          int[][] pi={}, light light=currentlight)
{
  if(p.length > 0)
    m=mean(p);
  m=material(m,light);
  real PRCshininess;
  if(prc())
    PRCshininess=PRCshininess(m.shininess);
  draw(f,v,vi,n,ni,m.p,m.opacity,m.shininess,PRCshininess,p,pi);
}
  
// Draw triangles on a picture.
void draw(picture pic=currentpicture, triple[] v, int[][] vi,
          triple[] n={}, int[][] ni={}, material m=currentpen, pen[] p={},
          int[][] pi={}, light light=currentlight)
{
  bool colors=pi.length > 0;
  bool normals=ni.length > 0;
  if(!colors && !normals) {
    n=new triple[];
    ni=new int[vi.length][3];
    triple lastnormal=O;
    for(int i=0; i < vi.length; ++i) {
      int[] vii=vi[i];
      int[] nii=ni[i];
      triple normal=normal(new triple[] {v[vii[0]],v[vii[1]],v[vii[2]]});
      if(normal != lastnormal || n.length == 0) {
        n.push(normal);
        lastnormal=normal;
      }
      nii[0]=nii[1]=nii[2]=n.length-1;
    }
  }

  pic.add(new void(frame f, transform3 t, picture pic, projection P) {
      triple[] v=t*v;
      triple[] n=t*n;

      if(is3D()) {
        draw(f,v,vi,n,ni,m,p,pi,light);
        if(pic != null) {
          for(int[] vii : vi)
            for(int viij : vii)
              pic.addPoint(project(v[viij],P));
        }
      } else if(pic != null) {
        static int[] edges={0,0,1};
        if(colors) {
          for(int i=0; i < vi.length; ++i) {
            int[] vii=vi[i];
            int[] pii=pi[i];
            gouraudshade(pic,project(v[vii[0]],P)--project(v[vii[1]],P)--
                         project(v[vii[2]],P)--cycle,
                         new pen[] {p[pii[0]],p[pii[1]],p[pii[2]]},edges);
          }
        } else {
          if(normals) {
            for(int i=0; i < vi.length; ++i) {
              int[] vii=vi[i];
              int[] nii=ni[i];
              gouraudshade(pic,project(v[vii[0]],P)--project(v[vii[1]],P)--
                           project(v[vii[2]],P)--cycle,
                           new pen[] {color(n[nii[0]],m,light),
                               color(n[nii[1]],m,light),
                               color(n[nii[2]],m,light)},edges);
            }
          } else {
            for(int i=0; i < vi.length; ++i) {
              int[] vii=vi[i];
              path g=project(v[vii[0]],P)--project(v[vii[1]],P)--
                project(v[vii[2]],P)--cycle;
              pen p=color(n[ni[i][0]],m,light);
              fill(pic,g,p);
              if(opacity(m.diffuse()) == 1) // Fill subdivision cracks
                draw(pic,g,p);
            }
          }
        }
      }   
    },true);

  for(int[] vii : vi)
    for(int viij : vii)
      pic.addPoint(v[viij]);
}

void drawPRCsphere(frame f, transform3 t=identity4, bool half=false, material m,
                   light light=currentlight, render render=defaultrender)
{
  m=material(m,light);
  drawPRCsphere(f,t,half,m.p,m.opacity,PRCshininess(m.shininess),render.sphere);
}

void drawPRCcylinder(frame f, transform3 t=identity4, material m,
                     light light=currentlight)
{
  m=material(m,light);
  drawPRCcylinder(f,t,m.p,m.opacity,PRCshininess(m.shininess));
}

void drawPRCdisk(frame f, transform3 t=identity4, material m,
                 light light=currentlight)
{
  m=material(m,light);
  drawPRCdisk(f,t,m.p,m.opacity,PRCshininess(m.shininess));
}

void drawPRCtube(frame f, path3 center, path3 g, material m,
                 light light=currentlight)
{
  m=material(m,light);
  drawPRCtube(f,center,g,m.p,m.opacity,PRCshininess(m.shininess));
}

void tensorshade(transform t=identity(), frame f, patch s,
                 material m, light light=currentlight, projection P)
{
  tensorshade(f,box(t*s.min(P),t*s.max(P)),m.diffuse(),
              s.colors(m,light),t*project(s.external(),P,1),
              t*project(s.internal(),P));
}

restricted pen[] nullpens={nullpen};
nullpens.cyclic=true;

void draw(transform t=identity(), frame f, surface s, int nu=1, int nv=1,
          material[] surfacepen, pen[] meshpen=nullpens,
          light light=currentlight, light meshlight=light, string name="",
          render render=defaultrender, projection P=currentprojection)
{
  bool is3D=is3D();
  if(is3D) {
    bool group=name != "" || render.defaultnames;
    if(group)
      begingroup3(f,name == "" ? "surface" : name,render);
    for(int i=0; i < s.s.length; ++i)
      draw3D(f,s.s[i],surfacepen[i],light);
    if(group)
      endgroup3(f);
    pen modifiers=thin()+squarecap;
    for(int k=0; k < s.s.length; ++k) {
      pen meshpen=meshpen[k];
      if(!invisible(meshpen)) {
        if(group)
          begingroup3(f,meshname(name),render);
        meshpen=modifiers+meshpen;
        real step=nu == 0 ? 0 : 1/nu;
        for(int i=0; i <= nu; ++i)
          draw(f,s.s[k].uequals(i*step),meshpen,meshlight,partname(i,render),
               render);
        step=nv == 0 ? 0 : 1/nv;
        for(int j=0; j <= nv; ++j)
          draw(f,s.s[k].vequals(j*step),meshpen,meshlight,partname(j,render),
               render);
        if(group)
          endgroup3(f);
      }
    }
  }
  if(!is3D || settings.render == 0) {
    begingroup(f);
    // Sort patches by mean distance from camera
    triple camera=P.camera;
    if(P.infinity) {
      triple m=min(s);
      triple M=max(s);
      camera=P.target+camerafactor*(abs(M-m)+abs(m-P.target))*unit(P.vector());
    }

    real[][] depth=new real[s.s.length][];
    for(int i=0; i < depth.length; ++i)
      depth[i]=new real[] {abs(camera-s.s[i].cornermean()),i};

    depth=sort(depth);

    light.T=shiftless(P.T.modelview);

    // Draw from farthest to nearest
    while(depth.length > 0) {
      real[] a=depth.pop();
      int i=round(a[1]);
      tensorshade(t,f,s.s[i],surfacepen[i],light,P);
      pen meshpen=meshpen[i];
      if(!invisible(meshpen))
        draw(f,t*project(s.s[i].external(),P),meshpen);
    }
    endgroup(f);
  }
}

void draw(transform t=identity(), frame f, surface s, int nu=1, int nv=1,
          material surfacepen=currentpen, pen meshpen=nullpen,
          light light=currentlight, light meshlight=light, string name="",
          render render=defaultrender, projection P=currentprojection)
{
  material[] surfacepen={surfacepen};
  pen[] meshpen={meshpen};
  surfacepen.cyclic=true;
  meshpen.cyclic=true;
  draw(t,f,s,nu,nv,surfacepen,meshpen,light,meshlight,name,render,P);
}

void draw(picture pic=currentpicture, surface s, int nu=1, int nv=1,
          material[] surfacepen, pen[] meshpen=nullpens,
          light light=currentlight, light meshlight=light, string name="",
          render render=defaultrender)
{
  if(s.empty()) return;

  bool cyclic=surfacepen.cyclic;
  surfacepen=copy(surfacepen);
  surfacepen.cyclic=cyclic;
  cyclic=meshpen.cyclic;
  meshpen=copy(meshpen);
  meshpen.cyclic=cyclic;

  pic.add(new void(frame f, transform3 t, picture pic, projection P) {
      surface S=t*s;
      if(is3D())
        draw(f,S,nu,nv,surfacepen,meshpen,light,meshlight,name,render);
      if(pic != null) {
        pic.add(new void(frame f, transform T) {
            draw(T,f,S,nu,nv,surfacepen,meshpen,light,meshlight,P);
          },true);
        pic.addPoint(min(S,P));
        pic.addPoint(max(S,P));
      }
    },true);
  pic.addPoint(min(s));
  pic.addPoint(max(s));

  pen modifiers;
  if(is3D()) modifiers=thin()+squarecap;
  for(int k=0; k < s.s.length; ++k) {
    pen meshpen=meshpen[k];
    if(!invisible(meshpen)) {
      meshpen=modifiers+meshpen;
      real step=nu == 0 ? 0 : 1/nu;
      for(int i=0; i <= nu; ++i)
        addPath(pic,s.s[k].uequals(i*step),meshpen);
      step=nv == 0 ? 0 : 1/nv;
      for(int j=0; j <= nv; ++j)
        addPath(pic,s.s[k].vequals(j*step),meshpen);
    }
  }
}

void draw(picture pic=currentpicture, surface s, int nu=1, int nv=1,
          material surfacepen=currentpen, pen meshpen=nullpen,
          light light=currentlight, light meshlight=light, string name="",
          render render=defaultrender)
{
  material[] surfacepen={surfacepen};
  pen[] meshpen={meshpen};
  surfacepen.cyclic=true;
  meshpen.cyclic=true;
  draw(pic,s,nu,nv,surfacepen,meshpen,light,meshlight,name,render);
}

void draw(picture pic=currentpicture, surface s, int nu=1, int nv=1,
          material[] surfacepen, pen meshpen,
          light light=currentlight, light meshlight=light, string name="",
          render render=defaultrender)
{
  pen[] meshpen={meshpen};
  meshpen.cyclic=true;
  draw(pic,s,nu,nv,surfacepen,meshpen,light,meshlight,name,render);
}

surface extrude(path3 p, path3 q)
{
  static patch[] allocate;
  return surface(...sequence(new patch(int i) {
        return patch(subpath(p,i,i+1)--subpath(q,i+1,i)--cycle);
      },length(p)));
}

surface extrude(path3 p, triple axis=Z)
{
  return extrude(p,shift(axis)*p);
}

surface extrude(path p, triple plane(pair)=XYplane, triple axis=Z)
{
  return extrude(path3(p,plane),axis);
}

surface extrude(explicit path[] p, triple axis=Z)
{
  surface s;
  for(path g:p)
    s.append(extrude(g,axis));
  return s;
}

triple rectify(triple dir) 
{
  real scale=max(abs(dir.x),abs(dir.y),abs(dir.z));
  if(scale != 0) dir *= 0.5/scale;
  dir += (0.5,0.5,0.5);
  return dir;
}

path3[] align(path3[] g, transform3 t=identity4, triple position,
              triple align, pen p=currentpen)
{
  if(determinant(t) == 0 || g.length == 0) return g;
  triple m=min(g);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(g)-m);
  return shift(position+align*labelmargin(p))*t*shift(-a)*g;
}

surface align(surface s, transform3 t=identity4, triple position,
              triple align, pen p=currentpen)
{
  if(determinant(t) == 0 || s.s.length == 0) return s;
  triple m=min(s);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(s)-m);
  return shift(position+align*labelmargin(p))*t*shift(-a)*s;
}

surface surface(Label L, triple position=O, bool bbox=false)
{
  surface s=surface(texpath(L,bbox=bbox));
  return L.align.is3D ? align(s,L.T3,position,L.align.dir3,L.p) :
    shift(position)*L.T3*s;
}

private path[] path(Label L, pair z=0, projection P)
{
  path[] g=texpath(L,bbox=P.bboxonly);
  return L.align.is3D ? align(g,z,project(L.align.dir3,P)-project(O,P),L.p) :
    shift(z)*g;
}

transform3 alignshift(path3[] g, transform3 t=identity4, triple position,
                      triple align)
{
  if(determinant(t) == 0) return identity4;
  triple m=min(g);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(g)-m);
  return shift(-a);
}

transform3 alignshift(surface s, transform3 t=identity4, triple position,
                      triple align)
{
  if(determinant(t) == 0) return identity4;
  triple m=min(s);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(s)-m);
  return shift(-a);
}

transform3 aligntransform(path3[] g, transform3 t=identity4, triple position,
                          triple align, pen p=currentpen)
{
  if(determinant(t) == 0) return identity4;
  triple m=min(g);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(g)-m);
  return shift(position+align*labelmargin(p))*t*shift(-a);
}

transform3 aligntransform(surface s, transform3 t=identity4, triple position,
                          triple align, pen p=currentpen)
{
  if(determinant(t) == 0) return identity4;
  triple m=min(s);
  triple dir=rectify(inverse(t)*-align);
  triple a=m+realmult(dir,max(s)-m);
  return shift(position+align*labelmargin(p))*t*shift(-a);
}

void label(frame f, Label L, triple position, align align=NoAlign,
           pen p=currentpen, light light=nolight,
           string name="", render render=defaultrender,
           interaction interaction=LabelInteraction(),
           projection P=currentprojection)
{
  Label L=L.copy();
  L.align(align);
  L.p(p);
  if(interaction.targetsize && settings.render != 0)
    L.T=L.T*scale(abs(P.camera-position)/abs(P.vector()));
  transform3 T=transform3(P);
  if(L.defaulttransform3)
    L.T3=T;

  if(is3D()) {
    bool lighton=light.on();
    if(name == "") name=L.s;
    if(prc() && interaction.type == Billboard.type) {
      surface s=surface(texpath(L));
      transform3 centering=L.align.is3D ?
        alignshift(s,L.T3,position,L.align.dir3) : identity4;
      transform3 positioning=
        shift(L.align.is3D ? position+L.align.dir3*labelmargin(L.p) : position);
      frame f1,f2,f3;
          begingroup3(f1,name,render);
          if(L.defaulttransform3)
            begingroup3(f3,render,position,interaction.type);
          else {
            begingroup3(f2,render,position,interaction.type);
            begingroup3(f3,render,position);
          }
      for(patch S : s.s) {
        S=centering*S;
        draw3D(f3,S,position,L.p,light,interaction);
        // Fill subdivision cracks
        if(render.labelfill && opacity(L.p) == 1 && !lighton)
          _draw(f3,S.external(),position,L.p,interaction.type);
      }
      endgroup3(f3);
          if(L.defaulttransform3)
            add(f1,T*f3);
          else {
            add(f2,inverse(T)*L.T3*f3);
            endgroup3(f2);
            add(f1,T*f2);
          }
      endgroup3(f1);
      add(f,positioning*f1);
    } else {
      begingroup3(f,name,render);
      for(patch S : surface(L,position).s) {
        triple V=L.align.is3D ? position+L.align.dir3*labelmargin(L.p) :
          position;
        draw3D(f,S,V,L.p,light,interaction);
        // Fill subdivision cracks
        if(render.labelfill && opacity(L.p) == 1 && !lighton)
          _draw(f,S.external(),V,L.p,interaction.type);
      }
      endgroup3(f);
    }
  } else {
    pen p=color(L.T3*Z,L.p,light,shiftless(P.T.modelview));
    if(L.defaulttransform3) {
      if(L.filltype == NoFill)
        fill(f,path(L,project(position,P.t),P),p);
      else {
        frame d;
        fill(d,path(L,project(position,P.t),P),p);
        add(f,d,L.filltype);
      }
    } else
      for(patch S : surface(L,position).s)
        fill(f,project(S.external(),P,1),p);
  }
}

void label(picture pic=currentpicture, Label L, triple position,
           align align=NoAlign, pen p=currentpen,
           light light=nolight, string name="",
           render render=defaultrender,
           interaction interaction=LabelInteraction())
{
  Label L=L.copy();
  L.align(align);
  L.p(p);
  L.position(0);
  
  pic.add(new void(frame f, transform3 t, picture pic2, projection P) {
      // Handle relative projected 3D alignments.
      Label L=L.copy();
      triple v=t*position;
      if(!align.is3D && L.align.relative && L.align.dir3 != O &&
         determinant(P.t) != 0)
        L.align(L.align.dir*unit(project(v+L.align.dir3,P.t)-project(v,P.t)));
      
      if(interaction.targetsize && settings.render != 0)
        L.T=L.T*scale(abs(P.camera-v)/abs(P.vector()));
      transform3 T=transform3(P);
      if(L.defaulttransform3)
        L.T3=T;

      if(is3D()) {
        bool lighton=light.on();
        if(name == "") name=L.s;
        if(prc() && interaction.type == Billboard.type) {
          surface s=surface(texpath(L,bbox=P.bboxonly));
          transform3 centering=L.align.is3D ?
            alignshift(s,L.T3,v,L.align.dir3) : identity4;
          transform3 positioning=
            shift(L.align.is3D ? v+L.align.dir3*labelmargin(L.p) : v);
          frame f1,f2,f3;
          begingroup3(f1,name,render);
          if(L.defaulttransform3)
            begingroup3(f3,render,v,interaction.type);
          else {
            begingroup3(f2,render,v,interaction.type);
            begingroup3(f3,render,v);
          }
          for(patch S : s.s) {
            S=centering*S;
            draw3D(f3,S,v,L.p,light,interaction);
            // Fill subdivision cracks
            if(render.labelfill && opacity(L.p) == 1 && !lighton)
              _draw(f3,S.external(),v,L.p,interaction.type);
          }
          endgroup3(f3);
          if(L.defaulttransform3)
            add(f1,T*f3);
          else {
            add(f2,inverse(T)*L.T3*f3);
            endgroup3(f2);
            add(f1,T*f2);
          }
          endgroup3(f1);
          add(f,positioning*f1);
        } else {
          begingroup3(f,name,render);
          for(patch S : surface(L,v,bbox=P.bboxonly).s) {
            triple V=L.align.is3D ? v+L.align.dir3*labelmargin(L.p) : v;
            draw3D(f,S,V,L.p,light,interaction);
            // Fill subdivision cracks
            if(render.labelfill && opacity(L.p) == 1 && !lighton)
              _draw(f,S.external(),V,L.p,interaction.type);
          }
          endgroup3(f);
        }
      }
      
      if(pic2 != null) {
        pen p=color(L.T3*Z,L.p,light,shiftless(P.T.modelview));
        if(L.defaulttransform3) {
          if(L.filltype == NoFill)
            fill(project(v,P.t),pic2,path(L,P),p);
          else {
            picture d;
            fill(project(v,P.t),d,path(L,P),p);
            add(pic2,d,L.filltype);
          }
        } else
          pic2.add(new void(frame f, transform T) {
              for(patch S : surface(L,v).s)
                fill(f,T*project(S.external(),P,1),p);
            });
      }
      
    },!L.defaulttransform3);

  Label L=L.copy();
  
  if(interaction.targetsize && settings.render != 0)
    L.T=L.T*scale(abs(currentprojection.camera-position)/
                  abs(currentprojection.vector()));
  path[] g=texpath(L,bbox=true);
  if(g.length == 0 || (g.length == 1 && size(g[0]) == 0)) return;
  if(L.defaulttransform3)
    L.T3=transform3(currentprojection);
  path3[] G=path3(g);
  G=L.align.is3D ? align(G,L.T3,O,L.align.dir3,L.p) : L.T3*G;
  pic.addBox(position,position,min(G),max(G));
}

void label(picture pic=currentpicture, Label L, path3 g, align align=NoAlign,
           pen p=currentpen, light light=nolight, string name="",
           interaction interaction=LabelInteraction())
{
  Label L=L.copy();
  L.align(align);
  L.p(p);
  bool relative=L.position.relative;
  real position=L.position.position.x;
  if(L.defaultposition) {relative=true; position=0.5;}
  if(relative) position=reltime(g,position);
  if(L.align.default) {
    align a;
    a.init(-I*(position <= sqrtEpsilon ? S :
               position >= length(g)-sqrtEpsilon ? N : E),relative=true);
    a.dir3=dir(g,position); // Pass 3D direction via unused field.
    L.align(a);             
  }
  label(pic,L,point(g,position),light,name,interaction);
}

surface extrude(Label L, triple axis=Z)
{
  Label L=L.copy();
  path[] g=texpath(L);
  surface S=extrude(g,axis);
  surface s=surface(g);
  S.append(s);
  S.append(shift(axis)*s);
  return S;
}

restricted surface nullsurface;

// Embed a Label onto a surface.
surface surface(Label L, surface s, real uoffset, real voffset,
                real height=0, bool bottom=true, bool top=true)
{
  int nu=s.index.length;
  int nv;
  if(nu == 0) nu=nv=1;
  else {
    nv=s.index[0].length;
    if(nv == 0) nv=1;
  }

  path[] g=texpath(L);
  pair m=min(g);
  pair M=max(g);
  pair lambda=inverse(L.T*scale(nu-epsilon,nv-epsilon))*(M-m);
  lambda=(abs(lambda.x),abs(lambda.y));
  path[] G=bezulate(g);

  path3 transpath(path p, real height) {
    return path3(unstraighten(p),new triple(pair z) {
        real u=uoffset+(z.x-m.x)/lambda.x;
        real v=voffset+(z.y-m.y)/lambda.y;
        if(((u < 0 || u >= nu) && !s.ucyclic()) ||
           ((v < 0 || v >= nv) && !s.vcyclic())) {
          warning("cannotfit","cannot fit string to surface");
          u=v=0;
        }
        return s.point(u,v)+height*unit(s.normal(u,v));
      });
  }

  surface s;
  for(path p : G) {
    for(path g : regularize(p)) {
      path3 b;
      bool extrude=height > 0;
      if(bottom || extrude) 
        b=transpath(g,0);
      if(bottom) s.s.push(patch(b));
      if(top || extrude) {
        path3 h=transpath(g,height);
        if(top) s.s.push(patch(h));
        if(extrude) s.append(extrude(b,h));
      }
    }
  }
  return s;
}

private real a=4/3*(sqrt(2)-1);
private transform3 t1=rotate(90,O,Z);
private transform3 t2=t1*t1;
private transform3 t3=t2*t1;
private transform3 i=xscale3(-1)*zscale3(-1);

restricted patch octant1=patch(X{Y}..{-X}Y{Z}..{-Y}Z..Z{X}..{-Z}cycle,
                               new triple[] {(1,a,a),(a,1,a),(a^2,a,1),
                                             (a,a^2,1)});

restricted surface unithemisphere=surface(octant1,t1*octant1,t2*octant1,
                                          t3*octant1);
restricted surface unitsphere=surface(octant1,t1*octant1,t2*octant1,t3*octant1,
                                      i*octant1,i*t1*octant1,i*t2*octant1,
                                      i*t3*octant1);

restricted patch unitfrustum(real t1, real t2)
{
  real s1=interp(t1,t2,1/3);
  real s2=interp(t1,t2,2/3);
  return patch(interp(Z,X,t2){Y}..{-X}interp(Z,Y,t2)--interp(Z,Y,t1){X}..{-Y}
               interp(Z,X,t1)--cycle,
               new triple[] {(s2,s2*a,1-s2),(s2*a,s2,1-s2),(s1*a,s1,1-s1),
                                          (s1,s1*a,1-s1)});
}

// Return a unitcone constructed from n frusta (the final one being degenerate)
surface unitcone(int n=6)
{
  surface unitcone;
  unitcone.s=new patch[4*n];
  real r=1/3;
  for(int i=0; i < n; ++i) {
    patch s=unitfrustum(i < n-1 ? r^(i+1) : 0,r^i);
    unitcone.s[i]=s;
    unitcone.s[n+i]=t1*s;
    unitcone.s[2n+i]=t2*s;
    unitcone.s[3n+i]=t3*s;
  }
  return unitcone;
}

restricted surface unitcone=unitcone();
restricted surface unitsolidcone=surface(patch(unitcircle3)...unitcone.s);

private patch unitcylinder1=patch(X{Y}..{-X}Y--Y+Z{X}..{-Y}X+Z--cycle);

restricted surface unitcylinder=surface(unitcylinder1,t1*unitcylinder1,
                                        t2*unitcylinder1,t3*unitcylinder1);

private patch unitplane=patch(new triple[] {O,X,X+Y,Y});
restricted surface unitcube=surface(reverse(unitplane),
                                    rotate(90,O,X)*unitplane,
                                    rotate(-90,O,Y)*unitplane,
                                    shift(Z)*unitplane,
                                    rotate(90,X,X+Y)*unitplane,
                                    rotate(-90,Y,X+Y)*unitplane);
restricted surface unitplane=surface(unitplane);
restricted surface unitdisk=surface(unitcircle3);

void dot(frame f, triple v, material p=currentpen,
         light light=nolight, string name="",
         render render=defaultrender, projection P=currentprojection)
{
  pen q=(pen) p;
  if(is3D()) {
    bool group=name != "" || render.defaultnames;
    if(group)
      begingroup3(f,name == "" ? "dot" : name,render);
    real size=0.5*linewidth(dotsize(q)+q);
    transform3 T=shift(v)*scale3(size);
    for(patch s : unitsphere.s)
      draw3D(f,T*s,v,p,light,prc=false);
    if(prc())
      drawPRCsphere(f,T,p,light);
    if(group)
      endgroup3(f);
  } else dot(f,project(v,P.t),q);
}

void dot(frame f, triple[] v, material p=currentpen, light light=nolight,
         string name="", render render=defaultrender,
         projection P=currentprojection)
{
  if(v.length > 0) {
    // Remove duplicate points.
    v=sort(v,lexorder);

    triple last=v[0];
    dot(f,last,p,light,name,render,P);
    for(int i=1; i < v.length; ++i) {
      triple V=v[i];
      if(V != last) {
        dot(f,V,p,light,name,render,P);
        last=V;
      }
    }
  }
}

void dot(frame f, path3 g, material p=currentpen, light light=nolight,
         string name="", render render=defaultrender,
         projection P=currentprojection)
{
  dot(f,sequence(new triple(int i) {return point(g,i);},size(g)),
      p,light,name,render,P);
}

void dot(frame f, path3[] g, material p=currentpen, light light=nolight,
         string name="", render render=defaultrender,
         projection P=currentprojection)
{
  int sum;
  for(path3 G : g)
    sum += size(G);
  int i,j;
  dot(f,sequence(new triple(int) {
        while(j >= size(g[i])) {
          ++i;
          j=0;
        }
        triple v=point(g[i],j);
        ++j;
        return v;
      },sum),p,light,name,render,P);
}

void dot(picture pic=currentpicture, triple v, material p=currentpen,
         light light=nolight, string name="", render render=defaultrender)
{
  pen q=(pen) p;
  real size=0.5*linewidth(dotsize(q)+q);
  pic.add(new void(frame f, transform3 t, picture pic, projection P) {
      triple V=t*v;
      if(is3D()) {
        bool group=name != "" || render.defaultnames;
        if(group)
          begingroup3(f,name == "" ? "dot" : name,render);
        transform3 T=shift(V)*scale3(size);
        for(patch s : unitsphere.s)
          draw3D(f,T*s,V,p,light,prc=false);
        if(prc())
          drawPRCsphere(f,T,p,light,render);
        if(group)
          endgroup3(f);
      }
      if(pic != null)
        dot(pic,project(V,P.t),q);
    },true);
  triple R=size*(1,1,1);
  pic.addBox(v,v,-R,R);
}

void dot(picture pic=currentpicture, triple[] v, material p=currentpen,
         light light=nolight, string name="", render render=defaultrender)
{
  if(v.length > 0) {
    // Remove duplicate points.
    v=sort(v,lexorder);

    triple last=v[0];
    bool group=name != "" || render.defaultnames;
    if(group)
      begingroup3(pic,name == "" ? "dots" : name,render);
    dot(pic,last,p,light,partname(0,render),render);
    int k=0;
    for(int i=1; i < v.length; ++i) {
      triple V=v[i];
      if(V != last) {
        dot(pic,V,p,light,partname(++k,render),render);
        last=V;
      }
    }
    if(group)
      endgroup3(pic);
  }
}

void dot(picture pic=currentpicture, explicit path3 g, material p=currentpen,
         light light=nolight, string name="",
         render render=defaultrender)
{
  dot(pic,sequence(new triple(int i) {return point(g,i);},size(g)),
      p,light,name,render);
}

void dot(picture pic=currentpicture, path3[] g, material p=currentpen,
         light light=nolight, string name="", render render=defaultrender)
{
  int sum;
  for(path3 G : g)
    sum += size(G);
  int i,j;
  dot(pic,sequence(new triple(int) {
        while(j >= size(g[i])) {
          ++i;
          j=0;
        }
        triple v=point(g[i],j);
        ++j;
        return v;
      },sum),p,light,name,render);
}

void dot(picture pic=currentpicture, Label L, triple v, align align=NoAlign,
         string format=defaultformat, material p=currentpen,
         light light=nolight, string name="", render render=defaultrender)
{
  Label L=L.copy();
  if(L.s == "") {
    if(format == "") format=defaultformat;
    L.s="("+format(format,v.x)+","+format(format,v.y)+","+
      format(format,v.z)+")";
  }
  L.align(align,E);
  L.p((pen) p);
  dot(pic,v,p,light,name,render);
  label(pic,L,v,render);
}

void pixel(picture pic=currentpicture, triple v, pen p=currentpen,
           real width=1)
{
  real h=0.5*width;
  pic.add(new void(frame f, transform3 t, picture pic, projection P) {
      triple V=t*v;
      if(is3D())
        drawpixel(f,V,p,width);
      if(pic != null) {
        triple R=h*unit(cross(unit(P.vector()),P.up));
        pair z=project(V,P.t);
        real h=0.5*abs(project(V+R,P.t)-project(V-R,P.t));
        pair r=h*(1,1)/mm;
        fill(pic,box(z-r,z+r),p,false);
      }
    },true);
  triple R=h*(1,1,1);
  pic.addBox(v,v,-R,R);
}

pair minbound(triple[] A, projection P)
{
  pair b=project(A[0],P);
  for(triple v : A)
      b=minbound(b,project(v,P.t));
  return b;
}

pair maxbound(triple[] A, projection P)
{
  pair b=project(A[0],P);
  for(triple v : A)
    b=maxbound(b,project(v,P.t));
  return b;
}

pair minbound(triple[][] A, projection P)
{
  pair b=project(A[0][0],P);
  for(triple[] a : A) {
    for(triple v : a) {
      b=minbound(b,project(v,P.t));
    }
  }
  return b;
}

pair maxbound(triple[][] A, projection P)
{
  pair b=project(A[0][0],P);
  for(triple[] a : A) {
    for(triple v : a) {
      b=maxbound(b,project(v,P.t));
    }
  }
  return b;
}

triple[][] operator / (triple[][] a, real[][] b) 
{
  triple[][] A=new triple[a.length][];
  for(int i=0; i < a.length; ++i) {
    triple[] ai=a[i];
    real[] bi=b[i];
    A[i]=sequence(new triple(int j) {return ai[j]/bi[j];},ai.length);
  }
  return A;
}

// Draw a NURBS curve.
void draw(picture pic=currentpicture, triple[] P, real[] knot,
          real[] weights=new real[], pen p=currentpen, string name="",
          render render=defaultrender)
{
  P=copy(P);
  knot=copy(knot);
  weights=copy(weights);
  pic.add(new void(frame f, transform3 t, picture pic, projection Q) {
      if(is3D()) {
        triple[] P=t*P;
        bool group=name != "" || render.defaultnames;
        if(group)
          begingroup3(f,name == "" ? "curve" : name,render);
        draw(f,P,knot,weights,p);
        if(group)
          endgroup3(f);
        if(pic != null)
          pic.addBox(minbound(P,Q),maxbound(P,Q));
      }
    },true);
  pic.addBox(minbound(P),maxbound(P));
}

// Draw a NURBS surface.
void draw(picture pic=currentpicture, triple[][] P, real[] uknot, real[] vknot,
          real[][] weights=new real[][], material m=currentpen,
          pen[] colors=new pen[], light light=currentlight, string name="",
          render render=defaultrender)
{
  if(colors.length > 0)
    m=mean(colors);
  m=material(m,light);
  bool lighton=light.on();
  P=copy(P);
  uknot=copy(uknot);
  vknot=copy(vknot);
  weights=copy(weights);
  colors=copy(colors);
  pic.add(new void(frame f, transform3 t, picture pic, projection Q) {
      if(is3D()) {
        bool group=name != "" || render.defaultnames;
        if(group)
          begingroup3(f,name == "" ? "surface" : name,render);
        triple[][] P=t*P;
        real PRCshininess;
        if(prc())
          PRCshininess=PRCshininess(m.shininess);
        draw(f,P,uknot,vknot,weights,m.p,m.opacity,m.shininess,PRCshininess,
             colors);
        if(group)
          endgroup3(f);
        if(pic != null)
          pic.addBox(minbound(P,Q),maxbound(P,Q));
      }
    },true);
  pic.addBox(minbound(P),maxbound(P));
}
