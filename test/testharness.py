#!/usr/bin/env python

"""Run through a series of tokenout tests."""

import sys
import os
import difflib
import shutil

GENMODE = 0

def check(filename, access) :
    if GENMODE == 1 : return

    """Make sure the file is available"""
    if access=='r' :
        if not os.path.isfile(filename) :
            raise Exception, "File doesn't exist! %s", filename
    if access=='w' :
        if os.path.isfile(filename) :
            os.remove(filename)



def diff_files(filename, output_file, expected_file, msg) :
    """Diff two files and see if they differ or not"""

    # Grab total files
    output       = file(output_file, 'r').readlines()
    expected     = file(expected_file, 'r').readlines()

    different = False
    for line in difflib.context_diff(output, expected, fromfile=output_file, tofile=expected_file):
        different = True
        sys.stdout.write(line) 

    if not different :
        print ' ',msg, 'matched. Passed', filename
    else :
        print '********* TEST FAILED ', msg, filename

    os.remove(output_file)


if __name__ == "__main__" :
    
    # print sys.argv, len(sys.argv)
    if not len(sys.argv) == 3:
        print >>sys.stdout, """
usage: %s executable test_dir

the executable should be an executable to run, and test_dir has a bunch
of files in it (expected inputs and outputs)
""" % sys.argv[0]

        sys.exit(1)

    executable = sys.argv[1]
    test_dir   = sys.argv[2]

    # get files to run thru
    filenames = os.listdir(test_dir)
    filenames.sort()
    os.chdir(test_dir)

    # change to test dir and run
    for filename in filenames :

        if not(filename.endswith(".input") and filename.startswith("test")) :
            # print '... ignoring ', filename
            continue
        # Filename found
        print 'Test found', filename

        basename = filename.split(".")[0]
        input_file = basename + ".input"
        output_file = basename + ".output"
        error_file = basename + ".error"
        expected_outfile = basename + ".outexpected"
        expected_errfile = basename + ".errexpected"

        # Make sure all files are in good shape
        check(input_file, 'r')
        check(output_file, 'w') 
        check(expected_outfile, 'r') 
        check(expected_errfile, 'r') 

        # Run your program under spim
        #print os.getcwd()
        s = "%s < %s 1> %s 2> %s" % (executable, input_file, output_file, error_file)
        # print s
        os.system(s)

        if GENMODE :
            print 'IN GEN MODE'
            shutil.copy(output_file, expected_outfile)
            shutil.copy(error_file, expected_errfile)
            continue

        # see how much they diff: both stderr and stdout!!
        diff_files(filename, output_file, expected_outfile, 'stdout')
        diff_files(filename, error_file, expected_errfile,  'stderr')




        

        

    
    
