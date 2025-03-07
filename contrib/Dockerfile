FROM alpine:latest
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
COPY --from=0 /lib/ld-musl-x86_64.so.1 /lib/
COPY --from=0 /usr/lib/libreadline* /usr/lib/
COPY --from=0 /usr/lib/libncursesw* /usr/lib/
COPY --from=0 /usr/lib/libasound* /usr/lib/
COPY --from=0 /usr/lib/libsqlite3* /usr/lib/
# immediate lib deps
COPY --from=0 /usr/lib/libportmidi* /usr/lib/
COPY --from=0 /usr/local/lib/libdclang* /lib/
# binaries
COPY --from=0 /usr/bin/rlwrap /usr/bin/
COPY --from=0 /usr/local/bin/dclang /usr/bin/
# environment
COPY --from=0 /root/.profile /root/.profile
COPY --from=0 /dclang* /dclang/
CMD sh -l
