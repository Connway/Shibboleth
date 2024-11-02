import glob
import sys
import os


def GetFilesString(dir_string, extension):
    glob_string = dir_string + "**/*." + extension
    files = glob.glob(glob_string)
    file_list = ""

    for file in files:
        if len(file_list) > 0:
            file_list = "{}  '{}',\n".format(file_list, file)
        else:
            file_list = "\n  '{}',\n".format(file)

    file_list = file_list.replace(dir_string, "")
    file_list = file_list.replace('\\', '/')

    return file_list


if len(sys.argv) < 2:
    print("No folder specified.")
    exit()


dir_string = sys.argv[1].replace('\\', '/')

if dir_string[-1] != '/':
    dir_string = dir_string + '/'

build_file = dir_string + "meson.build"

if not os.path.isfile(build_file):
    print("No 'meson.build' file in '{}'.".format(dir_string))
    exit()

header_files = GetFilesString(dir_string, "h")
source_files = GetFilesString(dir_string, "cpp")

file = open(build_file, "r")

if not file:
    print("Failed to open '{}' for read.".format(build_file))
    exit()

build_file_contents = file.read()
file.close()

file = open(build_file, "w")

if not file:
    print("Failed to open '{}' for write.".format(build_file))
    exit()

header_files_var_string = "header_files = ["
header_files_start = build_file_contents.index(header_files_var_string)
header_files_end = build_file_contents.index("]", header_files_start)

build_file_contents = build_file_contents[:(header_files_start + len(header_files_var_string))] + header_files + build_file_contents[header_files_end:]

source_files_var_string = "source_files = ["
source_files_start = build_file_contents.index(source_files_var_string)
source_files_end = build_file_contents.index("]", source_files_start)

build_file_contents = build_file_contents[:(source_files_start + len(source_files_var_string))] + source_files + build_file_contents[source_files_end:]

file.write(build_file_contents)
file.close()
