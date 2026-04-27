FROM alpine:latest AS builder
WORKDIR /dclang
COPY . ./
RUN apk add --no-cache build-base sqlite-dev portmidi-dev git rlwrap && \
    cd / && \
    cd /dclang && \
    cp -a Makefile.linux Makefile && \
    make clean && \
    make dclang && \
    make lib && \
    make install && \
    echo 'export DCLANG_LIBS="/dclang/lib"' >> /root/.profile && \
    echo 'alias dclang="rlwrap dclang"' >> /root/.profile
CMD /bin/bash

FROM busybox:1.36-glibc
WORKDIR /
# system needs
COPY --from=builder /lib/ld-musl-*.so.1 /lib/
COPY --from=builder /usr/lib/libreadline* /usr/lib/
COPY --from=builder /usr/lib/libptytty* /usr/lib
COPY --from=builder /usr/lib/libncursesw* /usr/lib/
COPY --from=builder /usr/lib/libgcc_s* /usr/lib/
COPY --from=builder /usr/lib/libasound* /usr/lib/
# immediate lib deps
COPY --from=builder /usr/lib/libsqlite3* /usr/lib/
COPY --from=builder /usr/lib/libportmidi* /usr/lib/
COPY --from=builder /usr/local/lib/libdclang* /lib/
# binaries
COPY --from=builder /usr/bin/rlwrap /usr/bin/
COPY --from=builder /usr/local/bin/dclang /usr/bin/
# environment
COPY --from=builder /root/.profile /root/.profile
COPY --from=builder /dclang* /dclang/
# command entrypoint
CMD sh -l
