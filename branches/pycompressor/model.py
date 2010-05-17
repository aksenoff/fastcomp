
EOF = -1
MAX_WEIGHT = 2**14-1

class BasicModel(object):
    @staticmethod
    def encode(char): # -> iterator: low_count, high_count, scale
        if char == EOF: i = 256
        else: i = ord(char)
        return False, i, i+1, 257
    @staticmethod
    def get_scale():
        return 257
    @staticmethod
    def decode(code): # -> char, low_count, high_count, scale
        if code == 256: char = EOF
        else: char = chr(code)
        return char, code, code+1, 257

basic_model = BasicModel()

class Node(object):
    __slots__ = 'char', 'weight', 'left_weight', 'right_weight', 'left', 'right'
    def __init__(self, char):
        self.char = char
        self.weight = 1
        self.left_weight = self.right_weight = 0
        self.left = self.right = None

def tostr(node, level=1):
    if node.left is None: leftstr = None
    else: leftstr = tostr(node.left, level+1)
    if node.right is None: rightstr = None
    else: rightstr = tostr(node.right, level+1)
    return '(%d:%s %d:%s:%d %s:%d)' % (node.left_weight, leftstr, level, node.char, node.weight, rightstr, node.right_weight)

def rescale(node):
    if node.left is not None:
        node.left_weight = rescale(node.left)
    if node.right is not None:
        node.right_weight = rescale(node.right)
    node.weight = (node.weight+1)//2
    return node.weight + node.left_weight + node.right_weight

def balance(parent, attr, char):
    node = getattr(parent, attr)
    while True:
        if char == node.char: return
        diff = abs(node.left_weight - node.right_weight)
        if char < node.char:
            child = node.left
            assert child is not None
            left_weight2 = child.left_weight
            right_weight2 = child.right_weight + node.weight + node.right_weight
            diff2 = abs(left_weight2 - right_weight2)
            if diff2 < diff:
                node.left_weight, child.right_weight = \
                    child.right_weight, child.right_weight + node.weight + node.right_weight
                node.left = child.right
                child.right = node
                setattr(parent, attr, child)
                if char == child.char: return
                parent = child
                if char < child.char:
                    node = child.left
                    assert node is not None
                    attr = 'left'
                else:
                    # node = node
                    attr = 'right'
            else:
                parent = node
                attr = 'left'
                node = child
        else:
            child = node.right
            assert child is not None
            right_weight2 = child.right_weight
            left_weight2 = child.left_weight + node.weight + node.left_weight
            diff2 = abs(left_weight2 - right_weight2)
            if diff2 < diff:
                node.right_weight, child.left_weight = \
                    child.left_weight, node.left_weight + node.weight + child.left_weight
                node.right = child.left
                child.left = node
                setattr(parent, attr, child)
                if char == child.char: return
                parent = child
                if char > child.char:
                    node = child.right
                    assert node is not None
                    attr = 'right'
                else:
                    # node = node
                    attr = 'left'
            else:
                parent = node
                attr = 'right'
                node = child

def process_char(parent, attr, char): # -> node, low_count
    node = getattr(parent, attr)
    tree_weight = node.weight + node.left_weight + node.right_weight
    if tree_weight == MAX_WEIGHT: rescale(node)
    low_count = 0
    while char <> node.char:
        if char < node.char:
            node.left_weight += 1
            if node.left is None:
                node.left = Node(char)
                node = node.left
            else: node = node.left
        else:
            node.right_weight += 1
            low_count += node.left_weight + node.weight
            if node.right is None:
                node.right = Node(char)
                node = node.right
            else: node = node.right
    node.weight += 1
    low_count += node.left_weight
    balance(parent, attr, char)
    return node, low_count

def process_code(parent, attr, code): # -> node, low_count
    node = getattr(parent, attr)
    tree_weight = node.weight + node.left_weight + node.right_weight
    if tree_weight == MAX_WEIGHT: rescale(node)
    low_count = 0
    while True:
        assert node is not None
        a = low_count + node.left_weight
        b = a + node.weight
        if code < a:
            node.left_weight += 1
            node = node.left
        else:
            b = a + node.weight
            if code >= b:
                low_count = b
                node.right_weight += 1
                node = node.right
            else: break
    node.weight += 1
    low_count += node.left_weight
    balance(parent, attr, char)
    return node, low_count
            
class Model(object):
    def __init__(self):
        self.root = None
        self.size = 0
        self.escaped = False
        self.escaped_char = None
    def get_escape_weight(self):
        return self.size + 1
    def encode(self, char): # -> iterator: low_count, high_count, scale
        if self.escaped:
            assert char == self.escaped_char
            self.escaped = False 
            return basic_model.encode(char)
        if self.root is None:
            self.root = node = Node(char)
            low_count = 0
        else:
            node, low_count = process_char(self, 'root', char)
            
        root = self.root
        # print tostr(root)
        tree_weight = root.left_weight + root.weight + root.right_weight
        if node.weight > 1:
            high_count = low_count + node.weight - 1
            scale = tree_weight + self.get_escape_weight() - 1
            return False, low_count, high_count, scale
        self.size += 1
        # print 'size:', self.size
        self.escaped = True
        self.escaped_char = char
        escape_low_count = tree_weight - 1
        escape_high_count = escape_low_count + self.get_escape_weight() - 1
        escape_scale = escape_high_count
        return True, escape_low_count, escape_high_count, escape_scale
    def get_scale(self):
        if self.escaped: return basic_model.get_scale()
        root = self.root
        if root is None: tree_weight = 0
        else: tree_weight = root.left_weight + root.weight + root.right_weight
        return tree_weight + self.get_escape_weight()
    def decode(self, code): # -> char, low_count, high_count, scale
        if self.escaped:
            self.escaped = False
            char, low_count, high_count, scale = basic_model.decode(code)
            if self.root is None: self.root = Node(char)
            else: process_char(self, 'root', char)
            self.size += 1
            # print tostr(self.root), self.size
            return char, low_count, high_count, scale
        root = self.root
        if root is None:
            self.escaped = True
            return None, 0, 1, 1
        tree_weight = root.left_weight + root.weight + root.right_weight
        scale = tree_weight + self.get_escape_weight()
        if code >= tree_weight:
            assert code < scale
            self.escaped = True
            return None, tree_weight, scale, scale
        node, low_count = process_code(self, 'root', code)
        high_count = low_count + node.weight - 1
        return node.char, low_count, high_count, scale
    