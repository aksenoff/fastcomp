from itertools import imap, repeat
import cStringIO

from model import BasicModel as Model
from arithmetic import Encoder, Decoder

def encode(input, output):
    model = Model()
    encoder = Encoder(output)
    for char in iter(imap(input.read, repeat(1)).next, ''):  # iter(lambda: input.read(1), '')
        low_count, high_count, scale = model.encode(char)
        encoder.encode(low_count, high_count, scale)
    encoder.flush()

def decode(input, output, size):
    model = Model()
    decoder = Decoder(input)
    for i in xrange(size):
        scale = model.get_scale()
        code = decoder.decode(scale)
        char, low_count, high_count, scale2 = model.decode(code)
        assert scale == scale2
        decoder.remove(low_count, high_count, scale)
        output.write(char)

def main():
    data = 'ababac' * 10000
    input = cStringIO.StringIO(data)
    output = cStringIO.StringIO()
    encode(input, output)
    print 'COMPRESS LENGTH:', len(output.getvalue())
    output.seek(0)
    result = cStringIO.StringIO()
    decode(output, result, len(data))
    if result.getvalue() == data: print 'PASSED'
    else: print 'ERROR'

if __name__ == '__main__': main()
