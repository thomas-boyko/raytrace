#include "Scene.h"
#include "Log.h"
#include <algorithm>
#include <cmath>
#include <limits>

const float c = sqrt(5) / 2.f;

// Shared vertex grid — unique positions indexed by (row, col)
static std::vector<vec3> floorGrid;

static int gridCols = 0;
static int gridRows = 0;
static float gridSize = 0.0f;
static float gridOriginX = 0.0f;
static float gridOriginZ = 0.0f;
static float gridY = 0.0f;

void initFloorGrid(float size, int cols, int rows, float originX, float originZ,
                   float y) {
  gridSize = size;
  gridCols = cols;
  gridRows = rows;
  gridOriginX = originX;
  gridOriginZ = originZ;
  gridY = y;

  float h = size * (sqrt(3.0f) / 2.0f);
  floorGrid.resize((cols + 1) * (rows + 1));

  for (int row = 0; row <= rows; row++) {
    for (int col = 0; col <= cols; col++) {
      float x = originX + col * size;
      float z = originZ + row * h;
      floorGrid[row * (cols + 1) + col] = vec3(x, y, z);
    }
  }
}

std::vector<vec3> buildFloorVerts(float t) {
  float h = gridSize * (sqrt(3.0f) / 2.0f);
  int stride = gridCols + 1;

  for (int row = 0; row <= gridRows; row++) {
    for (int col = 0; col <= gridCols; col++) {
      float x = gridOriginX + col * gridSize;
      float z = gridOriginZ + row * h;
      float y = gridY + 0.1f * sin(t + col * 0.5f + row * 0.5f);
      floorGrid[row * stride + col] = vec3(x, y, z);
    }
  }

  std::vector<vec3> verts;
  for (int row = 0; row < gridRows; row++) {
    for (int col = 0; col < 2 * gridCols - 1; col++) {
      int c = col / 2;
      int tl = row * stride + c;
      int tr = tl + 1;
      int bl = (row + 1) * stride + c;
      int br = bl + 1;

      if (col % 2 == 0) {
        verts.push_back(floorGrid[tl]);
        verts.push_back(floorGrid[bl]);
        verts.push_back(floorGrid[br]);
      } else {
        verts.push_back(floorGrid[tl]);
        verts.push_back(floorGrid[br]);
        verts.push_back(floorGrid[tr]);
      }
    }
  }
  return verts;
}
std::vector<glm::vec3> starPos = {
    {-9.0f, 8.5f, -20.f},  {-6.0f, 8.8f, -20.f},  {-3.0f, 8.2f, -20.f},
    {0.0f, 8.7f, -20.f},   {3.0f, 8.4f, -20.f},   {6.0f, 8.9f, -20.f},
    {9.0f, 8.3f, -20.f},   {-7.5f, 9.2f, -20.f},  {-1.5f, 9.0f, -20.f},
    {4.5f, 9.1f, -20.f},   {7.5f, 8.0f, -20.f},   {-9.0f, -4.2f, -20.f},
    {-6.5f, -3.8f, -20.f}, {-4.0f, -4.5f, -20.f}, {-1.5f, -3.6f, -20.f},
    {0.5f, -4.3f, -20.f},  {2.5f, -3.7f, -20.f},  {5.0f, -4.4f, -20.f},
    {7.0f, -3.5f, -20.f},  {9.0f, -4.1f, -20.f},  {-8.0f, -3.2f, -20.f},
    {4.0f, -3.0f, -20.f},  {1.0f, 1.0f, -20.f},   {5.0f, 3.0f, -20.f},
    {3.0f, 1.0f, -20.f},   {1.0f, 3.0f, -20.f},   {3.0f, 6.0f, -20.f},
    {-3.0f, 3.0f, -20.f},  {-1.0f, -2.0f, -20.f}, {-8.0f, 7.5f, -20.f},
    {-4.5f, 7.0f, -20.f},  {-1.5f, 7.8f, -20.f},  {2.0f, 7.2f, -20.f},
    {5.5f, 7.6f, -20.f},   {8.5f, 6.8f, -20.f},   {0.5f, 6.5f, -20.f},
    {-6.5f, 6.2f, -20.f},  {-9.0f, 5.0f, -20.f},  {-7.0f, 4.2f, -20.f},
    {-5.0f, 5.5f, -20.f},  {-2.0f, 4.8f, -20.f},  {0.0f, 5.2f, -20.f},
    {2.5f, 4.5f, -20.f},   {4.5f, 5.8f, -20.f},   {6.5f, 4.0f, -20.f},
    {8.0f, 5.5f, -20.f},   {9.5f, 3.5f, -20.f},   {-9.5f, 2.0f, -20.f},
    {-7.5f, 0.5f, -20.f},  {-5.5f, 2.5f, -20.f},  {-4.0f, 0.8f, -20.f},
    {-2.5f, 1.5f, -20.f},  {0.0f, 0.5f, -20.f},   {2.0f, 2.5f, -20.f},
    {4.0f, 1.0f, -20.f},   {6.0f, 2.8f, -20.f},   {7.5f, 0.8f, -20.f},
    {9.5f, 1.5f, -20.f},   {-9.0f, -1.0f, -20.f}, {-7.0f, -2.5f, -20.f},
    {-5.0f, -1.5f, -20.f}, {-3.0f, -3.0f, -20.f}, {-1.0f, -0.5f, -20.f},
    {1.5f, -1.5f, -20.f},  {3.5f, -3.5f, -20.f},  {5.5f, -1.0f, -20.f},
    {7.5f, -2.0f, -20.f},  {9.0f, -3.5f, -20.f},  {-7.0f, -5.0f, -20.f},
    {-3.5f, -5.2f, -20.f}, {0.0f, -4.8f, -20.f},  {3.5f, -5.1f, -20.f},
    {7.0f, -4.9f, -20.f},  {-5.5f, -4.6f, -20.f}, {2.0f, -5.3f, -20.f},
};

