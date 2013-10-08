#! /usr/bin/env python
import sys
import os
import subprocess

def sorted_files(dir_path):
    return sorted(os.listdir(dir_path), key=lambda x: int(x.replace('.input', '').replace('test', '').replace('bad', '')))

if __name__ == "__main__" :
    if not len(sys.argv) == 3:
        print >>sys.stdout, """
usage: %s executable test_dir

the executable should be an executable to run, and test_dir has a bunch
of files in it (expected inputs and outputs)
""" % sys.argv[0]

        sys.exit(1)

    executable = os.path.abspath(sys.argv[1])
    test_dir = os.path.abspath(sys.argv[2])

    # legal test cases
    legal_dir = os.path.join(test_dir, 'Legal')
    for filename in sorted_files(legal_dir):
        print "Test %s" % filename
        cmd = "%s < %s" % (executable, os.path.join(legal_dir, filename))
        print "  " + cmd
        os.system(cmd)
        print ""

    print "=" * 80

    # illegal test cases
    illegal_dir = os.path.join(test_dir, 'Illegal')
    for filename in sorted_files(illegal_dir):
        print "Test %s" % filename
        cmd = "%s < %s" % (executable, os.path.join(illegal_dir, filename))
        print "  " + cmd
        os.system(cmd)
        print ""
