import os

path = ""
if "TRICK_HOME" in os.environ:
    path = os.getenv("TRICK_HOME")
path += "/share/trick/pymods/trick/build_trickify.py"

exec(open(path).read())

build_obj_list()
