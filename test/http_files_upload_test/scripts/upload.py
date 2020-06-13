#!/usr/bin/env python3

import requests
import os
import hashlib
import pathlib
import json

# Change to where you http_upload_files_test is running
url = 'http://localhost:8080/upload'

flist = []
for p in pathlib.Path(os.getcwd() + '/files').iterdir():
    if p.is_file():
        flist.append(p)

files=[eval(f'("file_to_upload", open("{file}", "rb"))') for file in flist ]
hashes = requests.post(url=url, files=files)
print(hashes.text)
hashed_dct = json.loads(str(hashes.text))

hashes_ok = True

for file in flist:
    h = hashlib.blake2b(digest_size=32)
    
    with open(file, 'rb') as f: 
        h.update(f.read())

    print(file.name + ' : ' + h.hexdigest())

    if hashed_dct[file.name] != h.hexdigest():
        print("Hash error: " + file.name)
        hashes_ok = False

if hashes_ok:
    print("All files transmitted without errors!")
