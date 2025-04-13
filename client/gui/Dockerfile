FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV QT_QPA_PLATFORM=xcb

# Install dependencies for building Qt-based GUI
RUN apt-get update && apt-get install -y \
    qtbase5-dev qttools5-dev \
    x11-apps \
    libxcb-xinerama0 \
    libxcb1 \
    libx11-6 \
    libxext6 \
    libxrender1 \
    libxkbcommon-x11-0 \
    libglu1-mesa \
    libgl1-mesa-glx \
    libqt5gui5 \
    libqt5widgets5 \
    libqt5core5a \
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