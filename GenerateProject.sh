echo "Cmake generate project"

echo "step 1 - remove Build directory"
rm -r Build

echo "step 2 - create Build directory"
mkdir Build

echo "step 3 - generate project in Build directory"
cd Build/
cmake ../ -A x64 -T v141
#make
#make install

read -rsp $'Press any key to continue...\n' -n1 key