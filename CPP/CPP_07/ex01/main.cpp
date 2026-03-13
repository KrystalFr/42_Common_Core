/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 14:49:56 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/13 13:59:51 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "iter.hpp"

template <typename E>
void printElement(E &e)
{
	std::cout << e << std::endl;
}

template <typename E>
void plusTen(E &e)
{
	e += 10;
}

int main()
{
	int int_array[3] = {4, 5, 6};
    std::cout << "test for int: " << std::endl;
    ::iter(int_array, 3, printElement<int>);

    std::cout << "plusTen: " << std::endl;
    ::iter(int_array, 3, plusTen<int>);
    ::iter(int_array, 3, printElement<int>);

  
	std::string string_array[3] = {"yes", "no", "maybe"};
	std::cout << "\ntest for string: " << std::endl;
    ::iter(string_array, 3, printElement<std::string>);

    double double_array[3] = {1.2, 3.4 ,5.6};
    std::cout << "\ntest for double: " << std::endl;
    ::iter(double_array, 3, printElement<double>);
	
    std::cout << "plusTen: " << std::endl;
    ::iter(double_array, 3, plusTen<double>);
    ::iter(double_array, 3, printElement<double>);

    const int d[] = {1, 2 ,3};
    std::cout << "\ntest for const int: " << std::endl;
    ::iter(d, 3, printElement<const int>);
	return 0;
}