image_to_c Copyright (c) 2020 BitBank Software, Inc.
Written by Larry Bank

Usage: image_to_c <option> <filename>
output is written to stdout
example:

image_to_c ./test.jpg > test.h
image_to_c --strip ./test.tif > test.h
--strip = remove all metadata and just save the compressed image
This option is only available for TIFF & BMP files (for now)
