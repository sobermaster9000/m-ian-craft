from PIL import Image

texWidth, texHeight = 64, 32

texs = [
  'log',
  'leaves',
  'grass',
  'dirt',
  'sand',
  'stone',
  'snow',
  'water',
  'bedrock'
]
texs += ['dummy'] * max(0, 10 - len(texs))

imgs, widths, heights = [], [], []

for tex in texs:
  imgs.append(Image.open('assets/textures/' + tex + '.png'))
  widths.append(imgs[-1].size[0])
  heights.append(imgs[-1].size[1])

atlas = Image.new('RGBA', (texWidth, texHeight * len(imgs)))

for i, img in enumerate(imgs):
  atlas.paste(img, (0, i * texHeight))

atlas.save('assets/textures/atlas.png')

def getTexcoords(idx):
  totalHeight = texHeight * len(imgs)
  y1 = totalHeight - texHeight * (idx + 1)
  y2 = y1 + texHeight // 2
  y3 = y1 + texHeight
  y1p = y1 / totalHeight
  y2p = y2 / totalHeight
  y3p = y3 / totalHeight
  return [
    [(0.0, y2p), (0.25, y2p), (0.25, y3p), (0.0, y3p)],
    [(0.25, y2p), (0.5, y2p), (0.5, y3p), (0.25, y3p)],
    [(0.5, y2p), (0.75, y2p), (0.75, y3p), (0.5, y3p)],
    [(0.75, y2p), (1.0, y2p), (1.0, y3p), (0.75, y3p)],
    [(0.0, y1p), (0.25, y1p), (0.25, y2p), (0.0, y2p)],
    [(0.25, y1p), (0.5, y1p), (0.5, y2p), (0.25, y2p)]
  ]

def printTexcoordsA(texcoords):
  faces = ['front', 'right', 'back', 'left', 'top', 'bottom']

  for i, texcoord in enumerate(texcoords):
    if texs[i] == 'dummy.png':
      continue
    print('***', texs[i], '***')
    for j, e in enumerate(texcoord):
      print(f'{faces[j]}: {e}')
    print()

def printTexcoordsB(texcoords):
  for i, texcoord in enumerate(texcoords):
    if texs[i] == 'dummy':
      continue
    print(f"'{texs[i]}': [")
    for j, e in enumerate(texcoord):
      print(f'  {e}' + (',' if j < len(texcoord) - 1 else ''))
    print(']' + (',' if i < len(texs) - texs.count('dummy') - 1 else ''))

texcoords = [getTexcoords(i) for i in range(len(imgs))]

printTexcoordsB(texcoords)