FROM ubuntu:20.04

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        clang git cmake make ninja-build apt-transport-https ca-certificates \
    && rm -rf /var/lib/apt/lists/* \
    && update-ca-certificates
