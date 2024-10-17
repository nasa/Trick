from pathlib import Path
import os

def_ext = ["h", "hh", "hpp", "H", "hxx", "h++"]

loc = ""
if "TRICKIFY_SOURCE" in os.environ:
    loc = os.getenv("TRICKIFY_SOURCE")

def find_files_by_extension(loc, ext):
    path = Path(loc)
    files = list(path.rglob(f'*.{ext}'))
    return files

files = find_files_by_extension("/users/plherrin/trick/include/", "hh")
s_source = open("S_source.hh", 'w')
for i in range(len(files)):
    s_source.write('#include "' + str(files[i]) + '"\n')
