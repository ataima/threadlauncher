rm -rf Debug*
rm -rf Release*
rm -rf dist
rm -rf CMakeFiles
rm -rf .idea
rm -rf build
find . -name CMakeFiles -exec rm -rf {} +
find . -name cmake_install.cmake -exec rm {} +
find . -name CMakeCache.txt  -exec rm {} +
find . -name Makefile  -exec rm {} +
git status > /dev/null  2&>1
if [ $? -eq 0 ]
then
    ALL=$(git ls-files --others --exclude-standard)
    for ii in $ALL ; do rm -rf $ii; done
fi

