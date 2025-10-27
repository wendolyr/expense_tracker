package main

import (
	"auth-service/src/database"
	"auth-service/src/handlers"
	"github.com/gin-gonic/gin"
	"log"
)

func main() {
	if err := database.InitDB(); err != nil {
		log.Fatal("Failed to initialize database:", err)
	}

	r := gin.Default()

	authHandler := handlers.NewAuthHandler(database.DB)

	r.POST("/register", authHandler.Register)
	r.POST("/login", authHandler.Login)

	r.GET("/health", func(c *gin.Context) {
		c.JSON(200, gin.H{"status": "OK", "service": "auth-service"})
	})

	log.Println("Auth service starting on :8080")
	if err := r.Run(":8080"); err != nil {
		log.Fatal("Failed to start server:", err)
	}
}
