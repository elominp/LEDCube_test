#include <Tlc5940.h>
#include <tlc_config.h>
#include <tlc_fades.h>
#include <tlc_servos.h>
#include <tlc_shifts.h>
#include <string.h>

template <typename T, typename U, uint8_t width, uint8_t height>
class Panel {
  public:
    class Pos2D {
      public:
        Pos2D() = delete;
        Pos2D(T x, T y):
          _x(x),
          _y(y) {}
        Pos2D(Pos2D const &other):
          _x(other._x),
          _y(other._y) {}
        Pos2D &operator=(Pos2D const &other) {
          _x = other._x;
          _y = other._y;
          return *this; 
        }
        ~Pos2D() {}
        // void setX(T x) { _x = x; }
        const T getX() { return _x; }
        // void setY(T y) { _y = y; }
        const T getY() { return _y; }
      protected:
        T _x;
        T _y;
    };
    class RGBColor {
      public:
        RGBColor(U r, U g, U b):
          _color({r, g, b}) {}
        RGBColor(const RGBColor &other):
          _color(other._color) {}
        RGBColor &operator=(const RGBColor &other) {
          memcpy(_color, other._color, 3);
          return *this;
        }
        ~RGBColor() {}
        // void setRed(U r) { _color[0] = r; }
        U getRed() { return _color[0]; }
        // void setGreen(U g) { _color[1] = g; }
        U getGreen() { return _color[1]; }
        // void setBlue(U b) { _color[2] = b; }
        U getBlue() { return _color[2]; }
        const U* getBuffer() { return _color; }
      protected:
        const U _color[3];
    };
    Panel(uint8_t id = 0):
      _width(width),
      _height(height),
      _id(id) {
        memset(_colors, 0, width * height * sizeof(U) * 3);
    }
    Panel(Panel<U, T, width, height> const &other):
      _width(width),
      _height(height),
      _id(other._id) {
        memcpy(_colors, other._colors, width * height * sizeof(U) * 3);
    }
    Panel &operator=(Panel<U, T, width, height> const &other) {
      memcpy(_colors, other._colors, width * height * sizeof(U) * 3);
      return *this;
    }
    ~Panel() {}
    T getWidth() { return width; }
    T getHeight() { return height; }
    uint8_t getId() { return _id; }
    void setPixel(Pos2D &pos, RGBColor &color) {
      memcpy(_colors + (pos.getX() * pos.getY() * 3), color.getBuffer(), sizeof(U) * 3);
    }
    RGBColor *getPixel(Pos2D const &pos) {
      U color[3];
      memcpy(color, _colors + (pos.getX * pos.getY * 3), sizeof(U) * 3);
      return new RGBColor(color[0], color[1], color[2]);
    }
    void refresh() {
      Tlc.clear();
      U *r = _colors;
      U *g = _colors + 1;
      U *b = _colors + 2;
      uint8_t cr = 0;
      uint8_t cg = 16;
      uint8_t cb = 32;
      for (uint8_t i = 0; i < (width * height); i++) {
        Tlc.set(cr++, *r);
        Tlc.set(cg++, *g);
        Tlc.set(cb++, *b);
        r += 3;
        g += 3;
        b += 3;
      }
      Tlc.update();
    }
  protected:
    const T _width;
    const T _height;
    const uint8_t _id;
    U _colors[width * height * 3];
};

Panel<uint8_t, uint8_t,  10, 10> *panel = NULL;
void (Panel<uint8_t, uint8_t, 10, 10>::*interrupt)() = &Panel<uint8_t, uint8_t, 10, 10>::refresh;

ISR(WDU_vect) {
  if (panel != NULL)
    (panel->*interrupt)();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(F("init"));
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);
  panel = new Panel<uint8_t, uint8_t, 10, 10>();
  Panel<uint8_t, uint8_t, 10, 10>::RGBColor color(127, 127, 127);
  for (uint8_t row = 0; row < 10; row++) {
    for (uint8_t col = 0; col < 10; col++) {
      Panel<uint8_t, uint8_t, 10, 10>::Pos2D pos(col, row);
      panel->setPixel(pos, color);
    }
  }
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  exit(0);
}
