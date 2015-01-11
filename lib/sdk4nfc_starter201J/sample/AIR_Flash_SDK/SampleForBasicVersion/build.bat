

SET SRCS=src\FeliCaSample.mxml

SET LIB_DIR=libs
SET RELEASE_DIR="bin-debug"
SET SWF_NAME="SampleForBasicVersion.swf"

mkdir %RELEASE_DIR%
mxmlc.exe -locale ja_JP -debug -source-path=.\src -library-path+=%LIB_DIR% %SRCS% -output %RELEASE_DIR%\\%SWF_NAME%