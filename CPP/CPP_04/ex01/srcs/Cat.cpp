/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:35 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:21:34 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Cat.hpp"

Cat::Cat(void) : brain(new Brain())
{
	this->type = "Cat";
    std::cout << "Cat: Default constructor called" << std::endl;
}

Cat::Cat(const Cat& other) : Animal(other), brain(new Brain(*other.brain))
{
	this->type = other.type;
    std::cout << "Cat: Copy constructor called" << std::endl;
}

Cat& Cat::operator=(const Cat& other)
{
	if (this != &other)
	{
        Animal::operator=(other);
        if (this->brain)
            delete this->brain;
        this->brain = new Brain(*other.brain);
    }
    std::cout << "Cat: Copy assignment operator called" << std::endl;
    return *this;
}

Cat::~Cat(void)
{
	delete this->brain;
    std::cout << "Cat: Destructor called" << std::endl;
}

void Cat::makeSound(void) const
{
    std::cout << "Cat: Meow!" << std::endl;
}