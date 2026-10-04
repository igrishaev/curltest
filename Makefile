
SHARED_LIB = curltest.dylib

SRC_PATH = src/main/java/org/example
C_PATH = c

JAVAFILES = \
	$(SRC_PATH)/Curl3.java \
	$(SRC_PATH)/http/Request.java \
	$(SRC_PATH)/IWriteHandler.java

MODULES = \
	$(C_PATH)/org_example_Curl3.c \
	$(C_PATH)/accum.c

OBJECTS = $(MODULES:.c=.o)

JAVA_HOME ?= $(error Please specify JAVA_HOME)

JAVA_INC = \
	-I${JAVA_HOME}/include \
	-I${JAVA_HOME}/include/darwin \
	-I${JAVA_HOME}/include/win32 \
	-I${JAVA_HOME}/include/linux

CURL_HOME = /opt/homebrew/opt/curl

CC = gcc
CFLAGS = -Wall -ansi -pedantic -fPIC ${JAVA_INC} -I${CURL_HOME}/include -L${CURL_HOME}/lib # -DDEBUG

LIBS = -lcurl

all: clear headers sep $(SHARED_LIB) clone

sep:
	$(info .........................................)
	$(info .........................................)
	$(info .........................................)

clone: $(SHARED_LIB)
	cp $(SHARED_LIB) server

headers:
	javac -h c $(JAVAFILES)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

$(SHARED_LIB): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -shared -o $(SHARED_LIB) $(OBJECTS)

clear:
	rm -f *.html
	rm -f *.log
	rm -f *.o
	rm -f *.dylib

acc:
	$(CC) -Wall -ansi -pedantic -c -o accum.o accum.c

module ?= $(error module=... parameter not set)

new-module: SENTRY = __$(shell echo $(module) | tr 'a-z' 'A-Z')_H__
new-module: FILE_H = $(module).h
new-module: FILE_C = $(module).c
new-module:
	touch $(FILE_C)
	echo "#include \"$(FILE_H)\""  		  >> $(FILE_C)
	touch $(FILE_H)
	echo "#ifndef $(SENTRY)"       		  >> $(FILE_H)
	echo "#define $(SENTRY)"       		  >> $(FILE_H)
	echo ""                        		  >> $(FILE_H)
	echo "/* A big thing starts here! */" >> $(FILE_H)
	echo ""                        		  >> $(FILE_H)
	echo "#endif /* $(SENTRY) */"  		  >> $(FILE_H)
