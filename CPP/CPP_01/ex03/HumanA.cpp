/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanA.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:24:28 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/28 13:39:47 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HumanA.hpp"

HumanA::HumanA(std::string name, Weapon& weapon) 
	: name(name), weapon(weapon) { }

HumanA::~HumanA()
{
	// std::cout << name << " Destructor called" << std::endl;
}

void HumanA::attack()
{
	std::cout << name << ": attacks with their " << weapon.getType() << std::endl;
}