FROM golang:1.19-alpine

WORKDIR /app


COPY go.mod ./
RUN go mod download

COPY src/ ./src

RUN go build -o /auth-service ./src/main.go

EXPOSE 8080

CMD ["/auth-service"]