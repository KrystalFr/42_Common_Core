/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanB.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:39:23 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/28 13:40:33 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HumanB.hpp"

HumanB::HumanB(std::string name) : name(name) { }

HumanB::~HumanB()
{
	// std::cout << name << ": Destructor called" << std::endl;
}

void HumanB::setWeapon(Weapon& weapon)
{
	this->weapon = &weapon;
}

void HumanB::attack()
{
	if (weapon)
		std::cout << name << ": attacks with their " << weapon->getType() << std::endl;
	else
		std::cout << name << ": has no weapon" << std::endl;
}