echo "Cmake generate project"

echo "step 1 - remove Build directory"
rm -r Build

echo "step 2 - create Build directory"
mkdir Build

echo "step 3 - generate project in Build directory"
cd Build/
# v141 -> VS 2017
# v142 -> VS 2019
cmake -A x64 -T v142 ../
#make
#make install

read -rsp $'Press any key to continue...\n' -n1 key