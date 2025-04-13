FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies for building Qt-based GUI
RUN apt-get update && apt-get install -y \
    qtbase5-dev qttools5-dev \
    x11-apps \
    build-essential cmake

# Create working directory
WORKDIR /app

# Copy client GUI source files
COPY . .

# Build the project
WORKDIR /app/build  
RUN rm -rf * && cmake .. && make

# Set the default command to run the GUI client
CMD ["./gui"]