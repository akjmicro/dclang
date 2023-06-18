FROM debian:stable-slim
WORKDIR /dclang
COPY . ./
RUN apt update && \
    apt install -y git libc6-dev gcc make libasound2-dev rlwrap && \
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
    echo 'export DCLANG_LIBS="/dclang/lib"' >> /root/.bashrc && \
    echo 'alias dclang="rlwrap dclang"' >> /root/.bashrc
CMD /bin/bash
