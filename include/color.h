#pragma once

#include <iostream>
#include <cstdint> // Include for Uint8 definition

using Uint8 = uint8_t; // Type alias for unsigned 8-bit integer

struct Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    Color() : r(255), g(255), b(255), a(255) {}

    Color(int red, int green, int blue, int alpha = 255) {
        r =   static_cast<Uint8>((red > 255) ? 255 : ((red < 0) ? 0 : red));
        g = static_cast<Uint8>((green > 255) ? 255 : ((green < 0) ? 0 : green));
        b =  static_cast<Uint8>((blue > 255) ? 255 : ((blue < 0) ? 0 : blue));
        a = static_cast<Uint8>((alpha > 255) ? 255 : ((alpha < 0) ? 0 : alpha));
    }

    Color(float red, float green, float blue, float alpha = 1.0f)
        : r(static_cast<Uint8>(red * 255.0f)),
          g(static_cast<Uint8>(green * 255.0f)),
          b(static_cast<Uint8>(blue * 255.0f)),
          a(static_cast<Uint8>(alpha * 255.0f)) {
    
        r = (r > 255) ? 255 : ((r < 0) ? 0 : r);
        g = (g > 255) ? 255 : ((g < 0) ? 0 : g);
        b = (b > 255) ? 255 : ((b < 0) ? 0 : b);
        a = (a > 255) ? 255 : ((a < 0) ? 0 : a);
    }

    Color operator+(const Color& other) const {
        int _r = static_cast<int>(r) + static_cast<int>(other.r);
        int _g = static_cast<int>(g) + static_cast<int>(other.g);
        int _b = static_cast<int>(b) + static_cast<int>(other.b);
        int _a = static_cast<int>(a) + static_cast<int>(other.a);

        _r = (_r > 255) ? 255 : ((_r < 0) ? 0 : _r);
        _g = (_g > 255) ? 255 : ((_g < 0) ? 0 : _g);
        _b = (_b > 255) ? 255 : ((_b < 0) ? 0 : _b);
        _a = (_a > 255) ? 255 : ((_a < 0) ? 0 : _a);

        return Color(_r, _g, _b, _a);
    }

    Color operator*(float scalar) const {
        Uint8 _r = static_cast<Uint8>(r * scalar);
        Uint8 _g = static_cast<Uint8>(g * scalar);
        Uint8 _b = static_cast<Uint8>(b * scalar);
        Uint8 _a = static_cast<Uint8>(a * scalar);
        return Color(_r, _g, _b, _a);
    }

    Color operator*(Color other) const {
        Uint8 _r = static_cast<Uint8>((r * other.r) / 255);
        Uint8 _g = static_cast<Uint8>((g * other.g) / 255);
        Uint8 _b = static_cast<Uint8>((b * other.b) / 255);
        Uint8 _a = static_cast<Uint8>((a * other.a) / 255);

        return Color(_r, _g, _b, _a);
    }

    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool operator!=(const Color& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "Color(" 
        << static_cast<int>(color.r) << ", "
        << static_cast<int>(color.g) << ", "
        << static_cast<int>(color.b) << ", "
        << static_cast<int>(color.a) << ")";
        return os;
    }
};
