#include <iostream>
double f(double x, double y)
{
    std::cout << "val x: " << x << "\n";
    std::cout << "val y: " << y << "\n";
    x=x+1;
    y=y+1;
    return x * y;
}
double f2(double *x, double *y)
{
    std::cout << "val x: " << *x << "\n";
    std::cout << "val y: " << *y << "\n";
    *x=*x+1;
    *y=*y+1;
    return *x * *y;
}
int main()
{
    double a, b;
    a = 2;
    b = 3;
    std::cout << f(a, b) << "\n";
    std::cout << f2(&a, &b) << "\n";
    std::cout << f(a, b) << "\n";
    return 0;
}
