import os, sys
import subprocess
import shutil


def init():
    # change directory to cocos root path
    os.chdir(os.path.dirname(__file__))
    os.chdir('..')
    with open('external/filelist.txt', 'r') as f:
        filelist = f.read().split('\n')
    return filelist

def write_7z(filelist, output_filename):
    cmd = ['7z', 'a', output_filename, '-mx9'] + filelist
    subprocess.check_call(cmd, stdout=sys.stdout, stderr=subprocess.STDOUT)

def delete_files(filelist):
    # after packing files to 7z
    for fname in filelist:
        if not os.path.exists(fname):
            print(f'[NOT EXISTS] {fname}')
            continue
        if os.path.isfile(fname):
            print(f'[DELETE FILE]      {fname}')
            os.remove(fname)
        else:
            print(f'[DELETE DIRECTORY] {fname}')
            shutil.rmtree(fname)


if __name__ == '__main__':
    filelist = init()
    # write_7z(filelist, 'cocos2d-x-3-port-external.7z')
    delete_files(filelist)