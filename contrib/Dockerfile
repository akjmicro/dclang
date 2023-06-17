FROM jeanblanchard/alpine-glibc:latest

WORKDIR /dclang

COPY . ./

RUN apk add \
    musl-dev gcc make git rlwrap portmidi-dev && \
    cd / && \
    cd /dclang && \
    cp -a Makefile.linux Makefile && \
    make clean && \
    make dclang && \
    make lib && \
    ln -s /dclang/dclang.h /usr/local/include && \
    ln -s /dclang/libdclang.so /usr/local/lib && \
    ln -s /dclang/dclang /usr/local/bin && \
    echo 'export DCLANG_LIBS="/dclang/lib"' >> /etc/profile && \
    echo 'alias dclang="rlwrap dclang"' >> /etc/profile && \
    apk del git make gcc

CMD sh -l
