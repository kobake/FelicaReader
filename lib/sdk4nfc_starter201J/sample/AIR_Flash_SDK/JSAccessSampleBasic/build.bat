@echo off

SET SRCS=src\JSAccessSampleBasic.mxml
SET LIB_DIR=libs
SET RELEASE_DIR="bin-debug"
SET SWF_NAME="JSAccessSampleBasic.swf"

mkdir %RELEASE_DIR%
xcopy /Y html-template %RELEASE_DIR%
move  %RELEASE_DIR%\\index.template.html %RELEASE_DIR%\\index.html
mxmlc.exe -locale ja_JP -debug -library-path+=%LIB_DIR% %SRCS% -output %RELEASE_DIR%\\%SWF_NAME%