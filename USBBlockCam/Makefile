TARGET = USBBlockCam
SDKROOT = $(shell xcrun --sdk macosx --show-sdk-path)
CC = xcrun --sdk macosx clang
CXX = xcrun --sdk macosx clang++

ARCH = x86_64
MIN_MAC = 15.0

COMMONFLAGS = -target $(ARCH)-apple-macos$(MIN_MAC) \
              -isysroot $(SDKROOT) \
              -nostdlib \
              -fno-builtin \
              -mkernel \
              -DKERNEL -DKERNEL_PRIVATE -DDRIVER_PRIVATE -DAPPLE -DNeXT \
              -I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/PrivateHeaders \
              -I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/Headers \
              -I$(SDKROOT)/System/Library/Frameworks/IOKit.framework/Headers

CFLAGS   = $(COMMONFLAGS)
CXXFLAGS = $(COMMONFLAGS) \
           -fno-exceptions \
           -fno-rtti

LDFLAGS = -target $(ARCH)-apple-macos$(MIN_MAC) \
          -nostdlib \
          -Xlinker -kext \
          -lkmod \
          -lkmodc++ \
          -lcc_kext

OBJS = kmod_info.o USBBlockCamDevice.o USBBlockCamInterface.o

KEXT = $(TARGET).kext
BINARY = $(KEXT)/Contents/MacOS/$(TARGET)

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(OBJS)
	@mkdir -p $(KEXT)/Contents/MacOS
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)
	cp Info.plist $(KEXT)/Contents/Info.plist

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(KEXT) $(OBJS)
