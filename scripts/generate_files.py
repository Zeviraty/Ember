import os
import shutil
import sys

BST_REMAP = {
    'MART': 'LOBBY',
    'DOJO': 'GYM',
    'MUSEUM': 'FACILITY',
    'FOREST_GATE': 'GATE',
    'REDS_HOUSE_1': 'REDS_HOUSE',
    'REDS_HOUSE_2': 'REDS_HOUSE',
}

def gen() -> int:
    if shutil.which("git") in (None, ""):
        sys.stdout.write("Git not found / installed\n")
        return 1

    if os.path.exists("data"):
        inp = input("Data folder already exists, overwrite it? (y/N): ")
        if inp.lower() in ("y","yes","ok"):
            shutil.rmtree("data")
        else:
            return 1

    os.mkdir("data")
    os.system("git clone https://github.com/pret/pokered data/pokered --depth=1")

    HEADER_START = 'map_header '
    HEADER_END = 'end_map_header'
    MAP_CONST = 'map_const'
    HEADER_FOLDER = 'data/pokered/data/maps/headers'
    CONNECTION = 'connection '
    os.mkdir("data/maps")

    constants = open('data/pokered/constants/map_constants.asm','r').read()
    SIZES = {}
    for line in constants.splitlines():
        if not line.strip().startswith(MAP_CONST):
            continue
        parts = []
        for part in line[:line.index(';')].strip().split(','):
            spl = part.split()
            if spl[0] == MAP_CONST: spl.pop(0)
            parts.append(spl[0])
        SIZES[parts[0]] = {'x': parts[1], 'y': parts[2]}

    headers = os.listdir(HEADER_FOLDER)
    length = len(headers)

    for idx, header in enumerate(headers):
        data = {
            "ReadableName": '',
            "ID": '',
            "BlockSet": 'OVERWORLD',
            "North": 0,
            "East":  0,
            "South": 0,
            "West":  0
        }

        for line in open(HEADER_FOLDER+os.sep+header,'r').read().splitlines():
            if line.strip() == HEADER_END:
                break
            elif line.strip().startswith(HEADER_START):
                parts = line.strip()[len(HEADER_START):] \
                        .replace(', ',',') \
                        .split(',')
                data['ReadableName'] = parts[0]
                data['ID'] = parts[1]
                bst = parts[2]
                bst = BST_REMAP.get(bst, bst)
                data['BlockSet'] = bst
            elif line.strip().startswith(CONNECTION):
                parts = line.strip()[len(CONNECTION):] \
                        .replace(', ',',') \
                        .split(',')
                connection = {}
                connection['ReadableName'] = parts[1]
                connection['ID'] = parts[2]
                connection['offset'] = int(parts[3].split(' ')[0])
                data[parts[0].title()] = connection

        f = open('data/maps/'+data['ReadableName'],'wb')

        f.write(data['ReadableName'].encode()) # Name
        f.write(b'\x00')                       # Null byte seperator
        f.write(data['ID'].encode())           # ID
        f.write(b'\x00')                       # Null byte seperator
        f.write(data['BlockSet'].encode())     # Blockset used by map
        f.write(b'\x00')                       # Null byte seperator
        for dir in ("North","East","South","West"):
            if data[dir] == 0:
                f.write(b'0\x00') # No connection with null byte seperator
                continue
            f.write(data[dir]['ReadableName'].encode())
            f.write(b'\x01') # list seperator
            f.write(data[dir]['ID'].encode())
            f.write(b'\x01') # list seperator
            f.write(str(data[dir]['offset']).encode())
            f.write(b'\x00') # Null byte seperator

        f.write(str(SIZES[data['ID']]['x']).encode())
        f.write(b'\x00')
        f.write(str(SIZES[data['ID']]['y']).encode())
        f.write(b'\x00')

        f.write(open(f'data/pokered/maps/{data["ReadableName"].removesuffix("Copy")}.blk','rb').read())

        sys.stdout.write(f'\33[2K\r')
        sys.stdout.write(f'Wrote map {idx+1}/{length}')

    sys.stdout.write('\n')

    bst_path = 'data/bst'
    shutil.copytree('data/pokered/gfx/blocksets',bst_path)
    shutil.copytree('data/pokered/gfx/tilesets','data/tilesets')
    shutil.copytree('data/pokered/gfx/sprites','data/sprites')
    for file in os.listdir(bst_path):
        shutil.move(bst_path+os.sep+file,bst_path+os.sep+file.removesuffix('.bst').upper())
 
    #shutil.rmtree("data/pokered")
    return 0

if __name__ == '__main__':
    exit(gen())
