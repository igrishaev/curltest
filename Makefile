
headers:
	javac -h . src/main/java/org/example/Main.java

JAVA_HOME := $(shell echo $${JAVA_HOME%/})
JAVA_INC = -I${JAVA_HOME}/include -I${JAVA_HOME}/include/darwin -I${JAVA_HOME}/include/win32 -I${JAVA_HOME}/include/linux

OBJ = org_example_Main

compile:
	gcc -Wall -ansi -pedantic -fPIC ${JAVA_INC} -c ${OBJ}.c -o ${OBJ}.o
	gcc -Wall -ansi -pedantic -fPIC -shared ${OBJ}.o -lcurl -o curltest.dylib
