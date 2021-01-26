FROM ubuntu:20.04

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN groupadd --gid 1000 smplc \
    && useradd --uid 1000 --gid smplc --shell /bin/bash --create-home smplc --home-dir /home/smplc/ \
    && mkdir -p /etc/sudoers.d \
    && echo 'smplc ALL=(ALL:ALL) NOPASSWD:ALL' >> /etc/sudoers.d/smplc \
    && chmod 440 /etc/sudoers.d/smplc

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        sudo clang git cmake make ninja-build \
    && rm -rf /var/lib/apt/lists/*

USER smplc

WORKDIR /home/smplc
