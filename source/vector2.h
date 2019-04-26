#ifndef VECTOR2_H
#define VECTOR2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//  Example of use Vec2<double> vector1(1, 2, 3);

namespace fpmed
{
template <class T> class Vec2
{
    private:
        // A Vec2 simply has three properties called x, y and z
        T x, y;

    public:
        // ------------ Constructors ------------

        // Default constructor
        Vec2() { x = y = 0; };

        // Three parameter constructor
        Vec2(T xValue, T yValue)
        {
            x = xValue;
            y = yValue;
        }

        // ------------ Getters and setters ------------

        void set(const T &xValue, const T &yValue)
        {
            x = xValue;
            y = yValue;
        }

        T getX() const { return x; }
        T getY() const { return y; }

        void setX(const T &xValue) { x = xValue; }
        void setY(const T &yValue) { y = yValue; }

        // ------------ Helper methods ------------

        // Method to reset a vector to zero
        void zero()
        {
            x = y = 0;
        }

        // Method to normalise a vector
        void normalise()
        {
            // Calculate the magnitude of our vector
            T magnitude = sqrt((x * x) + (y * y));

            // As long as the magnitude isn't zero, divide each element by the magnitude
            // to get the normalised value between -1 and +1
            if (magnitude != 0)
            {
                x /= magnitude;
                y /= magnitude;
            }
        }

        // Static method to calculate and return the scalar dot product of two vectors
        //
        // Note: The dot product of two vectors tell us things about the angle between
        // the vectors. That is, it tells us if they are pointing in the same direction
        // (i.e. are they parallel? If so, the dot product will be 1), or if they're
        // perpendicular (i.e. at 90 degrees to each other) the dot product will be 0,
        // or if they're pointing in opposite directions then the dot product will be -1.
        //
        // Usage example: double foo = Vec2<double>::dotProduct(vectorA, vectorB);
        static T dotProduct(const Vec2 &vec1, const Vec2 &vec2)
        {
            return vec1.x * vec2.x + vec1.y * vec2.y;
        }


        static T getDistance(const Vec2 &v1, const Vec2 &v2)
        {
            T dx = v2.x - v1.x;
            T dy = v2.y - v1.y;

            return sqrt(dx * dx + dy * dy);
        }

		// Method to convert a char array string to a vector2, example "0.000000, 0.000000", WARNING, this function have no security CHECKS yet, use with caution
        void atovec2(char *array_Vector)
        {
			char *delimit = ",";
			x = atof(strtok(array_Vector, delimit));
			y = atof(strtok(NULL, delimit));

        }

        // ------------ Overloaded operators ------------

        // Overloaded addition operator to add Vec2s together
        Vec2 operator+(const Vec2 &vector) const
        {
            return Vec2<T>(x + vector.x, y + vector.y);
        }

        // Overloaded add and asssign operator to add Vec2s together
        void operator+=(const Vec2 &vector)
        {
            x += vector.x;
            y += vector.y;
        }

        // Overloaded subtraction operator to subtract a Vec2 from another Vec2
        Vec2 operator-(const Vec2 &vector) const
        {
            return Vec2<T>(x - vector.x, y - vector.y);
        }

        // Overloaded subtract and asssign operator to subtract a Vec2 from another Vec2
        void operator-=(const Vec2 &vector)
        {
            x -= vector.x;
            y -= vector.y;
        }

        // Overloaded multiplication operator to multiply two Vec2s together
        Vec2 operator*(const Vec2 &vector) const
        {
            return Vec2<T>(x * vector.x, y * vector.y);
        }

        // Overloaded multiply operator to multiply a vector by a scalar
        Vec2 operator*(const T &value) const
        {
            return Vec2<T>(x * value, y * value);
        }

        // Overloaded multiply and assign operator to multiply a vector by a scalar
        void operator*=(const T &value)
        {
            x *= value;
            y *= value;
        }

        // Overloaded multiply operator to multiply a vector by a scalar
        Vec2 operator/(const T &value) const
        {
            return Vec2<T>(x / value, y / value);
        }

        // Overloaded multiply and assign operator to multiply a vector by a scalar
        void operator/=(const T &value)
        {
            x /= value;
            y /= value;
        }
};
}// fpmed
#endif
