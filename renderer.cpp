#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<cmath>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1280

struct Vector2f{
  float x, y;
  Vector2f(float a=0, float b=0) : x(a), y(b) {}

  float& operator[](size_t index) {
    if (index > 1) throw std::out_of_range("Index out of range");
    return (index == 0) ? x : y;
  }
};

struct Vector3f {
  float x, y, z;
  Vector3f(float a = 0, float b = 0, float c = 0) : x(a), y(b), z(c) {}
  
  float& operator[](size_t index) {
    if (index > 2) throw std::out_of_range("Index out of range");
    return (index == 0) ? x : (index == 1) ? y : z;
  }

  Vector3f normalize(){
    float magnitude = sqrtf(x * x + y * y + z * z);
    if (magnitude == 0) return *this;
    float x2 = x / magnitude;
    float y2 = y / magnitude;
    float z2 = z / magnitude;
    return Vector3f(x2, y2, z2);
  }

  float magnitude(){
    return sqrtf(x * x + y * y + z * z);
  }

  Vector3f operator- (Vector3f sub){
    float x2 = x - sub[0];
    float y2 = y - sub[1];
    float z2 = z - sub[2];
    return Vector3f(x2, y2, z2);
  }

  Vector3f operator+ (Vector3f sub){
    float x2 = x + sub[0];
    float y2 = y + sub[1];
    float z2 = z + sub[2];
    return Vector3f(x2, y2, z2);
  }

  Vector3f cross(Vector3f b){
    return Vector3f((y * b[2]) - (z * b[1]), (z * b[0]) - (x * b[2]), (x * b[1]) - (y * b[0]));
  }

  float dot (Vector3f b){
    return (x * b[0] + y * b[1] + z * b[2]);
  }

  Vector3f operator* (float multiplier){
    return Vector3f(x * multiplier, y * multiplier, z * multiplier);
  }

  Vector3f operator-() const{
    return Vector3f(-x, -y, -z);
  }



};

Vector3f reflect (Vector3f I, Vector3f N){
  return (I - N * I.dot(N) * 2.f);
}

struct Light {
  Light(const Vector3f &p, const float &i) : position(p), intensity(i) {}
  Vector3f position;
  float intensity;
};

struct Material{
  Vector3f color;
  Vector2f albedo;
  float specular_exponent;

  Material(Vector3f c, float s, Vector2f a) : color(c), specular_exponent(s),albedo(a){}

  Material() {};
};

struct HitInfo{
  Vector3f position, normal;
  float distance;
  Material material;
  bool didHit;
  
  HitInfo() {};

};

struct tri{
  Vector3f a, b, c, E1, E2;
  Material material;

  tri (Vector3f x, Vector3f y, Vector3f z, Material m) : a(x), b(y), c(z), material(m){
    E1 = b - a;
    E2 = c - a;
  }

  HitInfo intersectsRay(Vector3f &orig, Vector3f &dir){
    
    HitInfo hitInfo;
    hitInfo.material = material;

    Vector3f h = dir.cross(E2);
    float det = E1.dot(h);

    if (std::abs(det) < 1e-8) {
      hitInfo.didHit = false;
      return hitInfo;
    }

    float invDet = 1.0f / det;

    Vector3f s = orig - a;
    float u = invDet * s.dot(h);

    if (u < 0.0f || u > 1.0f) {
      hitInfo.didHit = false;
      return hitInfo;
    }

    Vector3f q = s.cross(E1);
    float v = invDet * dir.dot(q);

    if (v < 0.0f || u + v > 1.0f) {
      hitInfo.didHit = false;
      return hitInfo;
    }

    float t = invDet * E2.dot(q);
    
    if (t <= 0){
      hitInfo.didHit = false;
    };
    hitInfo.didHit = true;
    hitInfo.distance = t;
    hitInfo.normal = E2.cross(E1).normalize();
    hitInfo.position = orig + (dir * t);
    return hitInfo;
  }
};

