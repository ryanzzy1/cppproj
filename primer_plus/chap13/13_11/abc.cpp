class Ellipse
{
private:
    double x;
    double y;
    double a;
    double b;
    double angle;
public:
    void Move(int nx, int ny) {x = nx; y = ny;}
    virtual double Area() const {return 3.14159 * a * b;}
    virtual void Rotate(double nang) {angle += nang;}
    virtual void Scale(double sa, double sb) {a *= sa; b *= sb;}
};

/**
 * 
 */
// //class Circle : public Ellipse
// {
// };

class Circle
{
private:
    double x;
    double y;
    double r;
public:
    void Move(int nx, int ny) {x = nx; y = ny;}
    double Area() const {return 3.14159 * r * r;}
    void Scale(double sr) {r *= sr;}
};