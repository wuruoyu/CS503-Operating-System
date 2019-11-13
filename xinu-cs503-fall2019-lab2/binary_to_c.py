#!/usr/bin/python

import sys

# Converts a binary file to a C array
#
# Usage:
#  ./binary_to_c.py hello.elf > hello-contents.c
# 

def binary_to_c(input_filename):
	binary_contents = None
	with open(input_filename, "r") as f:
		binary_contents = f.read()

	i = 0
        if input_filename == "../compile/apphello.elf":
	    print "char apphello_contents[] = {"
        elif input_filename == "../compile/libmy.so":
            print "char libmy_contents[] = {"
        elif input_filename == "../compile/apphello_dyn_loading.elf":
            print "char apphello_dyn_loading_contents[] = {"


        print "\t",
        for x in binary_contents:
                i = i + 1
                print "\'\\x%02x\'," % ord(x),
                if i == 10:
                        print "\n\t",
                        i = 0
        print "};"

        if input_filename == "../compile/apphello.elf":
            print "int apphello_contents_size = sizeof(apphello_contents);"
        elif input_filename == "../compile/libmy.so":
            print "int libmy_contents_size = sizeof(libmy_contents);"
        elif input_filename == "../compile/apphello_dyn_loading.elf":
            print "int apphello_dyn_loading_contents_size = sizeof(apphello_dyn_loading_contents);"



binary_to_c(sys.argv[1])

