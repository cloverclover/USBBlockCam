TARGET = USBBlockCam
SDKROOT = $(shell xcrun --sdk macosx --show-sdk-path)
CC = xcrun --sdk macosx clang
CXX = xcrun --sdk macosx clang++

ARCH = x86_64
MIN_MAC = 15.0

CXXFLAGS = -target $(ARCH)-apple-macos$(MIN_MAC) \
           -isysroot $(SDKROOT) \
           -nostdlib \
           -fno-builtin \
           -fno-exceptions \
           -fno-rtti \
           -mkernel \
           -DKERNEL -DKERNEL_PRIVATE -DDRIVER_PRIVATE -DAPPLE -DNeXT \
           -I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/PrivateHeaders \
           -I$(SDKROOT)/System/Library/Frameworks/Kernel.framework/Headers \
           -I$(SDKROOT)/System/Library/Frameworks/IOKit.framework/Headers

LDFLAGS = -target $(ARCH)-apple-macos$(MIN_MAC) \
          -nostdlib \
          -Xlinker -kext \
          -lkmod \
          -lkmodc++ \
          -lcc_kext

SRCS = USBBlockCamDevice.cpp USBBlockCamInterface.cpp
OBJS = $(SRCS:.cpp=.o)

KEXT = $(TARGET).kext
BINARY = $(KEXT)/Contents/MacOS/$(TARGET)

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(OBJS)
	@mkdir -p $(KEXT)/Contents/MacOS
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)
	cp Info.plist $(KEXT)/Contents/Info.plist

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(KEXT) $(OBJS)
