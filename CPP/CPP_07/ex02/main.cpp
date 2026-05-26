/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 13:51:20 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/26 19:23:28 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Array.hpp"

int main()
{
	std::cout << "Test empty" << std::endl;
	
	Array<int> empty;
	std::cout << "[empty] size = " << empty.size() << std::endl;
	try
	{
		std::cout << "Accessing empty[0]..." << std::endl;
		std::cout << empty[0] << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
	}

	std::cout << std::endl;
	std::cout << "Test [] access" << std::endl;
	
	Array<int> a(5);
	std::cout << "[a] size = " << a.size() << std::endl;
	for (unsigned int i = 0; i < a.size(); i++)
		a[i] = static_cast<int>(i) * 10;
	std::cout << "[a] values: ";
	for (unsigned int i = 0; i < a.size(); i++)
		std::cout << a[i] << " ";
	std::cout << std::endl;

	std::cout << std::endl;
	std::cout << "Test deep copy" << std::endl;
	
	Array<int> b(a);
	b[0] = 999;
	std::cout << "[copy constructor] a[0] = " << a[0] << ", b[0] = " << b[0] << std::endl;

	Array<int> c;
	c = a;
	c[1] = 777;
	std::cout << "[operator=] a[1] = " << a[1] << ", c[1] = " << c[1] << std::endl;

	std::cout << std::endl;
	std::cout << "Test const array" << std::endl;
	
	const Array<int> ca(a);
	std::cout << "[ca] size = " << ca.size() << std::endl;
		std::cout << "[ca] values: ";
	for (unsigned int i = 0; i < ca.size(); i++)
		std::cout << ca[i] << " ";
	std::cout << std::endl;


	std::cout << std::endl;
	std::cout << "Test out of bounds" << std::endl;
	try
	{
		std::cout << "Accessing a[100]..." << std::endl;
		std::cout << a[100] << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cout << "Caught std::exception: " << e.what() << std::endl;
	}
	return 0;
}