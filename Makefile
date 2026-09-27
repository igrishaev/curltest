
all: clear headers compile

headers:
	javac -h . \
	src/main/java/org/example/Arena.java \
	src/main/java/org/example/Native.java \
	src/main/java/org/example/IWriteHandler.java

JAVA_HOME := $(shell echo $${JAVA_HOME%/})
JAVA_INC = -I${JAVA_HOME}/include -I${JAVA_HOME}/include/darwin -I${JAVA_HOME}/include/win32 -I${JAVA_HOME}/include/linux

OBJ = org_example_Native
OUT = curltest

# doesn't need curl
# OBJ = org_example_Arena
# OUT = arena

clear:
	rm -f *.html
	rm -f *.log

compile:
	gcc -Wall -ansi -pedantic -fPIC ${JAVA_INC} -c ${OBJ}.c -o ${OBJ}.o
	gcc -Wall -ansi -pedantic -fPIC -shared ${OBJ}.o -lcurl -o ${OUT}.dylib
