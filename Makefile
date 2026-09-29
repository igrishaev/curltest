
all: clear headers compile

SRC_PATH = src/main/java/org/example

JAVAFILES = \
	$(SRC_PATH)/Arena.java \
	$(SRC_PATH)/Native.java \
	$(SRC_PATH)/IWriteHandler.java

MODULES = \
	org_example_Arena.c \
	org_example_Native.c \

OUTPUTS = $(MODULES:.c=.dylib)

headers:
	javac -h . $(JAVAFILES)

JAVA_HOME ?= $(error Please specify JAVA_HOME)

JAVA_INC = \
	-I${JAVA_HOME}/include \
	-I${JAVA_HOME}/include/darwin \
	-I${JAVA_HOME}/include/win32 \
	-I${JAVA_HOME}/include/linux

CURL_HOME = /opt/homebrew/opt/curl

CC = gcc
CFLAGS = -Wall -ansi -pedantic -fPIC ${JAVA_INC} -I${CURL_HOME}/include/curl -L${CURL_HOME}/lib

org_example_Native.dylib: CFLAGS += -lcurl

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

%.dylib: %.o
	$(CC) $(CFLAGS) -shared -o $@ $*.o

compile: $(OUTPUTS)

clear:
	rm -f *.html
	rm -f *.log
	rm -f *.o
	rm -f *.dylib
