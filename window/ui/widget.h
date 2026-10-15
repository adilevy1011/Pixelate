#pragma once
class DrawContext;

enum class CursorType {
    Default,
    Hand
};

class Widget {
public:
    virtual ~Widget() = default;

    virtual void draw(DrawContext& context) = 0;

    void setPosition(int x, int y);
    void setSize(int width, int height);
    int x() const { return x_; }
    int y() const { return y_; }
    int width() const { return width_; }
    int height() const { return height_; }
    
    bool containsPoint(int x, int y) const;

    virtual bool onMouseDown(int x, int y);
    virtual CursorType cursor() const {
        return CursorType::Default;
    }
protected:
    int x_ = 0;
    int y_ = 0;
    int width_ = 100;
    int height_ = 30;
};
