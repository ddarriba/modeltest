FROM debian:buster-slim AS builder
WORKDIR /modeltest
COPY . /modeltest
RUN apt update \
    && apt install -y \
    autoconf \
    automake \
    libtool \
    bison \
    flex \
    cmake \
    build-essential \
    git \
    && mkdir build && cd build \
    && cmake .. && make

FROM debian:buster-slim
WORKDIR /root
COPY --from=builder /modeltest/bin/modeltest-ng /bin
CMD ["modeltest-ng", "--help"]
