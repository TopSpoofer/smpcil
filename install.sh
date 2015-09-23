#! /bin/bash

if [[ -d "/usr/include/smpcil" ]]; then
	rm -rf /usr/include/smpcil
fi

mkdir /usr/include/smpcil
cp -r smp_linux /usr/include/smpcil/
cp -r smp_mpool /usr/include/smpcil/
cp -r smp_stl /usr/include/smpcil/
cp smpcil.h /usr/include/smpcil/

rm  -rf /usr/include/smpcil/*/*.c
rm -rf /usr/include/smpcil/*/*/*.c