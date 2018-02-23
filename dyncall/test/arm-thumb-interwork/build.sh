cp buildsys/arm-thumb-interwork/Makefile-default.config Makefile.config
make clean all
cp buildsys/arm-thumb-interwork/Makefile-thumb.config Makefile.config
cd test && make all

