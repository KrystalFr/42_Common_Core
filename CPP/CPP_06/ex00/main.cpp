/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 17:49:06 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/06 05:21:10 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/ScalarConverter.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "Invalid number of arguments, need: <"<< av[0] << "><string>" << std::endl;
		return 1;
	}
	
	try 
	{
		ScalarConverter::convert(av[1]);
	}
		catch (const std::exception& e)
	{
		std::cout << "Impossible conversion: " << e.what() << std::endl;
		return (1);
	}

	return 0;
}