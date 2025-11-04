FROM golang:1.24-alpine

WORKDIR /app


COPY go.mod ./
COPY go.sum ./
RUN go mod download

COPY src/ ./src

RUN go build -o /auth-service ./src/main.go

EXPOSE 8080

CMD ["/auth-service"]