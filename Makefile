CFLAGS=-Wall

INCDIR=-I./em_test_3/em_test_3 \
-I./em_test_3/em_test_3/Game \
-I./em_test_3/em_test_3/Common \
-I./em_test_3/em_test_3/Common/imagelib/png \
-I./em_test_3/em_test_3/Common/imagelib/zlib \
-I./em_test_3/em_test_3/Ananas \
-I./em_test_3/em_test_3/Ananas/Actions \
-I./em_test_3/em_test_3/Ananas/pugi \
-I./em_test_3/em_test_3/Ananas/xml \
-I./em_test_3/em_test_3/Ananas/jsoncpp

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
./em_test_3/em_test_3/Common/Utils.cpp \
./em_test_3/em_test_3/Common/FileLoader.cpp \
./em_test_3/em_test_3/Common/Trace.cpp \
./em_test_3/em_test_3/Common/esUtils.cpp \
./em_test_3/em_test_3/Common/MouseHandler.cpp \
./em_test_3/em_test_3/Ananas/CDirector.cpp \
./em_test_3/em_test_3/Ananas/CNode.cpp \
./em_test_3/em_test_3/Ananas/EventHandler.cpp \
./em_test_3/em_test_3/Ananas/Actions/CAction.cpp \
./em_test_3/em_test_3/Ananas/Actions/CActionModifiers.cpp \
./em_test_3/em_test_3/Ananas/Actions/CActionRepeate.cpp \
./em_test_3/em_test_3/Ananas/Actions/CActionRunEvent.cpp \
./em_test_3/em_test_3/Ananas/Actions/CActionSequence.cpp \
./em_test_3/em_test_3/Ananas/Actions/CActionSimultaneously.cpp \
./em_test_3/em_test_3/Ananas/Actions/CActionWaiting.cpp \
./em_test_3/em_test_3/Ananas/ShaderManager.cpp \
./em_test_3/em_test_3/Ananas/pugi/pugixml.cpp \
./em_test_3/em_test_3/Ananas/jsoncpp/json_reader.cpp \
./em_test_3/em_test_3/Ananas/jsoncpp/json_value.cpp \
./em_test_3/em_test_3/Ananas/jsoncpp/json_writer.cpp \
./em_test_3/em_test_3/Ananas/xml/tinystr.cpp \
./em_test_3/em_test_3/Ananas/xml/tinyxml.cpp \
./em_test_3/em_test_3/Ananas/xml/tinyxmlerror.cpp \
./em_test_3/em_test_3/Ananas/xml/tinyxmlparser.cpp \
./em_test_3/em_test_3/Ananas/CSprite.cpp \
./em_test_3/em_test_3/Ananas/Render.cpp \
./em_test_3/em_test_3/Ananas/CImage.cpp \
./em_test_3/em_test_3/Ananas/GraphicsLoader.cpp \
./em_test_3/em_test_3/Ananas/CButton.cpp \
./em_test_3/em_test_3/Ananas/TexturePackerLoader.cpp \
./em_test_3/em_test_3/Ananas/CAnimation.cpp \
./em_test_3/em_test_3/Game/Element.cpp \
./em_test_3/em_test_3/GameScene.cpp \

MAIN_SRC=./em_test_3/em_test_3/main.cpp

all: ./output/main.bc

./output/main.bc: zlib.o png.o ${SOURCES} ${MAIN_SRC}
	$(CXX) $(CFLAGS) ${SOURCES} ${MAIN_SRC} ./output/png.o ./output/zlib.o -o $@ ${INCDIR} -std=c++11

png.o: ${PNG_SOURCES}
	$(CC) -Wall ${PNG_SOURCES} -o ./output/png.o ${PNG_INCDIR}

zlib.o: ${ZLIB_SOURCES}
	$(CC) -Wall ${ZLIB_SOURCES} -o ./output/zlib.o

clear:
	find . -name "*.bc" | xargs rm -f
