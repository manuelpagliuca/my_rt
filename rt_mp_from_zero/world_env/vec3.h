#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec3 {


public:
	vec3() { e[0] = 0; e[1] = 0; e[2] = 0; }
	vec3(const float t) { e[0] = t; e[1] = t; e[2] = t; }
	vec3(const vec3& v) { this->e[0] = v.e[0]; this->e[1] = v.e[1]; this->e[2] = v.e[2]; }
	vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }

	inline const vec3& operator+() const {
		return *this;
	}

	inline vec3 operator-() const {
		return vec3(-e[0], -e[1], -e[2]);
	}

	inline float operator[](int i) const {
		return e[i];
	}

	inline float& operator[](int i) {
		return e[i];
	};

	inline vec3& operator+=(const vec3 & v2);
	inline vec3& operator-=(const vec3 & v2);
	inline vec3& operator*=(const vec3 & v2);
	inline vec3& operator/=(const vec3 & v2);
	inline vec3& operator*=(const float t);
	inline vec3& operator/=(const float t);

	inline float length() const {
		return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	}

	inline float squared_length() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline void make_unit_vector();

	float e[3];
};

inline std::istream& operator>>(std::istream & is, vec3 & t) {
	is >> t.e[0] >> t.e[1] >> t.e[2];
	return is;
}

inline std::ostream& operator<<(std::ostream & os, const vec3 & t) {
	os << t.e[0] << " " << t.e[1] << " " << t.e[2];
	return os;
}

inline void vec3::make_unit_vector() {
	float k = 1.0f / sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
	e[0] *= k; e[1] *= k; e[2] *= k;
}

inline vec3 operator+(const vec3 & v1, const vec3 & v2) {
	return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3 & v1, const vec3 & v2) {
	return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3 & v1, const vec3 & v2) {
	return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator/(const vec3 & v1, const vec3 & v2) {
	return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator*(float t, const vec3 & v) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator/(vec3 v, float t) {
	return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline vec3 operator*(const vec3 & v, float t) {
	return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline float dot(const vec3 & v1, const vec3 & v2) {
	return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3 & v1, const vec3 & v2) {
	return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
				(-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
				(v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

inline vec3 & vec3::operator+=(const vec3 & v) {
	e[0] += v.e[0];
	e[1] += v.e[1];
	e[2] += v.e[2];
	return *this;
}

inline vec3& vec3::operator*=(const vec3 & v) {
	e[0] *= v.e[0];
	e[1] *= v.e[1];
	e[2] *= v.e[2];
	return *this;
}

inline vec3& vec3::operator/=(const vec3 & v) {
	e[0] /= v.e[0];
	e[1] /= v.e[1];
	e[2] /= v.e[2];
	return *this;
}

inline vec3& vec3::operator-=(const vec3 & v) {
	e[0] -= v.e[0];
	e[1] -= v.e[1];
	e[2] -= v.e[2];
	return *this;
}

inline vec3& vec3::operator*=(const float t) {
	e[0] *= t;
	e[1] *= t;
	e[2] *= t;
	return *this;
}

inline vec3& vec3::operator/=(const float t) {
	float k = 1.0f / t;

	e[0] *= k;
	e[1] *= k;
	e[2] *= k;
	return *this;
}

// Funzione di normalizzazione di un vettore, restituisce il suo versore
inline vec3 unit_vector(const vec3 v) {
	return v / v.length();
}

// Gamma Correction
inline float sRGB(const float color_linear)
{
	return (color_linear <= 0.0031308f) ? 12.92f * color_linear : 1.055f * powf(color_linear, 1.0f / 2.4f) - 0.055f;
}

// Interpolazione lineare di due valori (Linear Interpolation)
inline vec3 lerp(const float t, const vec3& v0, const vec3& v1) {
	return (1.0f - t) * v0 + t * v1;
}
// Interpolazione lineare di due valori (old method)
inline vec3 lerp2(const float t, const vec3 & v0, const vec3 & v1) {
	return v0 + t * (v1 - v0);
}

// Mapping Lineare (Linear Mapping)
inline float linear_mapped(const vec3 & v) {
	return 0.5f* (v.y() + 1.0f);
}

#endif