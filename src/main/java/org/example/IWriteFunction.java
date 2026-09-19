package org.example;

@FunctionalInterface
public interface IWriteFunction {
    void handle(byte[] buf, int len, int off);
}
