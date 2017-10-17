TYPE="Debug"

if [ ! -z "$1" ]; then
    if [ "$1" = "release" ]; then
        TYPE="Release"
    fi
fi

./build/Vii -stdc
g++ -c io.cpp
./build/Vii -w
g++ -o io.o -c test.cpp
g++ -o test io.o
