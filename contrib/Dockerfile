FROM jeanblanchard/alpine-glibc:latest

RUN apk add \
    musl-dev gcc make git rlwrap && \
    cd / && \
    git clone https://github.com/akjmicro/dclang && \
    cd /dclang && \
    make && \
    ln -s /dclang/dclang /usr/local/bin && \
    echo 'export DCLANG_LIBS="/dclang/lib"' >> /etc/profile && \
    echo 'alias dclang="rlwrap dclang"' >> /etc/profile && \
    apk del git make gcc

CMD sh -l
