# Makefile for WindowsCommandManager

# Compiler and compiler flags
CXX = g++
CXXFLAGS = -std=c++11

# Source files and target executable
SRCS = remote_test.cpp
TARGET = RequestManager.exe

# Libraries to link
LIBS = -lole32 -lpowrprof -luuid -lws2_32 -lwsock32 

WINDOWS_SDK_DIR = "/mnt/c/Program Files (x86)/Windows Kits/10"

# Windows include directories 
WIN_INCLUDE_UM = -I$(WINDOWS_SDK_DIR)/Include/10.0.22621.0/um
WIN_INCLUDE_SHARED = -I$(WINDOWS_SDK_DIR)/Include/10.0.22621.0/shared
WIN_INCLUDE_WINRT = -I$(WINDOWS_SDK_DIR)/Include/10.0.22621.0/winrt
WIN_INCLUDE_UCRT = -I$(WINDOWS_SDK_DIR)/Include/10.0.22621.0/ucrt

DIR = -I"C:/DEV"
#VERBOSE = -v 

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS) $(WIN_INCLUDE_UM) $(WIN_INCLUDE_SHARED) $(WIN_INCLUDE_WINRT) $(WIN_INCLUDE_UCRT) $(DIR) 

clean:
	rm -f $(TARGET)

.PHONY: all clean
