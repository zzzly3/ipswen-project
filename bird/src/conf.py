#!/usr/bin/python3

import os, sys, pickle, base64, typing, traceback

def encode_objstr(obj):
    return 'objstr:base64,' + base64.b64encode(pickle.dumps(obj)).decode()

def encode_objfile(obj, path):
    with open(path, 'wb') as f:
        pickle.dump(obj, f)
    return 'objfile:binary,' + path

def decode_objstr(text):
    assert text[:7] == 'objstr:'
    i = text.find(',')
    assert i > 7
    if text[7:i] == 'base64':
        return pickle.loads(base64.b64decode(text[i+1:]))
    else:
        raise ValueError('unknown encoding')

def decode_objfile(text):
    assert text[:8] == 'objfile:'
    i = text.find(',')
    assert i > 8
    if text[8:i] == 'binary':
        with open(text[i+1:], 'rb') as f:
            return pickle.load(f)
    else:
        raise ValueError('unknown encoding')

g : typing.Dict[str, typing.Any] = {
    '__output__': '',
}

def echo(text):
    global g
    g['__output__'] += str(text)

def getenv(key):
    a = os.getenv(key)
    try:
        assert a is not None
        i = a.find(':')
        assert i > 0
        if a[:i] == 'objstr':
            return decode_objstr(a)
        elif a[:i] == 'objfile':
            return decode_objfile(a)
        else:
            return a
    except:
        return a

def get_output():
    t = str(g['__output__'])
    g['__output__'] = ''
    return t

g['echo'] = echo
g['getenv'] = getenv

def process_fail(line_id, char_id):
    print('FATAL error while processing code before %d:%d:' % (line_id + 1, char_id + 1))
    traceback.print_exc(limit=0)
    exit(1)

def process(text):
    lines = text.split('\n')
    output = []
    buffer = []
    block = False
    for line_id in range(len(lines)):
        line = lines[line_id]
        if block:
            if line == '@end':
                try:
                    exec('\n'.join(buffer), g)
                except Exception:
                    process_fail(line_id, 1)
                output.append(get_output())
                buffer = []
                block = False
            else:
                buffer.append(line)
        elif line == '@begin':
            block = True
        else:
            quote = ''
            bracket = 0
            p = 0
            i = 0
            while i < len(line):
                if quote:
                    if line[i] == '\\':
                        i += 1
                    elif line[i] == quote:
                        quote = ''
                elif bracket:
                    if line[i] == '(':
                        bracket += 1
                    elif line[i] == ')':
                        bracket -= 1
                        if bracket == 0:
                            t = ''
                            try:
                                t = str(eval(line[p:i], g))
                            except Exception:
                                process_fail(line_id, i)
                            buffer.append(get_output())
                            buffer.append(t)
                    elif line[i] == '\'' or line[i] == '"' or line[i] == '`':
                        quote = line[i]
                elif line[i:i+2] == '@(':
                    bracket = 1
                    p = i + 2
                    i += 1
                else:
                    buffer.append(line[i])
                i += 1
            output.append(''.join(buffer))
            buffer = []
    return '\n'.join(output)

def conf(fin, fout=''):
    if not fout:
        fout = fin + '1'
    with open(fin, 'r') as f:
        t = f.read()
    t = process(t)
    with open(fout, 'w') as f:
        f.write(t)

if __name__ == '__main__':
    conf(sys.argv[1], sys.argv[2])
