/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongCat.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 03:11:40 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:25:26 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/WrongCat.hpp"

WrongCat::WrongCat(void)
{
	this->type = "WrongCat";
    std::cout << "WrongCat: Constructor called" << std::endl;
}

WrongCat::WrongCat(const WrongCat& other) : WrongAnimal(other)
{
	this->type = other.type;
    std::cout << "WrongCat: Copy constructor called" << std::endl;
}

WrongCat& WrongCat::operator=(const WrongCat& other)
{
	if (this != &other)
		WrongAnimal::operator=(other);
    std::cout << "WrongCat: Copy assignment operator called" << std::endl;
    return *this;
}

WrongCat::~WrongCat(void)
{
    std::cout << "WrongCat: Destructor called" << std::endl;
}

void WrongCat::makeSound(void) const
{
    std::cout << "WrongCat: Woof?" << std::endl;
}
