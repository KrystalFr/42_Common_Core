/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:35 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:25:16 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Cat.hpp"

Cat::Cat(void)
{
	this->type = "Cat";
    std::cout << "Cat: Default constructor called" << std::endl;
}

Cat::Cat(const Cat& other) : Animal(other)
{
	this->type = other.type;
    std::cout << "Cat: Copy constructor called" << std::endl;
}

Cat& Cat::operator=(const Cat& other)
{
	if (this != &other)
		Animal::operator=(other);
    std::cout << "Cat: Copy assignment operator called" << std::endl;
    return *this;
}

Cat::~Cat(void)
{
    std::cout << "Cat: Destructor called" << std::endl;
}

void Cat::makeSound(void) const
{
    std::cout << "Cat: Meow!" << std::endl;
}