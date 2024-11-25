import argparse
import pathlib
import os

parser = argparse.ArgumentParser(prog="shibboleth_post_install", description="Does some small file cleanup when running install command.")

parser.add_argument(
    "-r", "--root",
    type=pathlib.Path,
    help="The folder to run the post install cleanup on.",
)

args = parser.parse_args()

library_extensions = [".a", "lib"]

root = pathlib.Path(".")

if args.root:
    root = args.root

os.rmdir(root / "lib")

for ext in library_extensions:
    library_files = sorted(root.glob("**/*" + ext))

    for lib_file in library_files:
        os.remove(lib_file)
