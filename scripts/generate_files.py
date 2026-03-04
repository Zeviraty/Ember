import os
import shutil
import sys
from pprint import pp

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
    HEADER_FOLDER = 'data/pokered/data/maps/headers'
    CONNECTION = 'connection '
    os.mkdir("data/maps")

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
                data['BlockSet'] = parts[2]
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
        f.write(open(f'data/pokered/gfx/maps/{data["ReadableName"]}','rb').read())

        sys.stdout.write(f'\33[2K\r')
        sys.stdout.write(f'Wrote map {idx+1}/{length}')

    sys.stdout.write('\n')
 
    #shutil.rmtree("data/pokered")
    return 0

if __name__ == '__main__':
    exit(gen())
