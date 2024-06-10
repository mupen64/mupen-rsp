CC = gcc
CFLAGS = -DUSE_GTK `gtk-config --cflags` -O3 -mcpu=athlon \
         -ffast-math -funroll-loops -fomit-frame-pointer
#CFLAGS = -DUSE_GTK `gtk-config --cflags` -g -fPIC
CXX = g++
CXXFLAGS = $(CFLAGS)
LD = g++
LDFLAGS =

TARGET = mupen64_hle_rsp_azimer.so
OBJECTS = main.o disasm.o jpeg.o ucode1.o ucode2.o ucode3.o ucode3mp3.o

all:	$(TARGET) instruction

$(TARGET): $(OBJECTS)
	$(LD) -shared -Wl,-Bsymbolic `gtk-config --libs` $(LDFLAGS) -o $@ $(OBJECTS)
	strip --strip-all $@

instruction:
	$(warning please copy $(TARGET) in the plugins/ folder of the emulator)

#.o: .c
#	$(CC) $(CFLAGS) -o $@ $<

#.o: .cpp
#	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(OBJECTS) $(ALL)

rebuild: clean $(ALL)
