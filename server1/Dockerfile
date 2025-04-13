FROM ubuntu:22.04

# Install dependencies for building and running
RUN apt-get update && apt-get install -y \
    g++ cmake make libx11-dev libxinerama-dev \
    libxrandr-dev libxi-dev libxtst-dev \
    libpng-dev libjpeg-dev \
    libxext-dev libsm-dev libice-dev

# Create working dir
WORKDIR /app

# Copy server source files
COPY . .

# Build the project
RUN mkdir build && cd build && cmake .. && make

CMD ["./build/server1"] 