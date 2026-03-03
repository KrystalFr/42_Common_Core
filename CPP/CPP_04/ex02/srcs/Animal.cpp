/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:07 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:13:27 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Animal.hpp"

Animal::Animal(void) : type("Animal")
{
    std::cout << "Animal: Default constructor called" << std::endl;
}

Animal::Animal(const Animal& other)
{
	this->type = other.type;
    std::cout << "Animal: Copy constructor called" << std::endl;
}

Animal& Animal::operator=(const Animal& other)
{
	if (this != &other)
        this->type = other.type;
    std::cout << "Animal: Copy assignment operator called" << std::endl;
    return *this;
}

Animal::~Animal(void)
{
    std::cout << "Animal: Destructor called" << std::endl;
}

std::string Animal::getType(void) const
{
    return this->type;
}
