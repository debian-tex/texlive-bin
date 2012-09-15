struct material {
  pen[] p; // diffusepen,ambientpen,emissivepen,specularpen
  real opacity;
  real shininess;  
  void operator init(pen diffusepen=black, pen ambientpen=black,
                     pen emissivepen=black, pen specularpen=mediumgray,
                     real opacity=opacity(diffusepen),
                     real shininess=defaultshininess) {
    p=new pen[] {diffusepen,ambientpen,emissivepen,specularpen};
    this.opacity=opacity;
    this.shininess=shininess;
  }
  void operator init(material m) {
    p=copy(m.p);
    opacity=m.opacity;
    shininess=m.shininess;
  }
  pen diffuse() {return p[0];}
  pen ambient() {return p[1];}
  pen emissive() {return p[2];}
  pen specular() {return p[3];}

  void diffuse(pen q) {p[0]=q;}
  void ambient(pen q) {p[1]=q;}
  void emissive(pen q) {p[2]=q;}
  void specular(pen q) {p[3]=q;}
}

void write(file file, string s="", material x, suffix suffix=none)
{
  write(file,s);
  write(file,"{");
  write(file,"diffuse=",x.diffuse());
  write(file,", ambient=",x.ambient());
  write(file,", emissive=",x.emissive());
  write(file,", specular=",x.specular());
  write(file,", opacity=",x.opacity);
  write(file,", shininess=",x.shininess);
  write(file,"}",suffix);
}

void write(string s="", material x, suffix suffix=endl)
{
  write(stdout,s,x,suffix);
}
  
bool operator == (material m, material n)
{
  return all(m.p == n.p) && m.opacity == n.opacity &&
  m.shininess == n.shininess;
}

material operator cast(pen p)
{
  return material(p);
}

material[] operator cast(pen[] p)
{
  return sequence(new material(int i) {return p[i];},p.length);
}

pen operator ecast(material m)
{
  return m.p.length > 0 ? m.diffuse() : nullpen;
}

material emissive(material m)
{
  return material(black+opacity(m.opacity),black,m.diffuse(),black,m.opacity,1);
}

pen color(triple normal, material m, light light, transform3 T=light.T) {
  triple[] position=light.position;
  if(invisible((pen) m)) return invisible;
  if(position.length == 0) return m.diffuse();
  normal=unit(T*normal);
  if(settings.twosided) normal *= sgn(normal.z);
  real s=m.shininess*128;
  real[] Diffuse=rgba(m.diffuse());
  real[] Ambient=rgba(m.ambient());
  real[] Specular=rgba(m.specular());
  real[] p=rgba(m.emissive());
  for(int i=0; i < position.length; ++i) {
    triple L=light.viewport ? position[i] : T*position[i];
    real Ldotn=max(dot(normal,L),0);
    p += light.ambient[i]*Ambient+Ldotn*light.diffuse[i]*Diffuse;
    // Apply specularfactor to partially compensate non-pixel-based rendering.
    if(Ldotn > 0) // Phong-Blinn model of specular reflection
      p += dot(normal,unit(L+Z))^s*light.specularfactor*
        light.specular[i]*Specular;
  }
  return rgb(p[0],p[1],p[2])+opacity(opacity(m.diffuse()));
}

light operator * (transform3 t, light light)
{
  light light=light(light);
  if(!light.viewport) light.position=shiftless(t)*light.position;
  return light;
}

light operator cast(triple v) {return light(v);}

light Viewport=light(ambient=gray(0.1),specularfactor=3,viewport=true,
                     (0.25,-0.25,1));

light White=light(new pen[] {rgb(0.38,0.38,0.45),rgb(0.6,0.6,0.67),
                             rgb(0.5,0.5,0.57)},specularfactor=3,
  new triple[] {(-2,-1.5,-0.5),(2,1.1,-2.5),(-0.5,0,2)});

light Headlamp=light(gray(0.8),ambient=gray(0.1),specular=gray(0.7),
                     specularfactor=3,viewport=true,dir(42,48));

currentlight=Headlamp;

light nolight;
