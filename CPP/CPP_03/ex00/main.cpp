/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 13:07:32 by krfranco          #+#    #+#             */
/*   Updated: 2025/11/03 17:18:28 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClapTrap.hpp"

int main()
{
	ClapTrap dumb("Dumb");
	ClapTrap dumber("Dumber");

	std::cout << std::endl;
	dumb.attack("Dumber");
	dumber.takeDamage(0);

	dumber.attack("Dumb");
	dumb.takeDamage(0);

	std::cout << std::endl;

	dumb.takeDamage(2);
	dumber.takeDamage(4);

	dumb.beRepaired(3);
	dumber.beRepaired(3);

	std::cout << std::endl;

	for (int i = 0; i < 10; i++)
		dumb.attack("the sky");

	return 0;
}