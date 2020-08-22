const float PI = 3.1415926536;

float abs(float x)
{
    return (x < 0 ? -x : x);
}

float sin(float x)
{
    if (x > PI)
        return -sin(x - ((int)(x / PI)) * PI);
    else if (x < -PI)
        return -sin(-x);
    const float B = 1.2732395447;
    const float C = -0.4052847346;
    const float P = 0.2310792853; //0.225;
    float y = B * x + C * x * abs(x);
    y = P * (y * abs(y) - y) + y;
    return y;
}

float cos(float x)
{
    const float Q = 1.5707963268;
    x += Q;

    if (x > PI)
        x -= 2 * PI;

    return (sin(x));
}

float sqrt(float a)
{
    double x, y;
    x = 0.0;
    y = a / 2;
    while (x != y)
    {
        x = y;
        y = (x + a / x) / 2;
    }
    return x;
}

float pow(float a, int b)
{
    float r = a;
    if (b > 0)
    {
        while (--b)
            r *= a;
    }
    else if (b < 0)
    {
        while (++b)
            r *= a;
        r = 1.0 / r;
    }
    else
        r = 0;
    return r;
}