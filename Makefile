CFLAGS=-Wall

INCDIR=-I./em_test_3/em_test_3 \
-I./em_test_3/em_test_3/Common \
-I./em_test_3/em_test_3/Common/imagelib/png \
-I./em_test_3/em_test_3/Common/imagelib/zlib

PNG_INCDIR=-I./em_test_3/em_test_3/Common/imagelib/zlib

PNG_SOURCES=./em_test_3/em_test_3/Common/imagelib/png/png.c \
./em_test_3/em_test_3/Common/imagelib/png/pngerror.c \
./em_test_3/em_test_3/Common/imagelib/png/pngget.c \
./em_test_3/em_test_3/Common/imagelib/png/pngmem.c \
./em_test_3/em_test_3/Common/imagelib/png/pngpread.c \
./em_test_3/em_test_3/Common/imagelib/png/pngread.c \
./em_test_3/em_test_3/Common/imagelib/png/pngrio.c \
./em_test_3/em_test_3/Common/imagelib/png/pngrtran.c \
./em_test_3/em_test_3/Common/imagelib/png/pngrutil.c \
./em_test_3/em_test_3/Common/imagelib/png/pngset.c \
./em_test_3/em_test_3/Common/imagelib/png/pngtrans.c \
./em_test_3/em_test_3/Common/imagelib/png/pngwio.c \
./em_test_3/em_test_3/Common/imagelib/png/pngwrite.c \
./em_test_3/em_test_3/Common/imagelib/png/pngwtran.c \
./em_test_3/em_test_3/Common/imagelib/png/pngwutil.c

ZLIB_SOURCES=./em_test_3/em_test_3/Common/imagelib/zlib/adler32.c \
./em_test_3/em_test_3/Common/imagelib/zlib/compress.c \
./em_test_3/em_test_3/Common/imagelib/zlib/crc32.c \
./em_test_3/em_test_3/Common/imagelib/zlib/deflate.c \
./em_test_3/em_test_3/Common/imagelib/zlib/gzclose.c \
./em_test_3/em_test_3/Common/imagelib/zlib/gzlib.c \
./em_test_3/em_test_3/Common/imagelib/zlib/gzread.c \
./em_test_3/em_test_3/Common/imagelib/zlib/gzwrite.c \
./em_test_3/em_test_3/Common/imagelib/zlib/infback.c \
./em_test_3/em_test_3/Common/imagelib/zlib/inffast.c \
./em_test_3/em_test_3/Common/imagelib/zlib/inflate.c \
./em_test_3/em_test_3/Common/imagelib/zlib/inftrees.c \
./em_test_3/em_test_3/Common/imagelib/zlib/trees.c \
./em_test_3/em_test_3/Common/imagelib/zlib/uncompr.c \
./em_test_3/em_test_3/Common/imagelib/zlib/zutil.c

SOURCES=./em_test_3/em_test_3/Scene.cpp \
./em_test_3/em_test_3/Common/algebra3.cpp \
./em_test_3/em_test_3/Common/FileLoader.cpp \
./em_test_3/em_test_3/Common/Trace.cpp \
./em_test_3/em_test_3/Common/esUtils.cpp \
./em_test_3/em_test_3/Common/MouseHandler.cpp

MAIN_SRC=./em_test_3/em_test_3/main.cpp

all: ./output/main.bc

./output/main.bc: zlib.o png.o ${SOURCES} ${MAIN_SRC}
	$(CXX) $(CFLAGS) ${SOURCES} ${MAIN_SRC} ./output/png.o ./output/zlib.o -o $@ ${INCDIR}

png.o: ${PNG_SOURCES}
	$(CC) -Wall ${PNG_SOURCES} -o ./output/png.o ${PNG_INCDIR}

zlib.o: ${ZLIB_SOURCES}
	$(CC) -Wall ${ZLIB_SOURCES} -o ./output/zlib.o

clear:
	find . -name "*.bc" | xargs rm -f
