import os
import glob
# import json

start_dir = "P:\\Project\\WY_TianYu\\GuangRen\\Shot"


def find_all(folder):
    out_list = list()

    def find_nk(path):
        f_list = glob.glob(os.path.join(path, '*'))
        for f in f_list:
            if os.path.isdir(f):
                find_nk(f)
            elif os.path.isfile(f):
                if f.endswith('.nk'):
                    print(f)
                    # out_list.append(f.decode('gbk'))
                    out_list.append(f)
        return True

    find_nk(folder)
    return out_list

def find_dir():
    aaa = find_all(start_dir)

    with open('out.txt', 'w') as f:
        for a in aaa:
            f.write(a)
            f.write('\n')

if __name__ == "__main__":
    pass
