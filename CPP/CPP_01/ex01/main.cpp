/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 12:49:03 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/14 13:26:20 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Zombie.hpp"
#include <cstdlib>

int main(int ac, char **av)
{
	if (ac != 2)
		return 1;
	
	int n = std::atoi(av[1]);
	if (n <= 0)
		return 1;
		
	Zombie* horde = zombieHorde(n, "Horde");
	if (!horde)
		return 1;
		
	for (int i = 0; i < n; i++)
		horde[i].announce();

	delete[] horde;
	return 0;
}