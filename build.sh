if [ ! -d "/build" ];then
  mkdir build
fi

if [ ! -d "/bin" ];then
  mkdir bin
fi

if [ ! -d "/log"];then
  mkdir log
fi

if [ ! -d "client/bin"];then
  mkdir client/bin
fi

cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=YES ..
make install