FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ cmake make

WORKDIR /app

COPY . .

RUN mkdir build && cd build && cmake .. && make
RUN ls -l ./build/logging_server

CMD ["./build/logging_server"]