#include <iostream>
#include <vector>
#include <map>
using namespace std;

#include "windowFramework.h"
#include "pandaFramework.h"
#include "pandaSystem.h"

#include "geomVertexData.h"
#include "geomVertexFormat.h"
#include "geomVertexWriter.h"
#include "geom.h"
#include "geomTriangles.h"
#include "geomNode.h"

#include "stackedPerlinNoise2.h"

#include "texture.h"
#include "texturePool.h"

#include "genericAsyncTask.h"
#include "asyncTaskManager.h"

const vector<vector<float>> cube_vertices_deltas = {
  {-0.5f, -0.5f, 0},
  { 0.5f, -0.5f, 0},
  { 0.5f,  0.5f, 0},
  {-0.5f,  0.5f, 0},
  {-0.5f, -0.5f, 1},
  { 0.5f, -0.5f, 1},
  { 0.5f,  0.5f, 1},
  {-0.5f,  0.5f, 1},
};

const vector<vector<int>> cube_quads = {
  {0, 1, 5, 4},
  {1, 2, 6, 5},
  {2, 3, 7, 6},
  {3, 0, 4, 7},
  {4, 5, 6, 7},
  {3, 2, 1, 0},
};

const vector<vector<int>> cube_normals = {
  { 0, -1,  0},
  { 1,  0,  0},
  { 0,  1,  0},
  {-1,  0,  0},
  { 0,  0,  1},
  { 0,  0, -1},
};

const vector<vector<pair<float, float>>> cube_uvs = {
  {{0, 0.5f}, {0.25f, 0.5f}, {0.25f, 1}, {0, 1}},
  {{0.25f, 0.5f}, {0.5f, 0.5f}, {0.5f, 1}, {0.25f, 1}},
  {{0.5f, 0.5f}, {0.75f, 0.5f}, {0.75f, 1}, {0.5f, 1}},
  {{0.75f, 0.5f}, {1, 0.5f}, {1, 1}, {0.75f, 1}},
  {{0, 0}, {0.25f, 0}, {0.25f, 0.5f}, {0, 0.5f}},
  {{0.25f, 0}, {0.5f, 0}, {0.5f, 0.5f}, {0.25f, 0.5f}}
};

map<string, bool> keys;
PT(AsyncTaskManager) taskMgr = AsyncTaskManager::get_global_ptr();

void put_quad(PT(GeomTriangles) gt, int v0, int v1, int v2, int v3);

NodePath create_cube(float x, float y, float z);

void update_key(const Event* event, void* data);
AsyncTask::DoneStatus move_player_task(GenericAsyncTask *task, void *camera_ptr);

int main(int argc, char *argv[]) {
  PandaFramework framework;
  framework.open_framework(argc, argv);
  framework.set_window_title("M-Ian-Craft");
  
  WindowFramework *window = framework.open_window();
  
  StackedPerlinNoise2 noise(0.3f, 0.3f, 3, 2.0f, 0.5f, 256);
  
  PT(Texture) tex = TexturePool::load_texture("legacy/assets/textures/grass.png");
  tex->set_magfilter(SamplerState::FT_nearest);
  tex->set_minfilter(SamplerState::FT_nearest);
  
  int s = 100;
  for (int x = -s; x <= s; x++) {
    for (int y = -s; y <= s; y++) {
      NodePath cube = create_cube(x, y, (int)(noise.noise(x * 0.01f, y * 0.01f) * 7.5f));
      cube.set_texture(tex);
      cube.reparent_to(window->get_render());
      // cube.set_scale(2.0f);
    }
  }
  
  keys = {
    {"w", false},
    {"a", false},
    {"s", false},
    {"d", false},
    {"space", false},
    {"shift", false},
  };

  window->enable_keyboard();
  NodePath camera = window->get_camera_group();
  camera.set_pos(0, 0, 25);
  for (auto &key_pair: keys) {
    // cout << "defining key: " << key_pair.first << endl;
    framework.define_key(key_pair.first,
      key_pair.first,
      update_key,
      nullptr);
    framework.define_key(key_pair.first + "-up",
      key_pair.first,
      update_key,
      nullptr);
  }

  taskMgr->add(new GenericAsyncTask("move_player_task", move_player_task, &camera));

  framework.main_loop();
  framework.close_framework();
  
  return 0;
}

void put_quad(PT(GeomTriangles) gt, int v0, int v1, int v2, int v3) {
  gt->add_vertices(v0, v1, v2);
  gt->add_vertices(v0, v2, v3);
}

NodePath create_cube(float x, float y, float z) {
  PT(GeomVertexData) vdata = new GeomVertexData(
    "cube", GeomVertexFormat::get_v3n3t2(), Geom::UH_static);
  vdata->set_num_rows(24);

  GeomVertexWriter vertex(vdata, "vertex");
  GeomVertexWriter normal(vdata, "normal");
  GeomVertexWriter texcoord(vdata, "texcoord");

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
    for (auto &uv: cube_uvs[i]) {
      texcoord.add_data2(LVecBase2(uv.first, uv.second));
    }
  }
  tris->close_primitive();

  PT(Geom) geom = new Geom(vdata);
  geom->add_primitive(tris);

  PT(GeomNode) node = new GeomNode("cube");
  node->add_geom(geom);

  return NodePath(node);
}

void update_key(const Event* event, void* data) {
  // only works for one character keys
  string key = event->get_name();
  int idx = key.find("-up");
  if (idx != string::npos)
    key = key.substr(0, idx);
  keys[key] = (idx == string::npos);
}

AsyncTask::DoneStatus move_player_task(GenericAsyncTask *task, void *camera_ptr) {
  // move the player
  NodePath *camera = static_cast<NodePath *>(camera_ptr);
  LVecBase3 pos = camera->get_pos();
  float speed = 1.0f;
  if (keys["w"])
    camera->set_y(camera->get_y() + speed);
    if (keys["a"])
      camera->set_x(camera->get_x() - speed);
  if (keys["s"])
    camera->set_y(camera->get_y() - speed);
  if (keys["d"])
    camera->set_x(camera->get_x() + speed);
  if (keys["space"])
    camera->set_z(camera->get_z() + speed);
  if (keys["shift"])
    camera->set_z(camera->get_z() - speed);
  return AsyncTask::DS_cont;
}