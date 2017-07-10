CC = g++
RM = del

SDK_ROOT = C:/Users/Tejada/Library-SDKs

SFML_INC = -I $(SDK_ROOT)/SFML/include
SFML_EXT = -L $(SDK_ROOT)/SFML/extlibs/libs-mingw/x86 -ljpeg -lfreetype -lopenal32 -lflac -lvorbisenc -lvorbisfile -lvorbis -logg
SFML_LIB = -L $(SDK_ROOT)/SFML/lib/mingw -lsfml-graphics-s -lsfml-window-s -lsfml-audio-s -lsfml-network-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32 -lws2_32

RELEASE_FLAGS = -DSFML_STATIC -static-libstdc++ -static-libgcc -static -lpthread -s
DEBUG_FLAGS = -DSFML_STATIC -static-libstdc++ -static-libgcc -static -lpthread -Wall -g

SOURCE = ./source/*.cpp
OUTPUT = Example

release: $(SOURCE)
	@echo Building release
	@$(CC) $(SOURCE) $(SFML_INC) $(SFML_LIB) $(SFML_EXT) $(RELEASE_FLAGS) -o ./bin/$(OUTPUT).exe

debug: $(SOURCE)
	@echo Building debug
	@$(CC) $(SOURCE) $(SFML_INC) $(SFML_LIB) $(SFML_EXT) $(DEBUG_FLAGS) -o ./bin/$(OUTPUT)Dbg.exe

clean:
	cd bin && @$(RM) *.o *.exe
