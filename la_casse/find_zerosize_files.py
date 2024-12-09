




# candidate folders
#   /desk
#   /root

import os, sys

if __name__ == '__main__':

    path = sys.argv[-1]

    if len(sys.argv) < 2:
        print('path is not set!')
        exit()

    if not os.path.isdir(path):
        print(f'path is not folder! {path}')
        exit()

    found_filepaths = []
    for curdir, dirs, files in os.walk(path):
        for file in files:
            filepath = os.path.join(curdir, file)
            size = os.path.getsize(filepath)
            if size == 0:
                found_filepaths.append(filepath)
                print(f'! {filepath}')

    if not found_filepaths:
        print(f'zero-size files not found in {path}')

