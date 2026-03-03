/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:28:45 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:25:45 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Animal.hpp"
#include "includes/Dog.hpp"
#include "includes/Cat.hpp"
#include "includes/WrongAnimal.hpp"
#include "includes/WrongCat.hpp"

int main()
{
    const Animal* meta = new Animal();
    const Animal* j = new Dog();
    const Animal* i = new Cat();

	std::cout << std::endl;
    std::cout << j->getType() << " goes: ";
    j->makeSound();
    std::cout << i->getType() << " goes: ";
    i->makeSound();
    meta->makeSound();

	
	std::cout << std::endl;
    delete meta;
    delete j;
    delete i;

	
	std::cout << std::endl;
	const WrongAnimal* wa = new WrongAnimal();
    const WrongAnimal* wc = new WrongCat();

	
	std::cout << std::endl;
    wa->makeSound();
    wc->makeSound();

	
	std::cout << std::endl;
    delete wa;
    delete wc;
	
    return 0;
}