
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

# OBJ = org_example_Arena
# OUT = arena

clear:
	rm -f *.html
	rm -f *.log

compile:
	gcc -Wall -ansi -pedantic -fPIC ${JAVA_INC} -c ${OBJ}.c -o ${OBJ}.o -I/opt/homebrew/opt/curl/include/curl -L/opt/homebrew/opt/curl/lib
	gcc -Wall -ansi -pedantic -fPIC -shared ${OBJ}.o -o ${OUT}.dylib -L/opt/homebrew/opt/curl/lib -lcurl
