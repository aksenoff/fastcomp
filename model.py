
class BasicModel(object):
    def __init__(self):
        pass
    def encode(self, char): # -> low_count, high_count, scale
        i = ord(char)
        return i, i+1, 256
    def get_scale(self):
        return 256
    def decode(self, code): # -> char, low_count, high_count, scale
        return chr(code), code, code+1, 256