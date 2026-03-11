COLLISIONS = {}
n = ""
d = open("data/pokered/data/tilesets/collision_tile_ids.asm",'r').read()

BST_REMAP = {
    'MART': 'LOBBY',
    'DOJO': 'GYM',
    'MUSEUM': 'FACILITY',
    'FOREST_GATE': 'GATE',
    'REDS_HOUSE_1': 'REDS_HOUSE',
    'REDS_HOUSE_2': 'REDS_HOUSE',
}

for line in d.splitlines():
    if line.strip().startswith("coll_tiles"):
        p = line.strip().replace(" ",",").split(',')
        while '' in p: p.remove('')
        if p[1] == ';': continue
        COLLISIONS[n] = p[1:]
    elif "::" in line:
        n = line.strip().split("::")[0].upper()
        if n in BST_REMAP: n = BST_REMAP[n]

for k,v in COLLISIONS.items():
    s = ""
    for i in v:
        s += i.replace("$",'0x')+', '
    print("static unsigned char "+k+"[] = { "+s+"};")

print()
print("struct { char *key; unsigned char value; } *colls = NULL;")
print()

print("void setup_collision() {")

for k,v in COLLISIONS.items():
    print(f"  shput(colls,\"{k.removesuffix('_COLL')}\",*{k});")


print("}")
