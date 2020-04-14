echo "Cmake generate project"

echo "step 1 - remove Build directory"
rm -r Build

echo "step 2 - create Build directory"
mkdir Build

echo "step 3 - generate project in Build directory"
cd Build/
cmake ../ -A x64