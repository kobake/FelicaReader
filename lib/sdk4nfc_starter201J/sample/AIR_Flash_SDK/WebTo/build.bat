@echo off

SET SRCS=src\WebTo.mxml
SET LIB_DIR=libs
SET RELEASE_DIR="bin-debug"
SET SWF_NAME="WebTo.swf"

mkdir %RELEASE_DIR%
mxmlc.exe -locale ja_JP -debug -library-path+=%LIB_DIR% %SRCS% -output %RELEASE_DIR%\\%SWF_NAME%