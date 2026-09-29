
SHARED_LIB = curltest.dylib

SRC_PATH = src/main/java/org/example

JAVAFILES = \
	$(SRC_PATH)/Arena.java \
	$(SRC_PATH)/Native.java \
	$(SRC_PATH)/IWriteHandler.java

MODULES = \
	org_example_Arena.c \
	org_example_Native.c \

OBJECTS = $(MODULES:.c=.o)

JAVA_HOME ?= $(error Please specify JAVA_HOME)

JAVA_INC = \
	-I${JAVA_HOME}/include \
	-I${JAVA_HOME}/include/darwin \
	-I${JAVA_HOME}/include/win32 \
	-I${JAVA_HOME}/include/linux

CURL_HOME = /opt/homebrew/opt/curl

CC = gcc
CFLAGS = -Wall -ansi -pedantic -fPIC ${JAVA_INC} -I${CURL_HOME}/include/curl -L${CURL_HOME}/lib

LIBS = -lcurl

all: clear headers $(SHARED_LIB)

headers:
	javac -h . $(JAVAFILES)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

$(SHARED_LIB): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -shared -o $(SHARED_LIB) $(OBJECTS)

clear:
	rm -f *.html
	rm -f *.log
	rm -f *.o
	rm -f *.dylib