HitInfo sceneHit(Vector3f &orig, Vector3f &dir, std::vector<tri> tris){
  HitInfo closest;
  closest.distance = MAXFLOAT;
  for(int i = 0; i < tris.size(); i++){
    HitInfo current = tris[i].intersectsRay(orig, dir);
    if(current.didHit && current.distance < closest.distance){ 
      closest = current;
    }
  }
  return closest;
}

Vector3f castRay (Vector3f &orig, Vector3f &dir, std::vector<tri> &tris, std::vector<Light> &lights){
  HitInfo closestTri;
  closestTri.distance = 999999.f;
  closestTri.didHit = false;
  for (tri triangle : tris){
    HitInfo hitInfo = triangle.intersectsRay(orig, dir);

    if (hitInfo.didHit && hitInfo.distance < closestTri.distance){
      closestTri = hitInfo;
    }
  }

  if (!closestTri.didHit){
    return Vector3f(0.2, 0.7, 0.8);
  }

  float diffuse_light_intensity = 0, specular_light_intensity = 0;

  for (size_t i = 0; i < lights.size(); i++){
    Vector3f light_dir = (lights[i].position - closestTri.position).normalize();
    diffuse_light_intensity += lights[i].intensity * std::max(0.f, closestTri.normal.dot(light_dir));

    specular_light_intensity += powf(std::max(0.f, -reflect(-light_dir, closestTri.normal).dot(dir)), closestTri.material.specular_exponent)*lights[i].intensity;

    float light_distance = (lights[i].position - closestTri.position).magnitude();
    Vector3f shadow_orig = light_dir.dot(closestTri.normal) < 0 ? closestTri.position - closestTri.normal * 1e-3 : closestTri.position + closestTri.normal*1e-3;
  
    HitInfo hitInfo = sceneHit(shadow_orig, light_dir, tris);
    if (hitInfo.didHit && (hitInfo.position - shadow_orig).magnitude() < light_distance){
      continue;
    }
  }

  return closestTri.material.color * diffuse_light_intensity + Vector3f(1,1,1) * specular_light_intensity;
  
}

void render(std::vector<tri> tris, std::vector<Light> lights){
  const int width = SCREEN_WIDTH;
  const int height = SCREEN_HEIGHT;
  Vector3f cameraPos = Vector3f(0,0,0);
  const float fov = (120.f/180.f) * M_PI;

  std::vector<Vector3f> framebuffer(width*height);

  for (size_t j = 0; j < height; j++){
    for (size_t i = 0; i < width; i++){
      float x =  (2*(i + 0.5)/(float)width  - 1)*tan(fov/2.)*width/(float)height;
      float y = -(2*(j + 0.5)/(float)height - 1)*tan(fov/2.);
      Vector3f dir = Vector3f(x, y, -1).normalize();
      framebuffer[i+j*width] = castRay(cameraPos, dir, tris, lights);
    }
  }

  std::ofstream ofs;
  ofs.open("./out.ppm");
  ofs << "P6\n" << width << " " << height << "\n255\n";

  for (size_t i = 0; i < height * width; i++){
    for (size_t j = 0; j< 3; j++){
      ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
    }
  }


}

int main(){

  std::vector<tri> triangles;
  
  triangles.push_back(tri(Vector3f(8.0,-1.5,-4.9), Vector3f(-1,1.5,-5), Vector3f(1,0,-5), Material(Vector3f(1,0,0), Vector2f(0,0)))); // Red Test Triangle
  triangles.push_back(tri(Vector3f(8.0,1.5,-5), Vector3f(-1,1.5,-5), Vector3f(1,0,-5), Material(Vector3f(0,1,0), Vector2f(1,1)))); // Green Test Triangle

  std::vector<Light> lights;

  lights.push_back(Light(Vector3f( 30, 50, -25),0.7f));

  render(triangles, lights);
  return 0;
}