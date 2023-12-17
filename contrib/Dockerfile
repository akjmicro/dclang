FROM debian:bullseye-slim
WORKDIR /dclang
COPY . ./
RUN apt update && \
    apt install -y git libc6-dev gcc make libasound2-dev libsqlite3-dev rlwrap && \
    cd / && \
    git clone http://github.com/akjmicro/portmidi-linux.git && \
    cd portmidi-linux && \
    make clean && make && make install && \
    cd /dclang && \
    cp -a Makefile.linux Makefile && \
    make clean && \
    make dclang && \
    make lib && \
    make install && \
    apt remove -y make gcc libc6-dev git && \
    apt autoremove -y && \
    echo 'export DCLANG_LIBS="/dclang/lib"' >> /root/.profile && \
    echo 'alias dclang="rlwrap dclang"' >> /root/.profile
CMD /bin/bash

FROM busybox:glibc
WORKDIR /
# sytem needs
COPY --from=0 /lib/x86_64-linux-gnu/libreadline* /lib/
COPY --from=0 /lib/x86_64-linux-gnu/libtinfo* /lib/
COPY --from=0 /lib/x86_64-linux-gnu/libutil.so.1 /lib/
COPY --from=0 /lib/x86_64-linux-gnu/libdl* /lib/
COPY --from=0 /usr/lib/x86_64-linux-gnu/libasound* /usr/lib/
COPY --from=0 /usr/lib/x86_64-linux-gnu/libsqlite3* /usr/lib/
# terminfo DB for rlwrap
COPY --from=0 /lib/terminfo/ /lib/terminfo/
# immediate lib deps
COPY --from=0 /usr/local/lib/libportmidi* /lib/
COPY --from=0 /usr/local/lib/libdclang* /lib/
# binaries
COPY --from=0 /usr/bin/rlwrap /usr/bin/
COPY --from=0 /usr/local/bin/dclang /usr/bin/
# environment
COPY --from=0 /root/.profile /root/.profile
COPY --from=0 /dclang* /dclang/
CMD sh -l
