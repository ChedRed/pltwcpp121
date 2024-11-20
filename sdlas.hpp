#pragma once
#include <cmath>

class Vector2{ public: int x; int y; float magnitude(){ return sqrt(pow(x, 2) + pow(y, 2)); }; };
class Vector2f{ public: float x; float y; float magnitude(){ return sqrt(pow(x, 2) + pow(y, 2)); }; };

Vector2 operator+(Vector2 a, Vector2 b){
    return Vector2{a.x + b.x, a.y + b.y};
}

Vector2 operator-(Vector2 a, Vector2 b){
    return Vector2{a.x - b.x, a.y - b.y};
}

Vector2 operator*(Vector2 a, Vector2 b){
    return Vector2{a.x * b.x, a.y * b.y};
}

Vector2 operator/(Vector2 a, Vector2 b){
    return Vector2{a.x / b.x, a.y / b.y};
}


Vector2f operator+(Vector2f a, Vector2f b){
    return Vector2f{a.x + b.x, a.y + b.y};
}

Vector2f operator+=(Vector2f a, Vector2f b){
    return Vector2f{a.x + b.x, a.y + b.y};
}


Vector2f operator-(Vector2f a, Vector2f b){
    return Vector2f{a.x - b.x, a.y - b.y};
}


Vector2f operator*(Vector2f a, Vector2f b){
    return Vector2f{a.x * b.x, a.y * b.y};
}

Vector2f operator*(Vector2f a, float b){
    return Vector2f{a.x * b, a.y * b};
}


Vector2f operator/(Vector2f a, Vector2f b){
    return Vector2f{a.x / b.x, a.y / b.y};
}
