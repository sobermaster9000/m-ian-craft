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

  for (float dx: {-0.5f, 0.5f}) {
    for (float dy: {-0.5f, 0.5f}) {
      for (float dz: {-0.5f, 0.5f}) {
        vertex.add_data3(LVecBase3(x + dx, y + dy, z + dz));
        normal.add_data3(LVecBase3(dx, dy, dz).normalized());
      }
    }
  }

  PT(GeomTriangles) tris = new GeomTriangles(Geom::UH_static);
  put_quad(tris, 4, 5, 1, 0);
  put_quad(tris, 6, 7, 5, 4);
  put_quad(tris, 2, 3, 7, 6);
  put_quad(tris, 0, 1, 3, 2);
  put_quad(tris, 5, 7, 3, 1);
  put_quad(tris, 6, 4, 0, 2);
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

    // NodePath cube = create_cube(0, 0, 0);
    // cube.reparent_to(window->get_render());
    // cube.set_pos(0, 10, 0);
    // cube.set_scale(2.0f);

    for (int x = -10; x <= 10; x++) {
      for (int y = -10; y <= 10; y++) {
        NodePath cube = create_cube(x, y, (noise.noise(x, y) * 1.0f));
        cube.reparent_to(window->get_render());
        cube.set_scale(2.0f);
      }
    }
    
    framework.main_loop();
    framework.close_framework();
    
    return 0;
}