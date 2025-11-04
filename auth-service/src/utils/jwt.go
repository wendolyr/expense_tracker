package utils

import (
	"auth-service/src/models"
	"time"

	"github.com/golang-jwt/jwt/v5"
)

var jwtSecrets = []byte("your-secret-key-change-in-production") //TODO: вынести в env

func GenerateTokens(userID, email string) (*models.TokenResponse, error) {
	accessExpires := time.Now().Add(15 * time.Minute)
	accessClaims := &models.Claims{
		UserID: userID,
		Email:  email,
		RegisteredClaims: jwt.RegisteredClaims{
			ExpiresAt: jwt.NewNumericDate(accessExpires),
			IssuedAt:  jwt.NewNumericDate(time.Now()),
			Subject:   userID,
		},
	}

	accessToken := jwt.NewWithClaims(jwt.SigningMethodHS256, accessClaims)
	accessTokenString, err := accessToken.SignedString(jwtSecrets)
	if err != nil {
		return nil, err
	}

	refreshExpires := time.Now().Add(7 * 24 * time.Hour)
	refreshClaims := &models.Claims{
		UserID: userID,
		Email:  email,
		RegisteredClaims: jwt.RegisteredClaims{
			ExpiresAt: jwt.NewNumericDate(refreshExpires),
			IssuedAt:  jwt.NewNumericDate(time.Now()),
			Subject:   userID,
		},
	}

	refreshToken := jwt.NewWithClaims(jwt.SigningMethodHS256, refreshClaims)
	refreshTokenString, err := refreshToken.SignedString(jwtSecrets)
	if err != nil {
		return nil, err
	}

	return &models.TokenResponse{
		UserID:       userID,
		Email:        email,
		AccessToken:  accessTokenString,
		RefreshToken: refreshTokenString,
		ExpiresIn:    int64(15 * time.Minute / time.Second),
	}, nil
}

func ValidateToken(tokenString string) (*models.Claims, error) {
	claims := &models.Claims{}

	token, err := jwt.ParseWithClaims(tokenString, claims, func(token *jwt.Token) (interface{}, error) {
		return jwtSecrets, nil
	})

	if err != nil {
		return nil, err
	}

	if !token.Valid {
		return nil, jwt.ErrSignatureInvalid
	}

	return claims, nil
}
