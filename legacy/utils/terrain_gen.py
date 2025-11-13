from perlin_noise import PerlinNoise
from random import randint

class TerrainGen:
  def __init__(self, octaves, seed):
    self.noise = PerlinNoise(octaves=octaves, seed=seed)
    self.offset = 20 # default sea level at -20
    self.squashFactor = 1
  
  def getGenHeightAndBiome(self, x, y): # ditch the randomness and make it be predetermined
    height = int(self.noise([x * 0.01, y * 0.01]) * 100) + self.offset
    height //= self.squashFactor
    return height, 'water' if height < 0 else \
      'sand' if height < 2 else \
        'grass' if height < 30 else \
          ('grass' if randint(1, height - 29) <= 3 else 'stone') if height < 40 else 'stone' 

if __name__ == '__main__':
  from PIL import Image

  BLUE = (0, 204, 204)
  YELLOW = (204, 204, 0)
  GREEN = (0, 204, 0)
  GRAY = (128, 128, 128)
  WHITE = (255, 255, 255)
  
  col = lambda val: BLUE if val < -0.2 else YELLOW if val < -0.1 else GREEN if val < 0.3 else GRAY

  noise = PerlinNoise(octaves=1, seed=69)

  img = Image.new('RGB', (500, 500), 'white')

  for x in range(500):
    for y in range(500):
      print(int(noise([x * 0.01, y * 0.01])))
      # img.putpixel((x, y), col(noise([x * 0.01, y * 0.01])))

  img.show()