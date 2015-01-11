@echo off

SET SRCS=src\SmartPosterSampleBasic.mxml
SET APP_XML=src\SmartPosterSampleBasic-app.xml
SET LIB_DIR=libs
SET RELEASE_DIR="bin-debug"
SET SWF_NAME="SmartPosterSampleBasic.swf"

mkdir %RELEASE_DIR%
copy %APP_XML% %RELEASE_DIR%
amxmlc -locale ja_JP -debug -library-path+=%LIB_DIR% %SRCS% -output %RELEASE_DIR%\\%SWF_NAME%
