import cStringIO

def encode(input, output):
    for char in iter(input.read, ''):
        output.write(char)

def decode(input, output):
    for char in iter(input.read, ''):
        output.write(char)

def main():
    data = 'ababac' * 100000
    input = cStringIO.StringIO(data)
    output = cStringIO.StringIO()
    encode(input, output)
    output.seek(0)
    result = cStringIO.StringIO()
    decode(output, result)
    if result.getvalue() == data: print 'PASSED'
    else: print 'ERROR'

if __name__ == '__main__': main()
