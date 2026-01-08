#ifndef CHUNK_CLASS_H

#define CHUNK_CLASS_H

#include "mesh.h"

class Chunk: public Mesh {
    public:
    int chunkX;
    int chunkZ;

    Chunk(int chunkX, int chunkZ, std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
        : Mesh(vertices, indices, textures), chunkX(chunkX), chunkZ(chunkZ) {}
};

#endif