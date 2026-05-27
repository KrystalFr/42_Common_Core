/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:18:40 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/27 14:44:46 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Error" << std::endl;
		return 1;
	}
	
	try
	{
		const std::string input = av[1];
		std::cout << RPN(input) << std::endl;
	}
	catch (const std::exception &e)
	{
		// std::cerr << e.what() << std::endl;
		std::cerr << "Error" << std::endl;
		return 1;
	}
	return 0;
}