from PIL import Image

outFileName = 'table23_720x480_argb8888'
outFilePath = './include/'
outFileFullPath = outFilePath + outFileName + '.h'

im = Image.open('table2023.png')
rgb_im = im.convert('RGB')

width, height = rgb_im.size

r, g, b = rgb_im.getpixel((width//2, height//2))

f = open(outFileFullPath, 'w')

f.write("""
#ifndef """ + outFileName.upper() + '_H\n' + 
"""#define """ + outFileName.upper() + '_H' + 
"""

#ifdef __cplusplus
extern "C" {
#endif

const uint32_t """+outFileName+'['+str(width*height)+"""] =
{
""")

for y in range(height):
    for x in range(width):
        r, g, b = rgb_im.getpixel((x, y))
        f.write('0xFF' + hex(r)[2:].zfill(2) + hex(g)[2:].zfill(2) + hex(b)[2:].zfill(2) + ',\n')

f.write("""};

#ifdef __cplusplus
}
#endif

#endif""")