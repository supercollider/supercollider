FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install --yes \
        python3.6 \
        git \
        clang-format-8 && \
    rm -rf /var/lib/apt/lists/*

# add python3.6->python3 alias
# clang needs python2 which already takes the python name
RUN ln -s /usr/bin/python3.6 /usr/bin/python3

ENV SC_CLANG_FORMAT=/usr/bin/clang-format-8
ENV SC_CLANG_FORMAT_DIFF=/sc/tools/clang-format.py

WORKDIR /sc/

ENTRYPOINT [ "python3", "/sc/tools/clang-format.py" ]