Scene initScene1(float t) {
  Scene scene1;

  ObjectMaterial gunmetal;
  gunmetal.diffuse = glm::vec3(0.3, 0.3, 0.3);
  gunmetal.specular = gunmetal.diffuse;
  gunmetal.reflectionStrength = vec3(0.4);
  gunmetal.ambient = 0.1f * gunmetal.diffuse;
  gunmetal.specularCoefficient = 128;

  ObjectMaterial water;
  water.diffuse = vec3(0.05f, 0.15f, 0.3f);
  water.ambient = 0.05f * water.diffuse;
  water.specular = vec3(0.8f, 0.85f, 0.9f);
  water.specularCoefficient = 256;
  water.reflectionStrength = vec3(0.5f, 0.55f, 0.6f);

  ObjectMaterial star;
  star.diffuse = glm::vec3(1);
  star.ambient = star.diffuse;
  star.specular = glm::vec3(0);

  ObjectMaterial sunMat;
  sunMat.diffuse = vec3(1.0f, 0.9f, 0.1f);
  sunMat.ambient = sunMat.diffuse;
  sunMat.specular = vec3(0);

  ObjectMaterial earthMat;
  earthMat.diffuse = vec3(0.1f, 0.4f, 0.9f);
  earthMat.ambient = 0.1f * earthMat.diffuse;
  earthMat.specular = 0.5f * earthMat.diffuse;
  earthMat.specularCoefficient = 64;

  ObjectMaterial moonMat;
  moonMat.diffuse = vec3(0.7f, 0.7f, 0.7f);
  moonMat.ambient = 0.1f * moonMat.diffuse;
  moonMat.specular = 0.3f * moonMat.diffuse;
  moonMat.specularCoefficient = 32;

  ObjectMaterial marsMat;
  marsMat.diffuse = vec3(0.8f, 0.3f, 0.1f);
  marsMat.ambient = 0.1f * marsMat.diffuse;
  marsMat.specular = 0.3f * marsMat.diffuse;
  marsMat.specularCoefficient = 32;

  vec3 sunPos = vec3(0.0f, 1.5f, -9.5f);

  float earthOrbitRadius = 1.f;
  float earthSpeed = 1.0f;
  vec3 earthPos = sunPos + vec3(earthOrbitRadius * cos(t * earthSpeed), 0.0f,
                                earthOrbitRadius * sin(t * earthSpeed));

  float moonOrbitRadius = 0.5f;
  float moonSpeed = 4.0f;
  vec3 moonPos = earthPos + vec3(moonOrbitRadius * cos(t * moonSpeed), 0.1f,
                                 moonOrbitRadius * sin(t * moonSpeed));

  float marsOrbitRadius = 2.0f;
  float marsSpeed = 0.5f;
  vec3 marsPos =
      sunPos + vec3(marsOrbitRadius * cos(t * marsSpeed + 1.0f), 0.0f,
                    marsOrbitRadius * sin(t * marsSpeed + 1.0f));

  std::shared_ptr<Sphere> sun = std::make_shared<Sphere>(sunPos, 0.45f, 1);
  sun->material = sunMat;
  scene1.shapesInScene.push_back(sun);

  std::shared_ptr<Sphere> earth = std::make_shared<Sphere>(earthPos, 0.18f, 2);
  earth->material = earthMat;
  scene1.shapesInScene.push_back(earth);

  std::shared_ptr<Sphere> moon = std::make_shared<Sphere>(moonPos, 0.07f, 3);
  moon->material = moonMat;
  scene1.shapesInScene.push_back(moon);

  std::shared_ptr<Sphere> mars = std::make_shared<Sphere>(marsPos, 0.14f, 4);
  mars->material = marsMat;
  scene1.shapesInScene.push_back(mars);

  static bool gridInitialized = false;
  if (!gridInitialized) {
    initFloorGrid(0.41f, 15, 15, -3, -5.0f, -1);
    gridInitialized = true;
  }
  auto floorVerts = buildFloorVerts(t);
  std::shared_ptr<Triangles> floorWall = std::make_shared<Triangles>();
  floorWall->initTriangles(floorVerts.size() / 3, floorVerts.data(), 5);
  floorWall->material = water;
  scene1.shapesInScene.push_back(floorWall);

  int i = 6;
  for (glm::vec3 p : starPos) {
    std::shared_ptr s = std::make_shared<Sphere>(p, 0.05f, i);
    s->material = star;

    scene1.shapesInScene.push_back(s);
    i++;
  }

  scene1.lightPosition = vec3(0.0f, 2.5f, -9.5f);
  scene1.lightColor = vec3(1, 0.8f, 0.8f);
  scene1.ambientFactor = 0.1f;
  return scene1;
}
