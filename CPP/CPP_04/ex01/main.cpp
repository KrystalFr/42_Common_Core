/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:28:45 by krfranco          #+#    #+#             */
/*   Updated: 2025/11/04 13:39:31 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Animal.hpp"
#include "includes/Dog.hpp"
#include "includes/Cat.hpp"
#include "includes/WrongAnimal.hpp"
#include "includes/WrongCat.hpp"

int main()
{
	int size = 10;
	Animal* tab[size];

	for (int i = 0; i < size; i++)
	{
		if (i < size/2)
			tab[i] = new Dog();
		else
			tab[i] = new Cat();
	}

	std::cout << std::endl;

	for (int i = 0; i < size; i++)
		delete tab[i];

	std::cout << std::endl;

    Dog* d1 = new Dog();
    d1->getBrain()->setIdea(0, "I want a bone");
	std::cout << "d1 brain idea: " << d1->getBrain()->getIdea(0) << std::endl;
	std::cout << std::endl;
	
    Dog d2 = *d1;
    std::cout << "d2 brain idea: " << d2.getBrain()->getIdea(0) << std::endl;
    d1->getBrain()->setIdea(0, "I want to play");
	std::cout << "d1 brain idea: " << d1->getBrain()->getIdea(0) << std::endl;
    std::cout << "d2 brain idea: " << d2.getBrain()->getIdea(0) << std::endl;

	std::cout << std::endl;
	delete d1;
	// Animal a;
	// a.makeSound();

    return 0;
}