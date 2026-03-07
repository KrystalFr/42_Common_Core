/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 14:49:56 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/07 15:46:00 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "iter.hpp"

template <typename E>
void printElement(E &e)
{
	std::cout << e << std::endl;
}

void plusTen(E &e)
{
	e += 10;
}

int main()
{
	int array1[3] = {4, 5, 6};
	std::string array2[3] = {"yes", "no", "maybe"};
	return 0;
}