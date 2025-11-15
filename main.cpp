#include <iostream>
#include <vector>
using namespace std;

#include "pandaFramework.h"
#include "pandaSystem.h"

#include "geomVertexData.h"
#include "geomVertexFormat.h"
#include "geomVertexWriter.h"
#include "geom.h"
#include "geomTriangles.h"
#include "geomNode.h"

#include "perlinNoise2.h"
#include "stackedPerlinNoise2.h"

// #include "texture.h"
// #include "texturePool.h"

const vector<vector<float>> cube_vertices_deltas = {
  {-0.5f, -0.5f, 0},
  { 0.5f, -0.5f, 0},
  {-0.5f,  0.5f, 0},
  { 0.5f,  0.5f, 0},
  {-0.5f, -0.5f, 1},
  { 0.5f, -0.5f, 1},
  {-0.5f,  0.5f, 1},
  { 0.5f,  0.5f, 1},
};

const vector<vector<int>> cube_quads = {
  {4, 5, 1, 0},
  {6, 7, 5, 4},
  {2, 3, 7, 6},
  {0, 1, 3, 2},
  {5, 7, 3, 1},
  {6, 4, 0, 2},
};

const vector<vector<int>> cube_normals = {
  { 0,  0, -1},
  { 0,  0,  1},
  {-1,  0,  0},
  { 1,  0,  0},
  { 0, -1,  0},
  { 0,  1,  0},
};

void put_quad(PT(GeomTriangles) gt, int v0, int v1, int v2, int v3) {
  gt->add_vertices(v0, v1, v2);
  gt->add_vertices(v0, v2, v3);
}

NodePath create_cube(float x, float y, float z) {
  PT(GeomVertexData) vdata = new GeomVertexData(
    "cube", GeomVertexFormat::get_v3n3(), Geom::UH_static);
  vdata->set_num_rows(8);

  GeomVertexWriter vertex(vdata, "vertex");
  GeomVertexWriter normal(vdata, "normal");

  PT(GeomTriangles) tris = new GeomTriangles(Geom::UH_static);
  for (int i = 0; i < 6; i++) {
    for (int idx: cube_quads[i]) {
      float dx = cube_vertices_deltas[idx][0];
      float dy = cube_vertices_deltas[idx][1];
      float dz = cube_vertices_deltas[idx][2];
      vertex.add_data3(LVecBase3(x + dx, y + dy, z + dz));
      normal.add_data3(LVecBase3(cube_normals[i][0], cube_normals[i][1], cube_normals[i][2]));
      put_quad(tris, (i + 1) * 4 - 4,
                     (i + 1) * 4 - 3,
                     (i + 1) * 4 - 2,
                     (i + 1) * 4 - 1);
    }
  }
  tris->close_primitive();

  PT(Geom) geom = new Geom(vdata);
  geom->add_primitive(tris);

  PT(GeomNode) node = new GeomNode("cube");
  node->add_geom(geom);

  return NodePath(node);
}

int main(int argc, char *argv[]) {
    PandaFramework framework;
    framework.open_framework(argc, argv);
    framework.set_window_title("Procedural Cube");
    
    WindowFramework *window = framework.open_window();
    window->setup_trackball();
    
    // PerlinNoise2 noise(0.02f, 0.02f, 256, 69);
    StackedPerlinNoise2 noise(0.01f, 0.01f, 2, 2.0f, 0.5f, 256);

    // PT(Texture) tex = TexturePool::load_texture("legacy/assets/textures/grass.png");

    // NodePath cube = create_cube(0, 0, 0);
    // cube.reparent_to(window->get_render());
    // cube.set_pos(0, 10, 0);
    // cube.set_scale(2.0f);

    for (int x = -10; x <= 10; x++) {
      for (int y = -10; y <= 10; y++) {
        NodePath cube = create_cube(x, y, (noise.noise(x, y) * 1.0f));
        // cube.set_texture(tex);
        cube.reparent_to(window->get_render());
        cube.set_scale(2.0f);
      }
    }

    framework.main_loop();
    framework.close_framework();
    
    return 0;
}