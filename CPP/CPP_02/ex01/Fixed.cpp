/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 23:29:01 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/29 13:57:37 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fixed.hpp"

const int Fixed::fractBits = 8;

Fixed::Fixed(int nb) : rawBits(nb << fractBits)
{
	std::cout << "Int constructor called" << std::endl;
}

Fixed::Fixed(float f)
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

std::ostream& operator<<(std::ostream& os, const Fixed& f)
{
    os << f.toFloat();
    return os;
}