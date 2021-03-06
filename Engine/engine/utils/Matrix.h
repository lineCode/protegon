#pragma once

#include <cstdlib>
#include <type_traits> // std::enable_if_t, etc.

#include "utils/Vector2.h"

namespace internal {

// Custom template helpers.

// Returns template qualifier of whether or not the type is a integer / float point number.
// This includes bool, char, char8_t, char16_t, char32_t, wchar_t, short, int, long, long long, float, double, and long double.
template <typename T>
using is_number = std::enable_if_t<std::is_arithmetic_v<T>, bool>;

// Returns true if the matrix is a given size
template <std::size_t ROWS, std::size_t COLUMNS>
using is_square_matrix = std::enable_if_t<ROWS == COLUMNS, bool>;

template <std::size_t ROWS, std::size_t COLUMNS, std::size_t rows, std::size_t columns>
using is_matrix = std::enable_if_t<ROWS == rows && COLUMNS == columns, bool>;

// Matrix stream output / input delimeters, allow for consistent serialization / deserialization.

static constexpr const char MATRIX_LEFT_DELIMETER = '(';
static constexpr const char MATRIX_CENTER_DELIMETER = ',';
static constexpr const char MATRIX_RIGHT_DELIMETER = ')';

} // namespace internal

template <typename T, std::size_t ROWS, std::size_t COLUMNS, internal::is_number<T> = true>
struct Matrix {
	template <internal::is_matrix<ROWS, COLUMNS, 2, 2> = true>
	Matrix(T m_00, T m_01, T m_10, T m_11) {
		matrix[0][0] = m_00;
		matrix[0][1] = m_01;
		matrix[1][0] = m_10;
		matrix[1][1] = m_11;
	}
	Matrix() {
		for (auto i = 0; i < ROWS; ++i) {
			for (auto j = 0; j < COLUMNS; ++j) {
				this->matrix[i][j] = T{ 0 };
			}
		}
	}
	Matrix(T matrix[ROWS][COLUMNS]) {
		for (auto i = 0; i < ROWS; ++i) {
			for (auto j = 0; j < COLUMNS; ++j) {
				this->matrix[i][j] = matrix[i][j];
			}
		}
	}
	template <internal::is_matrix<ROWS, COLUMNS, 2, 2> = true>
	void SetRotationMatrix(double radians) {
		auto c = std::cos(radians);
		auto s = std::sin(radians);

		matrix[0][0] = c; 
		matrix[0][1] = -s;
		matrix[1][0] = s;
		matrix[1][1] = c;
	}
	template <internal::is_matrix<ROWS, COLUMNS, 2, 2> = true>
	Matrix Transpose() {
		return { matrix[0][0], matrix[1][0], matrix[0][1], matrix[1][1] };
	}
	T matrix[ROWS][COLUMNS];
};

// Bitshift / stream operators.

template <typename T, std::size_t ROWS, std::size_t COLUMNS>
std::ostream& operator<<(std::ostream& os, const Matrix<T, ROWS, COLUMNS> & obj) {
	os << internal::MATRIX_LEFT_DELIMETER << " ";
	for (auto i = 0; i < ROWS; ++i) {
		os << internal::MATRIX_LEFT_DELIMETER;
		for (auto j = 0; j < COLUMNS; ++j) {
			os << obj.matrix[i][j];
			if (j != COLUMNS) {
				os << ",";
			}
		}
		os << internal::MATRIX_RIGHT_DELIMETER;
		if (i != ROWS) {
			os << ",";
		}
	}
	os << " " << internal::MATRIX_RIGHT_DELIMETER;
	return os;
}

// Multiply 2x2 Matrix by Vector2.
template <typename T, typename U, internal::is_number<U> = true, typename S = typename std::common_type<T, U>::type>
Vector2<S> operator*(Matrix<T, 2, 2> m, const Vector2<U>& v) {
	return { m.matrix[0][0] * v.x + m.matrix[0][1] * v.y, m.matrix[1][0] * v.x + m.matrix[1][1] * v.y };
}