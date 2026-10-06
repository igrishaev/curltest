package org.example;

import java.io.*;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

public class Native {

    public static final long NULL;
    public static final String libPath;

    private static String writeLibToTemp() throws IOException {
        ClassLoader cl = Native.class.getClassLoader();
        String resourcePath = OS.getLibName();
        final URL url = cl.getResource(resourcePath);
        if (url == null) {
            Err.error("failed to load a resource: %s", resourcePath);
        }
        final File tmp = File.createTempFile("temp_", ".lib");
        final Path path = tmp.toPath();
        System.out.println(path);
        tmp.deleteOnExit();
        try (InputStream in = url.openStream();
             OutputStream out = Files.newOutputStream(tmp.toPath())) {
            Files.copy(in, path, StandardCopyOption.REPLACE_EXISTING);
            in.transferTo(out);
        }
        return path.toString();
    }

    static {
        try {
            libPath = writeLibToTemp();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    static void loadLib() {
        System.load(libPath);
    }

    static {
        loadLib();
        NULL = get_null();
    }
    native private static long get_null();
}
