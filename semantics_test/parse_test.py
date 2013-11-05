#! /usr/bin/env python
import sys
import os
import subprocess

def sorted_files(dir_path):
    return sorted(os.listdir(dir_path))

if __name__ == "__main__" :
    if not len(sys.argv) >= 3:
        print >>sys.stdout, """
usage: %s executable test_dir

the executable should be an executable to run, and test_dir has a bunch
of files in it (expected inputs and outputs)
""" % sys.argv[0]

        sys.exit(1)

    executable = os.path.abspath(sys.argv[1])
    test_dir = os.path.abspath(sys.argv[2])

    show_legal = show_illegal = True

    if len(sys.argv) > 3:
        test_type = sys.argv[3]
        if test_type == "legal":
            show_illegal = False
        if test_type == "illegal":
            show_legal = False

    # legal test cases
    if show_legal:
        num_passed = num_failed = 0
        legal_dir = os.path.join(test_dir, 'Legal')
        for filename in sorted_files(legal_dir):
            print "Test %s" % filename
            with open(os.path.join(legal_dir, filename)) as f:
                print f.read()
            cmd = "%s < %s" % (executable, os.path.join(legal_dir, filename))
            #print "  " + cmd
            return_value = os.system(cmd)
            if return_value == 0:
                num_passed = num_passed + 1
                print "  PASSED"
            else:
                num_failed = num_failed + 1
                print "  FAILED"
                print "To debug: "
                print "debug < %s" % os.path.join(legal_dir, filename)
            print ""

        print "=" * 80
        print ""
        print "Passed %d / %d " % (num_passed, num_passed+num_failed) + "test cases"
        print ""
        print "=" * 80

    if show_illegal:
        # illegal test cases
        illegal_dir = os.path.join(test_dir, 'Illegal')
        for filename in sorted_files(illegal_dir):
            print "Test %s" % filename
            with open(os.path.join(illegal_dir, filename)) as f:
                print f.read()
            print ""
            cmd = "%s < %s" % (executable, os.path.join(illegal_dir, filename))
            print "  " + cmd
            os.system(cmd)
            print " "
            print " "
