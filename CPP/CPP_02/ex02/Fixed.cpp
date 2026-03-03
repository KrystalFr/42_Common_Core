/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 23:29:01 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/29 14:11:29 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fixed.hpp"

const int Fixed::fractBits = 8;

Fixed::Fixed(const int nb) : rawBits(nb << fractBits)
{
	 std::cout << "Int constructor called" << std::endl;
}

Fixed::Fixed(const float f)
{
	std::cout << "Float constructor called" << std::endl;
	float scaled = f * (1 << fractBits);
	float rounded = roundf(scaled);
	this->rawBits = rounded;
}

Fixed::Fixed(void) : rawBits(0)
{
    std::cout << "Default constructor called" << std::endl;
}

Fixed::Fixed(const Fixed& other)
{
    std::cout << "Copy constructor called" << std::endl;
    *this = other;
}

Fixed& Fixed::operator=(const Fixed& other)
{
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)
		this->rawBits = other.getRawBits();
    return *this;
}

Fixed::~Fixed(void)
{
    std::cout << "Destructor called" << std::endl;
}

int Fixed::getRawBits(void) const
{
	// std::cout << "getRawBits member function called" << std::endl;
	return this->rawBits;
}

void Fixed::setRawBits(int const raw)
{
	this->rawBits = raw;
}

float Fixed::toFloat(void) const
{
	return this->rawBits / 256.0f;
}

int Fixed::toInt(void) const
{
    return this->rawBits >> fractBits;
}

bool Fixed::operator>(const Fixed& other) const { return this->rawBits > other.rawBits; }
bool Fixed::operator<(const Fixed& other) const { return this->rawBits < other.rawBits; }
bool Fixed::operator>=(const Fixed& other) const { return this->rawBits >= other.rawBits; }
bool Fixed::operator<=(const Fixed& other) const { return this->rawBits <= other.rawBits; }
bool Fixed::operator==(const Fixed& other) const { return this->rawBits == other.rawBits; }
bool Fixed::operator!=(const Fixed& other) const { return this->rawBits != other.rawBits; }

Fixed Fixed::operator+(const Fixed& other) const
{
	Fixed res;
	res.rawBits =  this->rawBits + other.rawBits;
	return res;
}

Fixed Fixed::operator-(const Fixed& other) const
{
	Fixed res;
	res.rawBits =  this->rawBits - other.rawBits;
	return res;
}

Fixed Fixed::operator*(const Fixed& other) const
{
	Fixed res;
	res = Fixed(this->toFloat() * other.toFloat());
	return res;
}

Fixed Fixed::operator/(const Fixed& other) const
{
	Fixed res;
	res = Fixed(this->toFloat() / other.toFloat());
	return res;
}

Fixed& Fixed::operator++(void)
{
    ++(this->rawBits);
    return *this;
}

Fixed Fixed::operator++(int)
{
    Fixed tmp(*this);
    ++(this->rawBits);
    return tmp;
}

Fixed& Fixed::operator--(void)
{
    --(this->rawBits);
    return *this;
}

Fixed Fixed::operator--(int)
{
    Fixed tmp(*this);
    --(this->rawBits);
    return tmp;
}

Fixed& Fixed::min(Fixed& a, Fixed& b)
{
    return (a < b) ? a : b;
}

const Fixed& Fixed::min(const Fixed& a, const Fixed& b)
{
    return (a < b) ? a : b;
}

Fixed& Fixed::max(Fixed& a, Fixed& b)
{
    return (a > b) ? a : b;
}

const Fixed& Fixed::max(const Fixed& a, const Fixed& b)
{
    return (a > b) ? a : b;
}

std::ostream& operator<<(std::ostream& os, const Fixed& f)
{
    os << f.toFloat();
    return os;
}