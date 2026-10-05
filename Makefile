
SHARED_LIB = resources/curltest.dylib

SRC_PATH = java/main/org/example
C_PATH = c

JAVAFILES = \
	$(SRC_PATH)/Accumulator.java \
	$(SRC_PATH)/Headers.java \
	$(SRC_PATH)/FILE.java \
	$(SRC_PATH)/Native.java \
	$(SRC_PATH)/CurlEasy.java \
	$(SRC_PATH)/Err.java \
	$(SRC_PATH)/Request.java \
	$(SRC_PATH)/Response.java \
	$(SRC_PATH)/IWriteHandler.java

MODULES = \
	$(C_PATH)/org_example_Accumulator.c \
	$(C_PATH)/org_example_Headers.c \
	$(C_PATH)/org_example_FILE.c \
	$(C_PATH)/org_example_Native.c \
	$(C_PATH)/org_example_CurlEasy.c \
	$(C_PATH)/org_example_Response.c \
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
CFLAGS = -Wall -ansi -pedantic -fPIC ${JAVA_INC} -I${CURL_HOME}/include -L${CURL_HOME}/lib -DDEBUG

LIBS = -lcurl

all: clear headers sep $(SHARED_LIB)

sep:
	$(info .........................................)
	$(info .........................................)
	$(info .........................................)

headers:
	javac -h $(C_PATH) $(JAVAFILES)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

$(SHARED_LIB): $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -shared -o $(SHARED_LIB) $(OBJECTS)

clear:
	rm -rf target
	find . -name '*.o'     	 -delete
	find . -name '*.dylib' 	 -delete
	find . -name '*.lib'   	 -delete
	find . -name '*.so'    	 -delete
	find . -name '*.dll'   	 -delete
	find . -name '*.class' 	 -delete
	find . -name '.DS_Store' -delete

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

repl:
	DEBUG=1 lein with-profile +test repl
