#!/usr/bin/env python3
import requests
import os
import hashlib
import pathlib
import json

BLOCK_SIZE = 4096

url = 'http://192.168.1.45:8080/upload'
flist = []
for p in pathlib.Path(os.getcwd() + '/files').iterdir():
    if p.is_file():
        flist.append(p)
files=[eval(f'("file_to_upload", open("{file}", "rb"))') for file in flist ]
hashes = requests.post(url=url, files=files)
# print(hashes.text)
hashed_dct = json.loads(str(hashes.text))

# h = hashlib.blake2b(digest_size=32)
# h.update(b'')
# print(h.hexdigest())

error_in_hashes = False
for file in flist:
    # h = hashlib.blake2b(key=b'my16characterKey', digest_size=32)
    h = hashlib.blake2b(digest_size=32)
    fb = ' '
    with open(file, 'rb') as f: 
        # fb = f.read(BLOCK_SIZE)
        # h.update(fb)
        while len(fb) > 0:
            fb = f.read(BLOCK_SIZE)
            h.update(fb)
    # print(file.name + ' : ' + h.hexdigest())
    if hashed_dct[file.name] != h.hexdigest():
        print("hash error: " + file.name)
        error_in_hashes = True

if error_in_hashes:
    print("Error in hashes")
else:
    print("All files transmitted without errors")
    
