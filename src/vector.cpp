#include "vector.h"


Vector2::operator Vector3() const
{
    return Vector3(x, y, 0.0f);
}

Vector2 Vector2::operator+(const Vector3& vecOther) const
{
    return Vector2(x + vecOther.x, y + vecOther.y);
}

Vector2 Vector2::operator-(const Vector3& vecOther) const
{
    return Vector2(x - vecOther.x, y - vecOther.y);
}

Vector2& Vector2::operator+=(const Vector3& vecOther)
{
    x += vecOther.x;
    y += vecOther.y;
    return *this;
}
