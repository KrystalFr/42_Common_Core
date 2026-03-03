/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongAnimal.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 03:11:34 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:21:42 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WrongAnimal.hpp"

WrongAnimal::WrongAnimal(void)
{
    std::cout << "WrongAnimal: Constructor called" << std::endl;
}

WrongAnimal::WrongAnimal(const WrongAnimal& other) : type(other.type)
{
    std::cout << "WrongAnimal: Copy constructor called" << std::endl;
}

WrongAnimal& WrongAnimal::operator=(const WrongAnimal& other)
{
	if (this != &other)
		this->type = other.type;
    std::cout << "WrongAnimal: Copy assignment operator called" << std::endl;
    return *this;
}

WrongAnimal::~WrongAnimal(void)
{
    std::cout << "WrongAnimal: Destructor called" << std::endl;
}

void WrongAnimal::makeSound(void) const
{
    std::cout << "WrongAnimal: *distorded animal sounds*" << std::endl;
}

std::string WrongAnimal::getType(void) const
{
    return this->type;
}
