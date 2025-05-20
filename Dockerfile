FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y \
    make \
    gdb \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN make

EXPOSE 8888

CMD ["./bin/server"]
