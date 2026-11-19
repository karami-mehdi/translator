FROM debian:bookworm-slim

WORKDIR /app

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libcurl4-openssl-dev \
    nlohmann-json3-dev \
    libcpp-httplib-dev \
    && rm -rf /var/lib/apt/lists/*

COPY CMakeLists.txt .
COPY main.cpp .

RUN cmake -S . -B build \
    && cmake --build build

CMD ["./build/Translator"]