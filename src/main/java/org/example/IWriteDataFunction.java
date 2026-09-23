package org.example;

@FunctionalInterface
public interface IWriteDataFunction {
    void handle(byte[] buf, int len, int off);
}
