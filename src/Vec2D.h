#pragma once
#include  "SDL.h"
#include <iostream>
#include <math.h>

struct Vec2D {
	float x, y;
	Vec2D(float x, float y) : x(x), y(y) {}
	Vec2D(int x, int y) : x((float)x), y((float)y) {}
	Vec2D() : x(0), y(0) {}
	float operator[] (int index) {
		index = index % 2;
		switch (index) {
			case 0: return x;
			case 1: return y;
			default: return 0.0f;
		}
	}
	friend Vec2D abs(Vec2D v) {
		return Vec2D(fabs(v.x), fabs(v.y));
	}
	operator bool() const {
		return x != 0.0f && y != 0.0f;
	}
	Vec2D infinite() {
		return Vec2D(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
	}
	Vec2D operator+ (Vec2D v) {
		return Vec2D(x + v.x, y + v.y);
	}
	Vec2D operator+ (float f) {
		return Vec2D(x + f, y + f);
	}
	Vec2D& operator+= (Vec2D v) {
		*this = *this + v;
		return *this;
	}
	Vec2D& operator+= (float f) {
		*this = *this + f;
		return *this;
	}
	Vec2D operator- (Vec2D v) {
		return Vec2D(x - v.x, y - v.y);
	}
	Vec2D operator- (float f) {
		return Vec2D(x - f, y - f);
	}
	Vec2D& operator-= (Vec2D v) {
		*this = *this - v;
		return *this;
	}
	Vec2D& operator-= (float f) {
		*this = *this - f;
		return *this;
	}
	Vec2D operator* (Vec2D v) {
		return Vec2D(x * v.x, y * v.y);
	}
	Vec2D operator* (float f) {
		return Vec2D(x * f, y * f);
	}
	Vec2D& operator*= (Vec2D v) {
		*this = *this * v;
		return *this;
	}
	Vec2D& operator*= (float f) {
		*this = *this * f;
		return *this;
	}
	Vec2D operator/ (Vec2D v) {
		return Vec2D(x / v.x, y / v.y);
	}
	Vec2D operator/ (float f) {
		return Vec2D(x / f, y / f);
	}
	Vec2D& operator/= (Vec2D v) {
		*this = *this / v;
		return *this;
	}
	Vec2D& operator/= (float f) {
		*this = *this / f;
		return *this;
	}
	bool operator== (Vec2D v) {
		return x == v.x && y == v.y;
	}
	bool operator!= (Vec2D v) {
		return x != v.x && y != v.y;
	}
	Vec2D abs() {
		return Vec2D(fabs(x), fabs(y));
	}
	float dotProduct(Vec2D v) {
		return x * v.x + y * v.y;
	}
	float crossProductArea(Vec2D v) {
		return x * v.y - y * v.x;
	}
	Vec2D normalized() {
		return unitVector();
	}
	Vec2D unitVector() {
		if (magnitude() != 0) {
			return Vec2D(x / magnitude(), y / magnitude());
		}
		return Vec2D(0, 0);
	}
	Vec2D identityVector() { // p.s. I started by using a ternary operator here but I think it's unclear code so I wrote it out with if statements
		Vec2D identity = Vec2D();
		if (x > 0) {
			identity.x = 1;
		} else if (x < 0) {
			identity.x = -1;
		} else {
			identity.x = 0;
		}
		if (y > 0) {
			identity.y = 1;
		} else if (y < 0) {
			identity.y = -1;
		} else {
			identity.y = 0;
		}
		return identity;
	}
	Vec2D tangent() {
		return Vec2D(-y, x);
	}
	Vec2D opposite() {
		return Vec2D(-x, -y);
	}
	float magnitude() {
		return sqrtf(x * x + y * y);
	}
	bool isZero() {
		return x == 0 && y == 0;
	}
	bool operator> (Vec2D v) {
		return magnitude() > v.magnitude();
	}
	bool operator>= (Vec2D v) {
		return magnitude() >= v.magnitude();
	}
	bool operator< (Vec2D v) {
		return magnitude() < v.magnitude();
	}
	bool operator<= (Vec2D v) {
		return magnitude() <= v.magnitude();
	}
	SDL_Rect Vec2DtoSDLRect(Vec2D v2) {
		return { (int)round(x), (int)round(y), (int)round(v2.x), (int)round(v2.y) };
	}
	friend std::ostream& operator<<(std::ostream& os, const Vec2D& v) {
		os << '(' << v.x << ',' << v.y << ')';
		return os;
	}
};