/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 19:58:52 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:18:19 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClapTrap.hpp"
#include "ScavTrap.hpp"

int main()
{
	ClapTrap dumb("Dumb");
	ClapTrap dumber("Dumber");

	std::cout << std::endl;
	
	dumb.attack("intruder");
	dumber.attack("intruder");

	std::cout << std::endl;

	dumb.takeDamage(2);
	dumber.takeDamage(4);

	dumb.beRepaired(3);
	dumber.beRepaired(3);

	std::cout << std::endl;
	ScavTrap a("GuardA");
	ScavTrap b("GuardB");

	std::cout << std::endl;
	a.attack("intruder");
	b.attack("intruder");
	a.takeDamage(20);
	b.takeDamage(55);
	
	std::cout << std::endl;
	a.beRepaired(30);
	b.beRepaired(30);


	a.guardGate();
	b.guardGate();
	std::cout << std::endl;
	return 0;
}