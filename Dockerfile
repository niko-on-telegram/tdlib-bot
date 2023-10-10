FROM alpine as builder
RUN apk update && apk add linux-headers gperf openssl-dev zlib-dev git make cmake g++ ccache
WORKDIR /app
COPY . .
ENV CCACHE_DIR "/opt/ccache"
RUN --mount=type=cache,target=$CCACHE_DIR\
    cmake -DCMAKE_C_COMPILER_LAUNCHER="ccache" -DCMAKE_CXX_COMPILER_LAUNCHER="ccache" -DCMAKE_BUILD_TYPE=Release -B build -S . && cmake --build build

FROM alpine as runtime
COPY --from=builder /app/build/tdlib_bot /app/tdlib_bot
COPY --from=builder /lib/libssl.so.3 /lib/libssl.so.3
COPY --from=builder /lib/libz.so.1 /lib/libz.so.1
COPY --from=builder /usr/lib/libstdc++.so.6 /usr/lib/libstdc++.so.6
COPY --from=builder /usr/lib/libgcc_s.so.1 /usr/lib/libgcc_s.so.1
COPY --from=builder /lib/ld-musl-x86_64.so.1 /lib/ld-musl-x86_64.so.1
CMD ["/app/tdlib_bot"]
