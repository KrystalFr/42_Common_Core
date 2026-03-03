/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 22:48:08 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/18 12:37:10 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Harl.hpp"

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cout << "NEED: <" << av[0] << "> <LEVEL>" << std::endl;
		return 1;
	}

	Harl harl;
	harl.complain(av[1]);
	return 0;
}