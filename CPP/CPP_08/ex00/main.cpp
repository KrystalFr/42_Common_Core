/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:03:35 by krfranco          #+#    #+#             */
/*   Updated: 2026/04/07 16:35:44 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <vector>
#include <list>
#include "easyfind.hpp"

void testVector()
{
	//using push_back (add at end) because, number = {10, 20, 30} is C++11 syntax
	std::vector<int> v;
	v.push_back(10);
	v.push_back(20);
	v.push_back(30);

	std::cout << "Vector test:" << std::endl;
	try
	{
		std::cout << "Searching for value: 20" << std::endl;
		std::vector<int>::iterator it = easyfind(v, 20);
		std::cout << "Found value: " << *it << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}

	try
	{
		std::cout << "Searching for value: 99" << std::endl;
		easyfind(v, 99);
		std::cout << "Error: value 99 found" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}

	const std::vector<int> vc(v);

	std::cout << "const test:" << std::endl;
	try
	{
		std::cout << "Searching for value: 30" << std::endl;
		std::vector<int>::const_iterator it = easyfind(vc, 30);
		std::cout << "Found value: " << *it << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
}

void testList()
{
	std::list<int> l;
	l.push_back(10);
	l.push_back(20);
	l.push_back(30);

	std::cout << "List test:" << std::endl;
	try
	{
		std::cout << "Searching for value: 20" << std::endl;
		std::list<int>::iterator it = easyfind(l, 20);
		std::cout << "Found value: " << *it << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}

	try
	{
		std::cout << "Searching for value: 99" << std::endl;
		easyfind(l, 99);
		std::cout << "Error: value 99 found" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}

	const std::list<int> lc(l);

	std::cout << "const test:" << std::endl;
	try
	{
		std::cout << "Searching for value: 30" << std::endl;
		std::list<int>::const_iterator it = easyfind(lc, 30);
		std::cout << "Found value: " << *it << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
}

int main()
{
	testVector();
	std::cout << std::endl;
	testList();

	return 0;
}