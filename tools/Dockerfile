FROM ubuntu:20.04

RUN apt-get update && \
    apt-get install --yes \
        python3.6 \
        git \
        clang-format-8 && \
    rm -rf /var/lib/apt/lists/*

ENV SC_CLANG_FORMAT=/usr/bin/clang-format-8
ENV SC_CLANG_FORMAT_DIFF=/sc/tools/clang-format.py

WORKDIR /sc/

ENTRYPOINT [ "python3", "/sc/tools/clang-format.py" ]
