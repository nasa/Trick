from pathlib import Path
import os

def_header_ext = ["h", "hh", "hpp", "H", "hxx", "h++"]
def_src_ext = ["cpp", "c"]

def find_files_by_extension(loc, ext):
    path = Path(loc)
    files = list(path.rglob(f'*.{ext}'))
    return files

def build_S_source():
    loc = ""
    if "TRICKIFY_HEADER" in os.environ:
        loc = os.getenv("TRICKIFY_HEADER")

    s_source = open("S_source.hh", 'w')
    for ext in def_header_ext:
        files = find_files_by_extension(loc, ext)
        for i in range(len(files)):
            s_source.write('#include "' + str(files[i]) + '"\n')

def build_obj_list():
    loc = ""
    if "TRICKIFY_SOURCE" in os.environ:
        loc = os.getenv("TRICKIFY_SOURCE")
    work_dir = ""
    if "TRICKIFY_WORK_DIR" in os.environ:
        work_dir = os.getenv("TRICKIFY_WORK_DIR")

    files = find_files_by_extension(loc, "o")
    s_source = open(work_dir + "trickify_obj_list", 'w')
    for i in range(len(files)):
        #s_source.write(work_dir + "/../" + str(files[i]) + '\n')
        s_source.write(work_dir + str(files[i]) + '\n')

def build_src_list():
    loc = ""
    if "TRICKIFY_SOURCE" in os.environ:
        loc = os.getenv("TRICKIFY_SOURCE")
    work_dir = ""
    if "TRICKIFY_WORK_DIR" in os.environ:
        work_dir = os.getenv("TRICKIFY_WORK_DIR")

    for ext in def_src_ext:
        files = find_files_by_extension(loc, ext)
        s_source = open(work_dir + "trickify_src_list", 'w')
        for i in range(len(files)):
            #s_source.write(work_dir + "/../" + str(files[i]) + '\n')
            s_source.write(work_dir + str(files[i]) + '\n')
