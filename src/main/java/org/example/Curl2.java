package org.example;

public record Curl2() {

    public static int request(final Request request, final Arena arena) {

        arena.orderJNI();

        // allocate counter
        long counter = 0;
        arena.putLong(counter);

        int dataOffset = 1024;

        // url
        arena.putLong(H.CURLOPT_URL);
        final String url = request.url();
        System.out.println(url);
        if (url == null) {
            arena.putNULL();
        } else {
            arena.putLong(arena.ptr() + dataOffset);
            final int len = arena.putCString(dataOffset, url);
            dataOffset += len;
        }
        counter++;

        // follow location
        arena.putLong(H.CURLOPT_FOLLOWLOCATION);
        arena.putLong(request.followLocation().code);
        counter++;

        // http method
//        switch (request.httpMethod()) {
//            case GET -> {
////                arena.putLong(CURLOPT.HTTPGET);
////                arena.putLong(1);
////                counter++;
//            }
//            case POST -> {
////                arena.putLong(CURLOPT.POST);
////                arena.putLong(1);
////                counter++;
//            }
//            case PUT -> {
////                arena.putLong(CURLOPT.UPLOAD);
////                arena.putLong(1);
////                counter++;
//            }
//            default -> {
//
//            }
//        }

        // set the final
        arena.putLong(0, counter);

        // arena.debug(64);
        // debug

        arena.rewind();

        arena.debug(64);
        arena.rewind();


        System.out.println(arena.getLong());
        System.out.println(arena.getLong());
        System.out.println(arena.getLong());
        System.out.println(arena.getLong());
        System.out.println(arena.getLong());
        System.out.println("-----------------");

        // arena.orderJVM();
        // final long c = arena.getLong();
        // System.out.println(c);
//        for (int i = 0; i < c; i++) {
//            System.out.println(arena.getLong());
//            System.out.println(arena.getLong());
//        }

        // return 0;
        return Native.perform(arena.ptr());
    }

    public static void main(final String... args) {
        final Arena arena = Arena.of(32000);
        final Request request = Request.builder()
                .url("https://google.com")
                .followLocation(FollowLocation.ALL)
                .build();
        final int code = request(request, arena);
        System.out.println(code);
    }

}
