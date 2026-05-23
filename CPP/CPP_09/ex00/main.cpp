/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 21:54:22 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/23 12:51:49 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <string>

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}

	std::string input = av[1];
	std::ifstream infile(input.c_str());
	
	if (!infile.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return 1;
	}
	
	return 0;
}