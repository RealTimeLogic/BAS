#!/bin/bash

# Zips together the data in the html directory and produces html.zip.c
# for 'release build'. Make sure to compile BAS/tools/bin2c.c prior to
# running this batch file. bin2c and zip must be in your path
# env. var.

function failed() {
    echo "$1 failed"
    sleep 5
    exit 1
}

rm -f obj/release/html.zip
cd html || failed "cd"
zip -D -r -9 ../obj/release/html.zip * || failed "zip"
cd ../obj/release
bin2c -z getZipReader html.zip html.zip.c || failed "bin2c"

echo done
