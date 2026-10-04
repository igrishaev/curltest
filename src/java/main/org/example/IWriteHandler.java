package org.example;

@FunctionalInterface
public interface IWriteHandler {
    void handle(byte[] buf, int off, int len);
}
