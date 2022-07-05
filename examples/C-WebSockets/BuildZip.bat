@echo off
rem Zips together the data in the html directory and produces html.zip.c for 'release build'. Make sure to compile BAS\tools\bin2c.c prior to running this batch file. bin2c.exe and zip.exe must be in your path env. var.

cd html || pause
zip -D -u -r -9 ..\obj\release\html.zip * || pause
cd ..\obj\release
bin2c -z getZipReader html.zip html.zip.c || pause

echo done
pause



