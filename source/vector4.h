#ifndef VECTOR4_H
#define VECTOR4_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//  Example of use Vec4<double> vector1(1, 2, 3);

namespace fpmed
{
template <class T> class Vec4
{
    private:
        // A Vec4 simply has four properties called p, q r and s
        T p, q, r, s;

    public:
        // ------------ Constructors ------------

        // Default constructor
        Vec4() { p = q = r = s = 0; };

        // Three parameter constructor
        Vec4(T pValue, T qValue, T rValue, T sValue)
        {
			p = pValue;
            q = qValue;
            r = rValue;
			s = sValue;
        }

        // ------------ Getters and setters ------------

        void set(const T &pValue, const T &qValue, const T &rValue, const T &sValue)
        {
            p = pValue;
            q = qValue;
            r = rValue;
			s = sValue;
        }

        T getP() const { return p; }
		T getQ() const { return q; }
        T getR() const { return r; }
		T getS() const { return s; }

        void setP(const T &pValue) { p = pValue; }
		void setQ(const T &qValue) { q = qValue; }
        void setR(const T &rValue) { r = rValue; }
		void setS(const T &sValue) { s = sValue; }

        // ------------ Helper methods ------------

        // Method to reset a vector to zero
        void zero()
        {
            p = q = r = s = 0;
        }

        // Method to normalise a vector
        void normalise()
        {
            // Calculate the magnitude of our vector
            T magnitude = sqrt((p * p) + (q * q) + (r * r) + (s * s));

            // As long as the magnitude isn't zero, divide each element by the magnitude
            // to get the normalised value between -1 and +1
            if (magnitude != 0)
            {
                p /= magnitude;
				q /= magnitude;
				r /= magnitude;
				s /= magnitude;
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
        // Usage example: double foo = Vec4<double>::dotProduct(vectorA, vectorB);
        static T dotProduct(const Vec4 &vec1, const Vec4 &vec2)
        {
            return vec1.p * vec2.p + vec1.q * vec2.q + vec1.r * vec2.r +  + vec1.s * vec2.s;
        }


        // Easy adders
		void addP(T value) { p += value; }
        void addQ(T value) { q += value; }
		void addR(T value) { r += value; }
		void addS(T value) { s += value; }

		// Method to convert a char array string to a vector3, example "0.000000, 0.000000, 0.000000, 0.000000", WARNING, this function have nbo security CHECKS yet, use with caution
        void atovec4(char *array_Vector)
        {
			char *delimit = ",";
			p = atof(strtok(array_Vector, delimit));
			q = atof(strtok(NULL, delimit));
			r = atof(strtok(NULL, delimit));
			s = atof(strtok(NULL, delimit));

        }

        // ------------ Overloaded operators ------------

        // Overloaded addition operator to add Vec4s together
        Vec4 operator+(const Vec4 &vector) const
        {
            return Vec4<T>(p + vector.p, q + vector.q, r + vector.r, s + vector.s);
        }

        // Overloaded add and asssign operator to add Vec4s together
        void operator+=(const Vec4 &vector)
        {
			p += vector.p;
			q += vector.q;
			r += vector.r;
			s += vector.s;
        }

        // Overloaded subtraction operator to subtract a Vec4 from another Vec4
        Vec4 operator-(const Vec4 &vector) const
        {
            return Vec4<T>(p - vector.p, q - vector.q, r - vector.r, s - vector.s);
        }

        // Overloaded subtract and asssign operator to subtract a Vec4 from another Vec4
        void operator-=(const Vec4 &vector)
        {
            p -= vector.p;
			q -= vector.q;
			r -= vector.r;
			s -= vector.s;
        }

        // Overloaded multiplication operator to multiply two Vec4s together
        Vec4 operator*(const Vec4 &vector) const
        {
            return Vec4<T>(p * vector.p, q * vector.q, r * vector.r, s * vector.s);
        }

        // Overloaded multiply operator to multiply a vector by a scalar
        Vec4 operator*(const T &value) const
        {
            return Vec4<T>(p * value, q * value, r * value, s * value);
        }

        // Overloaded multiply and assign operator to multiply a vector by a scalar
        void operator*=(const T &value)
        {
            p *= value;
			q *= value;
			r *= value;
			s *= value;
        }

        // Overloaded multiply operator to multiply a vector by a scalar
        Vec4 operator/(const T &value) const
        {
            return Vec4<T>(p / value, q / value, r / value, s / value);
        }

        // Overloaded multiply and assign operator to multiply a vector by a scalar
        void operator/=(const T &value)
        {
            p /= value;
			q /= value;
			r /= value;
			s /= value;
        }
};
}// fpmed
#endif
