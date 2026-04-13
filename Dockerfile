FROM archlinux:base-devel

# Update and install dependencies
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
    mingw-w64-gcc \
    make \
    wget \
    unzip \
    xxd \
    python && \
    pacman -Scc --noconfirm

WORKDIR /deps

# Fetch precompiled SDL3 MinGW development libraries
RUN wget --timeout=30 https://github.com/libsdl-org/SDL/releases/download/release-3.4.4/SDL3-devel-3.4.4-mingw.zip && \
    unzip SDL3-devel-3.4.4-mingw.zip && \
    cp -r SDL3-3.4.4/x86_64-w64-mingw32/include/SDL3 /usr/x86_64-w64-mingw32/include/ && \
    cp SDL3-3.4.4/x86_64-w64-mingw32/lib/libSDL3.* /usr/x86_64-w64-mingw32/lib/ && \
    cp SDL3-3.4.4/x86_64-w64-mingw32/bin/SDL3.dll /usr/x86_64-w64-mingw32/bin/

# Fetch precompiled SDL3_ttf MinGW development libraries
RUN wget --timeout=30 https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-devel-3.2.2-mingw.zip && \
    unzip SDL3_ttf-devel-3.2.2-mingw.zip && \
    cp -r SDL3_ttf-3.2.2/x86_64-w64-mingw32/include/SDL3_ttf /usr/x86_64-w64-mingw32/include/ && \
    cp SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib/libSDL3_ttf.* /usr/x86_64-w64-mingw32/lib/ && \
    cp SDL3_ttf-3.2.2/x86_64-w64-mingw32/bin/*.dll /usr/x86_64-w64-mingw32/bin/

WORKDIR /app

COPY hdr/ ./hdr/
COPY src/ ./src/
COPY Makefile ./
COPY assets/ ./assets/

CMD ["sh", "-c", "make clean && make embeds && make release CXX=x86_64-w64-mingw32-g++ BINARY=cppixelsorter.exe && cp /usr/x86_64-w64-mingw32/bin/*.dll ."]
