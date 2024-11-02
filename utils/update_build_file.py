import argparse
import pathlib

parser = argparse.ArgumentParser(
    prog="update_build_file",
    description="Updates the meson.build file of a directory with an updated list of source files."
)

parser.add_argument("directory", type=pathlib.Path)
parser.add_argument(
    "-he", "--header_extensions",
    type=str,
    nargs="*",
    help="A list of file extensions to look for and add to the header_files array."
)
parser.add_argument(
    "-se", "--source_extensions",
    type=str,
    nargs="*",
    help="A list of file extensions to look for and add to the source_files array."
)
parser.add_argument(
    "-efe", "--extra_files_extensions",
    type=str,
    nargs="*",
    help="A list of file extensions to look for and add to the extra_files array."
)
parser.add_argument(
    "-rd", "--recursive_depth",
    default=-1,
    type=int,
    help="How many folders deep we will search for a file type."
)

args = parser.parse_args()


def GetFiles(directory, extension, depth):
    if args.recursive_depth >= 0 and depth > args.recursive_depth:
        return []

    files = sorted(directory.glob("*." + extension))
    paths = sorted(directory.glob("*"))

    for path in paths:
        if path.is_dir():
            files.extend(GetFiles(path, extension, depth + 1))

    return files


def GetFilesString(list_name, extensions, ignore_recursion=False):
    original_recursive_depth = args.recursive_depth
    file_list = ""

    if ignore_recursion:
        args.recursive_depth = -1

    files = []

    for ext in extensions:
        if args.recursive_depth < 0:
            files.extend(sorted(args.directory.glob("**/*." + ext)))
        else:
            files.extend(GetFiles(args.directory, ext, 0))

    for file in files:
        file_list = "{}  '{}',\n".format(file_list, file.as_posix().replace(args.directory.as_posix() + "/", ""))

    if ignore_recursion:
        args.recursive_depth = original_recursive_depth

    return "{} = [\n{}".format(list_name, file_list)


def ModifyBuildString(build_file_contents, files, var_string):
    var_start = build_file_contents.index(var_string)
    var_end = build_file_contents.index("]", var_start)

    return build_file_contents[:var_start] + files + build_file_contents[var_end:]


build_file = args.directory / "meson.build"

if not build_file.exists():
    print("No 'meson.build' file in '{}'.".format(args.directory.as_posix()))
    exit()

if not build_file.is_file():
    print("'meson.build' in '{}' is not a file.".format(args.directory.as_posix()))
    exit()

header_extensions = ["h", "hpp"] + (args.header_extensions or [])
source_extensions = ["cpp"] + (args.source_extensions or [])
extra_files_extensions = ["inl", "natvis"] + (args.extra_files_extensions or [])

header_files = GetFilesString("header_files", header_extensions, True)
source_files = GetFilesString("source_files", source_extensions)
extra_files = GetFilesString("extra_files", extra_files_extensions, True)

build_file_contents = build_file.read_text()

build_file_contents = ModifyBuildString(build_file_contents, header_files, "header_files = [")
build_file_contents = ModifyBuildString(build_file_contents, source_files, "source_files = [")

if len(extra_files) > 0:
    build_file_contents = ModifyBuildString(build_file_contents, extra_files, "extra_files = [")

build_file.write_text(build_file_contents)
