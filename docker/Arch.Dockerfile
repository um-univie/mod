FROM archlinux
ARG j=7

WORKDIR /opt/mod
COPY ./build/mod-*.tar.gz ./
RUN tar xzf mod-*.tar.gz --strip-components=1

RUN pacman -Suy --noconfirm                                 \
    python-pip                                              \
 && pip3 install -r requirements_nodoc.txt                  \
 && cp docker/bindep_depends_arch.py                        \
    /usr/lib/python3.9/site-packages/bindep/depends.py      \
 && pacman -Suy --noconfirm                                 \
    $(bindep -b | tr '\n' ' ')                              \
 && rm -rf /var/cache/pacman


WORKDIR /opt/mod/build
RUN cmake ../ -DBUILD_DOC=no \
 -DCMAKE_BUILD_TYPE=Release                                                    \
 -DBUILD_TESTING_SANITIZERS=off                                                \
 -DCMAKE_MODULE_LINKER_FLAGS="-flto=$j" -DCMAKE_SHARED_LINKER_FLAGS="-flto=$j" \
 -DBUILD_TESTING=on                           \
 && make -j $j                                \
 && make tests -j $j                          \
 && make install                              \
 && ctest -j $j --output-on-failure -E cmake_ \
 && rm -rf /opt/mod

WORKDIR /workdir
