/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 22:55:31 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/07 00:39:34 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Base.hpp"
#include "../includes/A.hpp"
#include "../includes/B.hpp"
#include "../includes/C.hpp"

Base::~Base(){}

Base * generate()
{
	std::srand(std::time(NULL));
	int r = std::rand() % 3;
	
	if (r == 0)
	{
		std::cout << "A class created" << std::endl;
		return new A;
	}
	
	if (r  == 1)
	{
		std::cout << "B class created" << std::endl;
		return new B;
	}

	std::cout << "C class created" << std::endl;
		return new C;
}

void identify(Base* p)
{
	if (p == NULL)
	{
		std::cout << "NULL" << std::endl;
		return;
	}
	
	if (dynamic_cast<A*>(p))
	{
		std::cout << "Type is A" << std::endl;
		return;
	}

	if (dynamic_cast<B*>(p))
	{
		std::cout << "Type is B" << std::endl;
		return;
	}

	if (dynamic_cast<C*>(p))
	{
		std::cout << "Type is C" << std::endl;
		return;
	}

	std::cout << "Unknown type" << std::endl;
}

void identify(Base& p)
{
	try
	{
		(void)dynamic_cast<A&>(p);
		std::cout << "Type is A" << std::endl;
		return;
	}
	catch (const std::exception&) {}

	try
	{
		(void)dynamic_cast<B&>(p);
		std::cout << "Type is B" << std::endl;
		return;
	}
	catch (const std::exception&) {}
	
	try
	{
		(void)dynamic_cast<C&>(p);
		std::cout << "Type is C" << std::endl;
		return;
	}
	catch (const std::exception&) {}

	std::cout << "Unknown type" << std::endl;
}