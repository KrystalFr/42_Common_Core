/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:46 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:25:20 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Dog.hpp"

Dog::Dog(void)
{
	this->type = "Dog";
    std::cout << "Dog: Default constructor called" << std::endl;
}

Dog::Dog(const Dog& other) : Animal(other)
{
	this->type = other.type;
    std::cout << "Dog: Copy constructor called" << std::endl;
}

Dog& Dog::operator=(const Dog& other)
{
	if (this != &other)
		Animal::operator=(other);
    std::cout << "Dog: Copy assignment operator called" << std::endl;
    return *this;
}

Dog::~Dog(void)
{
    std::cout << "Dog: Destructor called" << std::endl;
}

void Dog::makeSound(void) const
{
    std::cout << "Dog: Woof! Woof!" << std::endl;
}
