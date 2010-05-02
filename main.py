from itertools import imap, repeat, chain
import cStringIO

from model import Model, EOF, tostr
from arithmetic import Encoder, Decoder

def encode(input, output):
    model = Model()
    encoder = Encoder(output)
    # for char in chain(iter(lambda: input.read(1), ''), [EOF])
    for char in chain(iter(imap(input.read, repeat(1)).next, ''), [EOF]):
        while True:
            escaped, low_count, high_count, scale = model.encode(char)
            # print char, escaped, low_count, high_count, scale
            encoder.encode(low_count, high_count, scale)
            if not escaped: break
    encoder.flush()
    # print 'ENCODING TREE:'
    # print tostr(model.root)

def decode(input, output):
    model = Model()
    decoder = Decoder(input)
    while True:
        scale = model.get_scale()
        code = decoder.decode(scale)
        char, low_count, high_count, scale2 = model.decode(code)
        # print scale, code, low_count, high_count, char
        assert scale == scale2
        decoder.remove(low_count, high_count, scale)
        if char == EOF: break
        if char is not None: output.write(char)
    # print 'DECODING TREE:'
    # print tostr(model.root)

def main():
    data = 'ababac' * 1000
    input = cStringIO.StringIO(data)
    output = cStringIO.StringIO()
    encode(input, output)
    # print '----------'
    output.seek(0)
    result = cStringIO.StringIO()
    decode(output, result)
    if result.getvalue() == data: print 'PASSED'
    else: print 'ERROR'

if __name__ == '__main__': main()
