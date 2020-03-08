# compile the sine.h table
# making it easy to change the animation parameters
#
# for reference: I originally computed the table on the Gemma M0
# but it consumes tons of power (causing the Mac to turn off the
# USB port) and the board heats up
# both to save power on the field and preserve the board, it is
# best to precompute
# I suppose that it is due to the lack of a floating point unit
# on the SAMD21

import math

def map(x, in_min, in_max, out_min, out_max):
  result = int((x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min)
  if result < 100:
    return ' ' + str(result)
  else:
    return str(result)

def computeSine(frequency, sampling):
  result = []
  # sin() expects a radian (between 0 and 2PI)
  # division by sampling keeps it in range
  o = (2 * math.pi) / sampling
  for i in range(sampling):
    # 1.5PI so the table starts at minimum
    s = math.sin((o * i) + (1.5 * math.pi))
    result.append((s + 1) * 127)
  return result

def compileSineH(fname, sine):
  MAPS = ( 100, 100, 255 )
  with open(fname, 'w') as file:
    file.write('// sine.h\n')
    file.write('// generated file, do not edit\n')
    file.write('// use sine.py to re-generate\n\n')
    file.write('#ifndef __SINE_H\n')
    file.write('#define __SINE_H\n')
    file.write('#define SINE_LEN ' + str(len(sine) * 3) + '\n\n');
    file.write('const uint8_t sine[] = {\n')
    for i in range(3):
      file.write('  // wave: ' + str(i) + '\n')
      for j in range(len(sine)):
        if j % 10 == 0:
          file.write('  ')
        file.write(map(sine[j], 0, 255, 10, MAPS[i]))
        if (j + 1) % 10 == 0:
          file.write(',\n')
        else:
          file.write(', ')
    file.write('};\n\n')
    file.write('#endif\n')

compileSineH('sine.h', computeSine(1, 200))
