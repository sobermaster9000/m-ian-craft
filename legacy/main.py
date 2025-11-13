from direct.showbase.ShowBase import ShowBase
from direct.gui.OnscreenText import OnscreenText
from direct.task import Task
from direct.gui.OnscreenImage import OnscreenImage

from panda3d.core import Fog

from panda3d.core import GeomVertexFormat, GeomVertexData
from panda3d.core import Geom, GeomTriangles, GeomVertexWriter
from panda3d.core import Texture, GeomNode
from panda3d.core import TransparencyAttrib

from panda3d.physics import ActorNode, ForceNode, LinearVectorForce, PhysicsCollisionHandler
from panda3d.core import NodePath
from panda3d.core import CollisionNode, CollisionBox, CollisionPolygon, CollisionSegment, CollisionHandlerQueue, CollisionTraverser, Point3
from panda3d.core import Vec3
from panda3d.core import BitMask32

from panda3d.core import WindowProperties
from panda3d.core import loadPrcFileData

from perlin_noise import PerlinNoise
from math import floor, sqrt
from hashlib import sha256
from functools import lru_cache

from direct.stdpy.threading import Thread
from queue import Queue

from sys import exit as sys_exit

loadPrcFileData('', 'window-title M-Ian-Craft')
loadPrcFileData('', 'fullscreen 1')

def getDist(pointA, pointB):
  return sqrt(sum([(a - b) ** 2 for a, b in zip(pointA, pointB)]))

class Chunks():
  def __init__(self, seed, atlas):
    self.seed = seed
    self.noise = PerlinNoise(octaves=1, seed=seed)

    self.chunkSize = 16
    self.renderDist = 8

    self.faceQuads = [
      [0, 1, 5, 4],
      [1, 2, 6, 5],
      [2, 3, 7, 6],
      [3, 0, 4, 7],
      [4, 5, 6, 7],
      [3, 2, 1, 0]
    ]
    self.blockTexcoords = {
      'log': [
        [(0.0, 0.95), (0.25, 0.95), (0.25, 1.0), (0.0, 1.0)],
        [(0.25, 0.95), (0.5, 0.95), (0.5, 1.0), (0.25, 1.0)],
        [(0.5, 0.95), (0.75, 0.95), (0.75, 1.0), (0.5, 1.0)],
        [(0.75, 0.95), (1.0, 0.95), (1.0, 1.0), (0.75, 1.0)],
        [(0.0, 0.9), (0.25, 0.9), (0.25, 0.95), (0.0, 0.95)],
        [(0.25, 0.9), (0.5, 0.9), (0.5, 0.95), (0.25, 0.95)]
      ],
      'leaves': [
        [(0.0, 0.85), (0.25, 0.85), (0.25, 0.9), (0.0, 0.9)],
        [(0.25, 0.85), (0.5, 0.85), (0.5, 0.9), (0.25, 0.9)],
        [(0.5, 0.85), (0.75, 0.85), (0.75, 0.9), (0.5, 0.9)],
        [(0.75, 0.85), (1.0, 0.85), (1.0, 0.9), (0.75, 0.9)],
        [(0.0, 0.8), (0.25, 0.8), (0.25, 0.85), (0.0, 0.85)],
        [(0.25, 0.8), (0.5, 0.8), (0.5, 0.85), (0.25, 0.85)]
      ],
      'grass': [
        [(0.0, 0.75), (0.25, 0.75), (0.25, 0.8), (0.0, 0.8)],
        [(0.25, 0.75), (0.5, 0.75), (0.5, 0.8), (0.25, 0.8)],
        [(0.5, 0.75), (0.75, 0.75), (0.75, 0.8), (0.5, 0.8)],
        [(0.75, 0.75), (1.0, 0.75), (1.0, 0.8), (0.75, 0.8)],
        [(0.0, 0.7), (0.25, 0.7), (0.25, 0.75), (0.0, 0.75)],
        [(0.25, 0.7), (0.5, 0.7), (0.5, 0.75), (0.25, 0.75)]
      ],
      'dirt': [
        [(0.0, 0.65), (0.25, 0.65), (0.25, 0.7), (0.0, 0.7)],
        [(0.25, 0.65), (0.5, 0.65), (0.5, 0.7), (0.25, 0.7)],
        [(0.5, 0.65), (0.75, 0.65), (0.75, 0.7), (0.5, 0.7)],
        [(0.75, 0.65), (1.0, 0.65), (1.0, 0.7), (0.75, 0.7)],
        [(0.0, 0.6), (0.25, 0.6), (0.25, 0.65), (0.0, 0.65)],
        [(0.25, 0.6), (0.5, 0.6), (0.5, 0.65), (0.25, 0.65)]
      ],
      'sand': [
        [(0.0, 0.55), (0.25, 0.55), (0.25, 0.6), (0.0, 0.6)],
        [(0.25, 0.55), (0.5, 0.55), (0.5, 0.6), (0.25, 0.6)],
        [(0.5, 0.55), (0.75, 0.55), (0.75, 0.6), (0.5, 0.6)],
        [(0.75, 0.55), (1.0, 0.55), (1.0, 0.6), (0.75, 0.6)],
        [(0.0, 0.5), (0.25, 0.5), (0.25, 0.55), (0.0, 0.55)],
        [(0.25, 0.5), (0.5, 0.5), (0.5, 0.55), (0.25, 0.55)]
      ],
      'stone': [
        [(0.0, 0.45), (0.25, 0.45), (0.25, 0.5), (0.0, 0.5)],
        [(0.25, 0.45), (0.5, 0.45), (0.5, 0.5), (0.25, 0.5)],
        [(0.5, 0.45), (0.75, 0.45), (0.75, 0.5), (0.5, 0.5)],
        [(0.75, 0.45), (1.0, 0.45), (1.0, 0.5), (0.75, 0.5)],
        [(0.0, 0.4), (0.25, 0.4), (0.25, 0.45), (0.0, 0.45)],
        [(0.25, 0.4), (0.5, 0.4), (0.5, 0.45), (0.25, 0.45)]
      ],
      'snow': [
        [(0.0, 0.35), (0.25, 0.35), (0.25, 0.4), (0.0, 0.4)],
        [(0.25, 0.35), (0.5, 0.35), (0.5, 0.4), (0.25, 0.4)],
        [(0.5, 0.35), (0.75, 0.35), (0.75, 0.4), (0.5, 0.4)],
        [(0.75, 0.35), (1.0, 0.35), (1.0, 0.4), (0.75, 0.4)],
        [(0.0, 0.3), (0.25, 0.3), (0.25, 0.35), (0.0, 0.35)],
        [(0.25, 0.3), (0.5, 0.3), (0.5, 0.35), (0.25, 0.35)]
      ],
      'water': [
        [(0.0, 0.25), (0.25, 0.25), (0.25, 0.3), (0.0, 0.3)],
        [(0.25, 0.25), (0.5, 0.25), (0.5, 0.3), (0.25, 0.3)],
        [(0.5, 0.25), (0.75, 0.25), (0.75, 0.3), (0.5, 0.3)],
        [(0.75, 0.25), (1.0, 0.25), (1.0, 0.3), (0.75, 0.3)],
        [(0.0, 0.2), (0.25, 0.2), (0.25, 0.25), (0.0, 0.25)],
        [(0.25, 0.2), (0.5, 0.2), (0.5, 0.25), (0.25, 0.25)]
      ],
      'bedrock': [
        [(0.0, 0.15), (0.25, 0.15), (0.25, 0.2), (0.0, 0.2)],
        [(0.25, 0.15), (0.5, 0.15), (0.5, 0.2), (0.25, 0.2)],
        [(0.5, 0.15), (0.75, 0.15), (0.75, 0.2), (0.5, 0.2)],
        [(0.75, 0.15), (1.0, 0.15), (1.0, 0.2), (0.75, 0.2)],
        [(0.0, 0.1), (0.25, 0.1), (0.25, 0.15), (0.0, 0.15)],
        [(0.25, 0.1), (0.5, 0.1), (0.5, 0.15), (0.25, 0.15)]
      ]
    }

    # self.chunkColliderNP = NodePath('chunk_collider')

    self.leavesRadius = 3

    self.chunkCentersToGenerate = Queue()
    # self.chunkColCentersToGenerate = Queue()

    self.chunksToRender = Queue()
    self.chunksToUnload = Queue()

    # self.chunkColsToLoad = Queue()
    # self.chunkColsToUnload = Queue()

    self.renderedChunks = dict()
    # self.chunkColliders = dict()

    # self.atlas = self.loader.loadTexture('assets/textures/atlas.png')
    self.atlas = atlas
    self.atlas.setMagfilter(Texture.FT_nearest)
    self.atlas.setMinfilter(Texture.FT_nearest)

  def getVerts(self, x, y, z):
    return [(x - 0.5, y - 0.5, z),
            (x + 0.5, y - 0.5, z),
            (x + 0.5, y + 0.5, z),
            (x - 0.5, y + 0.5, z),
            (x - 0.5, y - 0.5, z + 1),
            (x + 0.5, y - 0.5, z + 1),
            (x + 0.5, y + 0.5, z + 1),
            (x - 0.5, y + 0.5, z + 1)]

  def getChunkCoord(self, x, y):
    return floor((x + self.chunkSize // 2) / self.chunkSize), \
      floor((y + self.chunkSize // 2) / self.chunkSize)

  @lru_cache
  def getGenHeight(self, x, y):
    xMul, yMul = 0.01, 0.01
    noiseMul = 100
    offset = 20
    squashFactor = 1
    height = (int(self.noise([x * xMul, y * yMul]) * noiseMul) + offset) // squashFactor
    height += floor(self.noise([x * 0.02, y * 0.02]) * 7.5) # from previous version
    return height
  
  def randomSeedProbability(self, string):
    return sha256(string.encode()).digest()[0] / 255

  def getTransMountBlock(self, x, y, z, grassLvl):
    # return 'grass' if randint(1, z - (grassLvl - 1)) <= 3 else 'stone'
    return 'grass' if (self.randomSeedProbability(f'{x}{y}{x}{self.seed}')) <= (-(z - (grassLvl - 1)) / 12.5 + 0.8) else 'stone'

  def getTransSnowBlock(self, x, y, z, snowTransLvl):
    return 'stone' if (self.randomSeedProbability(f'{x}{y}{x}{self.seed}')) <= 0.5 else 'snow' # probs change the 50%

  def getBiomeBlock(self, x, y, z):
    waterLvl = 10
    sandLvl = 12
    grassLvl = 30
    mountainTransLvl = 40
    snowTransLvl = 56
    snowLvl = 59
    return 'sand' if z < sandLvl else \
    'grass' if z < grassLvl else \
    self.getTransMountBlock(x, y, z, grassLvl) if z < mountainTransLvl else \
    'stone' if z < snowTransLvl else \
    self.getTransSnowBlock(x, y, z, snowTransLvl) if z < snowLvl else \
    'snow'
  
  def getTreeLogZ(self, x, y, Z):
    prob1 = self.randomSeedProbability(f'{x}{y}{self.seed}')
    prob2 = self.randomSeedProbability(f'{x}{y}{Z}{self.seed}')
    return Z + [4, 6][round(prob2)] if (prob1 <= 0.01 and 12 <= Z < 30) else float('-inf')

  def getLeavesZRange(self, x, y):
    queue = Queue()
    queue.put((x, y))
    visited = set([(x, y)])
    logXY = None
    treeLogZ = float('-inf')

    while not queue.empty():
      u, v = queue.get()
      for h, k in [(u + 1, v), (u - 1, v), (u, v + 1), (u, v - 1)]:
        if (h, k) not in visited and getDist((x, y), (h, k)) <= self.leavesRadius:
          treeLogZ = self.getTreeLogZ(h, k, self.getGenHeight(h, k))
          if treeLogZ != float('-inf'):
            logXY = (h, k)
            queue = Queue()
            break
          queue.put((h, k))
          visited.add((h, k))
    
    if logXY:
      z = treeLogZ - 2
      upperBound = treeLogZ - 2
      while floor(getDist((x, y, treeLogZ - 2), (*logXY, z))) <= self.leavesRadius:
        upperBound = z
        z += 1
      return range(treeLogZ - 2, upperBound + 1)

    return range(0)

  def addFaceToRenderData(self, faceIdx, blockTex, verts, renderData):
    renderData['quads'].append([verts[i] for i in self.faceQuads[faceIdx]])
    renderData['texcoordsIdx'].append(faceIdx)
    renderData['blockTex'].append(blockTex)  

  def genChunkData(self, chunkX, chunkY):
    chunkXBound1 = chunkX * self.chunkSize - self.chunkSize // 2
    chunkXBound2 = chunkX * self.chunkSize + self.chunkSize // 2
    chunkYBound1 = chunkY * self.chunkSize - self.chunkSize // 2
    chunkYBound2 = chunkY * self.chunkSize + self.chunkSize // 2

    renderData = {'chunkX': chunkX, 'chunkY': chunkY, 'quads': [], 'texcoordsIdx': [], 'blockTex': []}

    for x in range(chunkXBound1, chunkXBound2):
      for y in range(chunkYBound1, chunkYBound2):

        Z = self.getGenHeight(x, y)
        _z = min(
          self.getGenHeight(x + 1, y),
          self.getGenHeight(x - 1, y),
          self.getGenHeight(x, y + 1),
          self.getGenHeight(x, y - 1)
        )
        
        treeLogZ = self.getTreeLogZ(x, y, Z)
        z = max(Z, treeLogZ + 1)
        
        while True:
          verts = self.getVerts(x, y, z)
          blockTex = self.getBiomeBlock(x, y, z)

          if z < Z and blockTex == 'grass':
            blockTex = 'dirt'

          if z > Z:
            # blockTex = 'log'
            if z > treeLogZ:
              self.addFaceToRenderData(4, 'leaves', verts, renderData)
              z -= 3
            blockTex = 'log'

          if self.getGenHeight(x, y - 1) < z:
            self.addFaceToRenderData(0, blockTex, verts, renderData)
          if self.getGenHeight(x + 1, y) < z:
            self.addFaceToRenderData(1, blockTex, verts, renderData)
          if self.getGenHeight(x, y + 1) < z:
            self.addFaceToRenderData(2, blockTex, verts, renderData)
          if self.getGenHeight(x - 1, y) < z:
            self.addFaceToRenderData(3, blockTex, verts, renderData)
          # top face
          if z == (Z if treeLogZ == float('-inf') else treeLogZ + 1):
            self.addFaceToRenderData(4, blockTex, verts, renderData)

          # for water
          waterLvl = 10
          if z <= waterLvl:
            verts = self.getVerts(x, y, waterLvl)
            self.addFaceToRenderData(4, 'water', verts, renderData)
          
          if _z + 1 >= z:
            break

          z -= 1
        
        leavesZRange = self.getLeavesZRange(x, y)
        for z in leavesZRange:
          if z <= Z or z <= treeLogZ + 1:
            continue

          verts = self.getVerts(x, y, z)
          blockTex = 'leaves'
          
          if self.getGenHeight(x, y - 1) < z and self.getTreeLogZ(x, y - 1, Z) + 1 <= z and z not in self.getLeavesZRange(x, y - 1):
            self.addFaceToRenderData(0, blockTex, verts, renderData)
          if self.getGenHeight(x + 1, y) < z and self.getTreeLogZ(x + 1, y, Z) + 1 <= z and z not in self.getLeavesZRange(x + 1, y):
            self.addFaceToRenderData(1, blockTex, verts, renderData)
          if self.getGenHeight(x, y + 1) < z and self.getTreeLogZ(x, y + 1, Z) + 1 <= z and z not in self.getLeavesZRange(x, y + 1):
            self.addFaceToRenderData(2, blockTex, verts, renderData)
          if self.getGenHeight(x - 1, y) < z and self.getTreeLogZ(x - 1, y, Z) + 1 <= z and z not in self.getLeavesZRange(x - 1, y):
            self.addFaceToRenderData(3, blockTex, verts, renderData)
          if z == leavesZRange.stop - 1:
            self.addFaceToRenderData(4, blockTex, verts, renderData)
          if z == leavesZRange.start and Z < z and treeLogZ + 1 < z:
            self.addFaceToRenderData(5, blockTex, verts, renderData)

    self.chunksToRender.put(renderData)

  # def addColFaceToChunk(self, faceIdx, coords, colData):
  #   colData['faceIdx'].append(faceIdx)
  #   colData['coords'].append(coords)

  # def genChunkColData(self, chunkX, chunkY):
  #   chunkXBound1 = chunkX * self.chunkSize - self.chunkSize // 2
  #   chunkXBound2 = chunkX * self.chunkSize + self.chunkSize // 2
  #   chunkYBound1 = chunkY * self.chunkSize - self.chunkSize // 2
  #   chunkYBound2 = chunkY * self.chunkSize + self.chunkSize // 2
  #   offset = 3

  #   colData = {'chunkX': chunkX, 'chunkY': chunkY, 'faceIdx': [], 'coords': []}

  #   for x in range(chunkXBound1 - offset, chunkXBound2 + offset):
  #     for y in range(chunkYBound1 - offset, chunkYBound2 + offset):

  #       Z = self.getGenHeight(x, y)
  #       _z = min(
  #         self.getGenHeight(x + 1, y),
  #         self.getGenHeight(x - 1, y),
  #         self.getGenHeight(x, y + 1),
  #         self.getGenHeight(x, y - 1)
  #       )
        
  #       treeLogZ = self.getTreeLogZ(x, y, Z)
  #       z = max(Z, treeLogZ + 1)
        
  #       while True:
  #         if z > Z and z > treeLogZ:
  #           self.addColFaceToChunk(4, (x, y, z), colData)
  #           z -= 3

  #         if self.getGenHeight(x, y - 1) < z:
  #           self.addColFaceToChunk(0, (x, y, z), colData)
  #         if self.getGenHeight(x + 1, y) < z:
  #           self.addColFaceToChunk(1, (x, y, z), colData)
  #         if self.getGenHeight(x, y + 1) < z:
  #           self.addColFaceToChunk(2, (x, y, z), colData)
  #         if self.getGenHeight(x - 1, y) < z:
  #           self.addColFaceToChunk(3, (x, y, z), colData)
  #         # top face
  #         if z == (Z if treeLogZ == float('-inf') else treeLogZ + 1):
  #           self.addColFaceToChunk(4, (x, y, z), colData)

  #         # for water
  #         # waterLvl = 10
  #         # if z <= waterLvl:
  #         #   self.addColFaceToChunk(4, (x, y, z), colData)
          
  #         if _z + 1 >= z:
  #           break

  #         z -= 1
        
  #       leavesZRange = self.getLeavesZRange(x, y)
  #       for z in leavesZRange:
  #         if z <= Z or z <= treeLogZ + 1:
  #           continue

  #         if self.getGenHeight(x, y - 1) < z and self.getTreeLogZ(x, y - 1, Z) + 1 <= z and z not in self.getLeavesZRange(x, y - 1):
  #           self.addColFaceToChunk(0, (x, y, z), colData)
  #         if self.getGenHeight(x + 1, y) < z and self.getTreeLogZ(x + 1, y, Z) + 1 <= z and z not in self.getLeavesZRange(x + 1, y):
  #           self.addColFaceToChunk(1, (x, y, z), colData)
  #         if self.getGenHeight(x, y + 1) < z and self.getTreeLogZ(x, y + 1, Z) + 1 <= z and z not in self.getLeavesZRange(x, y + 1):
  #           self.addColFaceToChunk(2, (x, y, z), colData)
  #         if self.getGenHeight(x - 1, y) < z and self.getTreeLogZ(x - 1, y, Z) + 1 <= z and z not in self.getLeavesZRange(x - 1, y):
  #           self.addColFaceToChunk(3, (x, y, z), colData)
  #         if z == leavesZRange.stop - 1:
  #           self.addColFaceToChunk(4, (x, y, z), colData)
  #         if z == leavesZRange.start and Z < z and treeLogZ + 1 < z:
  #           self.addColFaceToChunk(5, (x, y, z), colData)
    
  #   self.chunkColsToLoad.put(colData)

  def genChunksData(self, chunkX, chunkY):
    self.renderedChunks[chunkX, chunkY] = None
    chunkSet = set()
    toGenerate = []

    for i in range(-self.renderDist, self.renderDist + 1):
      for j in range(-self.renderDist, self.renderDist + 1):
        chunkSet.add((chunkX + i, chunkY + j))
        if self.renderedChunks.get((chunkX + i, chunkY + j)) is None:
            # self.genChunkData(chunkX + i, chunkY + j)
            toGenerate.append((chunkX + i, chunkY + j))
    
    [self.genChunkData(x, y) for x, y in sorted(toGenerate, key=lambda pos: getDist((chunkX, chunkY), pos))]
    
    [self.chunksToUnload.put((x, y)) for x, y in sorted(self.renderedChunks.keys() - chunkSet, key=lambda pos: getDist((chunkX, chunkY), pos), reverse=True)]

    # [self.chunksToUnload.put((x, y)) for x, y in self.renderedChunks.keys() - chunkSet]

  # def genChunkColsData(self, chunkX, chunkY):
  #   self.chunkColliders[chunkX, chunkY] = None
  #   chunkSet = set()
  #   toGenerate = []

  #   for i in range(1):
  #     for j in range(1):
  #       chunkSet.add((chunkX + i, chunkY + j))
  #       if self.chunkColliders.get((chunkX + i, chunkY + j)) is None:
  #         toGenerate.append((chunkX + i, chunkY + j))
    
  #   [self.genChunkColData(x, y) for x, y in sorted(toGenerate, key=lambda pos: getDist((chunkX, chunkY), pos))]

  #   [self.chunkColsToUnload.put((x, y)) for x, y in sorted(self.chunkColliders.keys() - chunkSet, key=lambda pos: getDist((chunkX, chunkY), pos), reverse=True)]

  def chunkGenThread(self):
    print('started chunkGenThread')
    while True:
      if not self.chunkCentersToGenerate.empty():
        self.genChunksData(*self.chunkCentersToGenerate.get())

  # def chunkColGenThread(self):
  #   print('started chunkColGenThread')
  #   while True:
  #     if not self.chunkColCentersToGenerate.empty():
  #       self.genChunkColsData(*self.chunkColCentersToGenerate.get())

  def renderChunk(self, renderData, render):
    chunkX, chunkY, quads, texcoordsIdx, blockTex = renderData.values()
    chunkNode = GeomNode('chunk')

    format = GeomVertexFormat.getV3n3cpt2()
    vdata = GeomVertexData('chunk', format, Geom.UHDynamic)
    verts = GeomVertexWriter(vdata, 'vertex')
    tris = GeomTriangles(Geom.UHDynamic)
    texcoord = GeomVertexWriter(vdata, 'texcoord')

    vertsLen = 0

    for i, quad in enumerate(quads):
      [verts.addData3(x, y, z) for x, y, z in quad]
      tris.addVertices(vertsLen + 0, vertsLen + 1, vertsLen + 3)
      tris.addVertices(vertsLen + 1, vertsLen + 2, vertsLen + 3)
      [texcoord.addData2f(u, v) for u, v in self.blockTexcoords[blockTex[i]][texcoordsIdx[i]]]
      vertsLen += 4

    chunkGeom = Geom(vdata)
    chunkGeom.addPrimitive(tris)
    chunkNode.addGeom(chunkGeom)

    chunkNP = render.attachNewNode(chunkNode)
    chunkNP.setTexture(self.atlas)
    chunkNP.setTransparency(True)
    self.renderedChunks[chunkX, chunkY] = chunkNP

  # def unloadChunks(self, chunkX, chunkY):
  #   chunkSet = set()
  #   for i in range(-self.renderDist, self.renderDist + 1):
  #     for j in range(-self.renderDist, self.renderDist + 1):
  #       chunkSet.add((chunkX + i, chunkY + j))
    
  #   [self.chunksToUnload.put((x, y)) for x, y in self.renderedChunks.keys() - chunkSet]

  # def loadChunkCol(self, colData, render):
  #   chunkX, chunkY, faceIdx, coords = colData.values()
  #   chunkColNode = CollisionNode('chunk_collider')

  #   for coord, i in zip(coords, faceIdx):
  #     verts = self.getVerts(*coord)
  #     faceColPoly = CollisionPolygon(*[Point3(*verts[j]) for j in self.faceQuads[i]])
  #     chunkColNode.addSolid(faceColPoly)
    
  #   chunkColNP = self.chunkColliderNP.attachNewNode(chunkColNode)
  #   # chunkColNP.show()

  #   chunkColNP.node().setFromCollideMask(BitMask32.bit(1))
  #   chunkColNP.node().setIntoCollideMask(BitMask32.bit(1))

  #   self.chunkColliders[chunkX, chunkY] = chunkColNP

class mIANcraft(ShowBase):
  def __init__(self):
    ShowBase.__init__(self)
    
    winProps = WindowProperties()
    winProps.setIconFilename("assets/icon.ico")
    self.win.requestProperties(winProps)

    self.enableRelativeMouse()

    self.camLens.setFov(90)
    self.camLens.setNearFar(0.1, 1000)
    
    self.initPlayerCollision()
    self.initPlayerControls()

    self.setBackgroundColor(0.5, 0.8, 1.0)

    self.chunks = Chunks(420, self.loader.loadTexture('assets/textures/atlas.png'))

    fog = Fog('distance_fog')
    fog.setColor(0.5, 0.8, 1.0)
    # fog.setLinearRange(0, self.chunks.renderDist * self.chunks.chunkSize)
    fog.setExpDensity(0.005)
    self.render.setFog(fog)

    self.prevPlayerChunkCoords = (0, 0)
    self.lastChunkRenderCoords = (0, 0)

    # self.chunks.chunkColliderNP.reparentTo(self.render)
    
    self.tickDuration = 0.05
    self.lastTick = 0
    self.taskMgr.add(self.tickUpdateTask, 'tick_update_task')

    self.taskMgr.add(self.suspendPlayerTask, 'suspend_player_task')

    self.crosshair = OnscreenImage(image='assets/textures/crosshair.png', pos=(0, 0, 0), scale=0.02)
    self.crosshair.setTransparency(TransparencyAttrib.MAlpha)
    self.crosshairTex = self.crosshair.getTexture() 
    self.crosshairTex.setMagfilter(Texture.FT_nearest)
    self.crosshairTex.setMinfilter(Texture.FT_nearest)

    self.controlsText = OnscreenText(
      text='Controls:\n\nWASD - Move\nMouse - Look around\nEscape - Exit',
      pos=(0, 0.9),
      scale=0.05,
      fg=(1, 1, 1, 1)
    )

  def initPlayerCollision(self):
    self.enableParticles()
    self.playerPhysicsNode = ActorNode('player_physics')
    self.playerPhysicsNP = self.render.attachNewNode(self.playerPhysicsNode)
    self.physicsMgr.attachPhysicalNode(self.playerPhysicsNode)

    self.playerNP = NodePath('player')
    self.playerNP.reparentTo(self.playerPhysicsNP)
    self.camera.reparentTo(self.playerNP)
    self.playerNP.setPos(0, 0, 1)
    self.disableMouse()
    self.camera.setPos(0, 0, 1.5)

    self.playerCollisionNode = CollisionNode('player_collider')
    self.playerCollisionBox = CollisionBox(Point3(0, 0, 0.9), 0.3, 0.3, 0.9)
    self.playerCollisionNode.addSolid(self.playerCollisionBox)
    self.playerCollisionNP = self.playerNP.attachNewNode(self.playerCollisionNode)
    # self.playerCollisionNP.show()

    self.playerCollisionNP.node().setFromCollideMask(BitMask32.bit(1))
    self.playerCollisionNP.node().setIntoCollideMask(BitMask32.bit(1))

    self.groundCheckNode = CollisionNode('player_ground_check')
    self.groundCheckBox = CollisionBox(Point3(0, 0, 0), 0.3, 0.3, 0.1)
    self.groundCheckNode.addSolid(self.groundCheckBox)
    self.groundCheckNP = self.playerNP.attachNewNode(self.groundCheckNode)
    # self.groundCheckNP.show()

    self.groundCheckNP.node().setFromCollideMask(BitMask32.bit(1))
    self.groundCheckNP.node().setIntoCollideMask(BitMask32.bit(0))

    self.groundCheckHandler = CollisionHandlerQueue()

    self.collisionPusher = PhysicsCollisionHandler()
    self.collisionPusher.addCollider(self.playerCollisionNP, self.playerPhysicsNP)
    
    self.cTrav = CollisionTraverser()
    self.cTrav.addCollider(self.playerCollisionNP, self.collisionPusher)
    self.cTrav.addCollider(self.groundCheckNP, self.groundCheckHandler)

  def initPlayerControls(self):
    self.keyStates = {'w': False, 'a': False, 's': False, 'd': False}
    self.accept('w', self.setKeyState, ['w', True])
    self.accept('a', self.setKeyState, ['a', True])
    self.accept('s', self.setKeyState, ['s', True])
    self.accept('d', self.setKeyState, ['d', True])
    self.accept('w-up', self.setKeyState, ['w', False])
    self.accept('a-up', self.setKeyState, ['a', False])
    self.accept('s-up', self.setKeyState, ['s', False])
    self.accept('d-up', self.setKeyState, ['d', False])
    # self.accept('space', self.playerJump)
    # self.accept('mouse1', self.destroyBlock)
    # self.accept('mouse3', self.placeBlock)
    self.accept('escape', sys_exit)

    self.speed = 5
    self.taskMgr.add(self.updatePlayerMotionTask, "update_player_motion_task")

    self.mouseSensitivity = 30
    self.taskMgr.add(self.updateMouseMotionTask, 'update_mouse_motion_task')

  def clampCamAngle(self, pitch):
    return max(min(pitch, 90), -90)

  def enableRelativeMouse(self):
    winProps = WindowProperties()
    winProps.setCursorHidden(True)
    winProps.setMouseMode(WindowProperties.M_relative)
    self.win.requestProperties(winProps)

  def disableRelativeMouse(self):
    winProps = WindowProperties()
    winProps.setCursorHidden(False)
    winProps.setMouseMode(WindowProperties.M_absolute)
    self.win.requestProperties(winProps)

  def updateMouseMotionTask(self, task): # change to m_relative mode
    if self.mouseWatcherNode.hasMouse():
      mouse = self.mouseWatcherNode.getMouse()
      mouseX, mouseY = mouse.getX(), mouse.getY()
      centerX, centerY = self.win.getXSize() // 2, self.win.getYSize() // 2
      dx, dy = mouseX * self.mouseSensitivity, mouseY * self.mouseSensitivity

      self.camera.setH(self.camera.getH() - dx)
      self.camera.setP(self.clampCamAngle(self.camera.getP() + dy))

      self.win.movePointer(0, centerX, centerY)

      # md = self.win.getPointer(0)
      # dx = md.getX() * self.mouseSensitivity
      # dy = md.getY() * self.mouseSensitivity
      # self.camera.setH(self.camera.getH() - dx)
      # self.camera.setP(self.clampCamAngle(self.camera.getP() + dy))

    return Task.cont

  def setKeyState(self, key, state):
    self.keyStates[key] = state

  def updatePlayerMotionTask(self, task):
    dt = globalClock.getDt()
    
    # fix diagonals stacking velocities
    direction = Vec3(0, 0, 0)
    if self.keyStates['w']:
      direction += self.camera.getQuat().getForward()
    if self.keyStates['a']:
      direction -= self.camera.getQuat().getRight()
    if self.keyStates['s']:
      direction -= self.camera.getQuat().getForward()
    if self.keyStates['d']:
      direction += self.camera.getQuat().getRight()

    # direction.normalize()
    self.playerNP.setPos(self.playerNP.getPos() + direction * self.speed * dt)

    return Task.cont

  def suspendPlayerTask(self, task):
    if task.time < 3:
      self.playerNP.setPos(0, 0, self.chunks.getGenHeight(0, 0) + 2)
      return Task.cont
    else:
      self.playerForces = ForceNode('player_forces')
      self.gravNP = self.render.attachNewNode(self.playerForces)
      # self.gravForce = LinearVectorForce(0, 0, -30)
      self.gravForce = LinearVectorForce(0, 0, 0)
      self.playerForces.addForce(self.gravForce)
      self.physicsMgr.addLinearForce(self.gravForce)

  # def playerIsGrounded(self):
  #   # print(list(self.groundCheckHandler.entries))
  #   return any(str(hit.getIntoNodePath()).endswith('chunk_collider') \
  #              for hit in self.groundCheckHandler.entries)

  # def playerJump(self):
  #   if self.playerIsGrounded():
  #     self.playerPhysicsNode.getPhysicsObject().addImpulse(Vec3(0, 0, 10))

  def getDist(self, pointA, pointB):
    return sqrt(sum([(a - b) ** 2 for a, b in zip(pointA, pointB)]))

  # def rayCast(self):
  #   self.rayLength = 5
  #   startPoint = self.playerNP.getPos(self.render)
  #   startPoint.z += 1.5
  #   transformVect = self.camera.getQuat().getForward() * self.rayLength
  #   endPoint = startPoint + transformVect

  #   rayCastNP = NodePath('raycast')
  #   rayCastNP.reparentTo(self.render)
  #   rayCastColNode = CollisionNode('raycast')
  #   rayCastCol = CollisionSegment(*startPoint, *endPoint)
  #   rayCastColNode.addSolid(rayCastCol)
  #   rayCastColNP = rayCastNP.attachNewNode(rayCastColNode)
  #   # rayCastColNP.show()
    
  #   rayCastColNP.node().setFromCollideMask(BitMask32.bit(1))
  #   rayCastColNP.node().setIntoCollideMask(BitMask32.bit(0))

  #   self.rayCastHandler = CollisionHandlerQueue()

  #   self.cTrav.addCollider(rayCastColNP, self.rayCastHandler)
  #   self.cTrav.traverse(self.render)

  #   result = None
  #   for hit in self.rayCastHandler.entries:
  #     hitNodePath = hit.getIntoNodePath()
  #     if str(hitNodePath).endswith('chunk_collider'):
  #       result = (hit.getSurfacePoint(hitNodePath), hit.getSurfaceNormal(hitNodePath))

  #   rayCastNP.removeNode()

  #   return result

  # def rayCast(self):
  #   self.rayLength = 5
  #   startPoint = self.playerNP.getPos(self.render)
  #   startPoint.z += 1.5
  #   transformVect = self.camera.getQuat().getForward() * self.rayLength
  #   endPoint = startPoint + transformVect

  #   rayCastNP = NodePath('raycast')
  #   rayCastNP.reparentTo(self.render)
  #   rayCastColNode = CollisionNode('raycast')
  #   rayCastCol = CollisionSegment(*startPoint, *endPoint)
  #   rayCastColNode.addSolid(rayCastCol)
  #   rayCastColNP = rayCastNP.attachNewNode(rayCastColNode)
  #   # rayCastColNP.show()
    
  #   rayCastColNP.node().setFromCollideMask(BitMask32.bit(1))
  #   rayCastColNP.node().setIntoCollideMask(BitMask32.bit(0))

  #   self.rayCastHandler = CollisionHandlerQueue()

  #   self.cTrav.addCollider(rayCastColNP, self.rayCastHandler)
  #   self.cTrav.traverse(self.render)

  #   result = None

  #   if self.rayCastHandler.getNumEntries == 0:
  #     return result
    
  #   hit = self.rayCastHandler.getEntry(0)
  #   hitNP = hit.getIntoNodePath()
  #   if str(hitNP).endswith('chunk_collider'):
  #     result = hit.getSurfacePoint(hitNP), hit.getSurfaceNormal(hitNP)

  #   return result

  def tickUpdateTask(self, task):
    if task.time  - self.lastTick >= self.tickDuration:
      currentChunkCoords = self.chunks.getChunkCoord(self.playerNP.get_x(), self.playerNP.get_y())

      if currentChunkCoords not in self.chunks.renderedChunks:
        self.chunks.chunkCentersToGenerate.put(currentChunkCoords)
        # self.chunks.chunkColCentersToGenerate.put(currentChunkCoords)
      
      if currentChunkCoords != self.prevPlayerChunkCoords:
        # self.chunks.chunkColCentersToGenerate.put(currentChunkCoords)
        if self.getDist(currentChunkCoords, self.lastChunkRenderCoords) > sqrt(2):
          self.chunks.chunkCentersToGenerate.put(currentChunkCoords)
          self.lastChunkRenderCoords = currentChunkCoords

      chunksPerTick = 1
      chunksThisTick = 0
      while not self.chunks.chunksToRender.empty() and chunksThisTick < chunksPerTick:
        self.chunks.renderChunk(self.chunks.chunksToRender.get(), self.render)
        chunksThisTick += 1
      
      # chunksThisTick = 0
      # while not self.chunks.chunkColsToLoad.empty() and chunksThisTick < chunksPerTick:
      #   self.chunks.loadChunkCol(self.chunks.chunkColsToLoad.get(), self.render)
      #   chunksThisTick += 1

      chunksThisTick = 0
      while not self.chunks.chunksToUnload.empty() and chunksThisTick < chunksPerTick:
        chunkToUnload = self.chunks.chunksToUnload.get()
        if chunkToUnload in self.chunks.renderedChunks:
          self.chunks.renderedChunks.pop(chunkToUnload).removeNode()
        else:
          self.chunks.chunksToUnload.put(chunkToUnload)
        chunksThisTick += 1

      # chunksThisTick = 0
      # while not self.chunks.chunkColsToUnload.empty() and chunksThisTick < chunksPerTick:
      #   chunkColToUnload = self.chunks.chunkColsToUnload.get()
      #   if chunkColToUnload in self.chunks.chunkColliders:
      #     self.chunks.chunkColliders.pop(chunkColToUnload).removeNode()
      #   else:
      #     self.chunks.chunkColsToUnload.put(chunkColToUnload)
      #   chunksThisTick += 1

      self.lastTick = task.time

    return Task.cont

if __name__ == '__main__':
  app = mIANcraft()
  Thread(target=app.chunks.chunkGenThread, args=()).start()
  # Thread(target=app.chunks.chunkColGenThread, args=()).start()
  app.run()