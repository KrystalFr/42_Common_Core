/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScavTrap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 21:39:59 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:22:28 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ScavTrap.hpp"

ScavTrap::ScavTrap(std::string const& name) : ClapTrap(name)
{
	this->hp = 100;
	this->ep = 50;
	this->ad = 20;
	std::cout << "ScavTrap " << name << " constructed" << std::endl;
}

ScavTrap::ScavTrap(const ScavTrap& other) : ClapTrap(other)
{
    std::cout << "ScavTrap copy constructor called" << std::endl;
}

ScavTrap& ScavTrap::operator=(const ScavTrap& other)
{
    std::cout << "ScavTrap copy assignment operator called" << std::endl;
    if (this != &other)
		ClapTrap::operator=(other);
    return *this;
}

ScavTrap::~ScavTrap(void)
{
    std::cout << "ScavTrap " << name << " destructed" << std::endl;
}

void ScavTrap::attack(const std::string& target)
{
	if (hp <= 0)
	{
		std::cout << "ScavTrap " << name << ": has no HP and cannot attack." << std::endl;
		return;
	}
	if (ep <= 0)
	{
		std::cout << "ScavTrap " << name << ": has no EP and cannot attack." << std::endl;
		return;
	}
	--ep;
	std::cout << "ScavTrap " << name << ": attacks " << target << " with great force, dealing " << ad << " damages!" << std::endl; 
}

void ScavTrap::beRepaired(unsigned int amount)
{	
	if (ep > 0 && hp > 0 && hp + static_cast<int>(amount) <= 100)
    {
        hp += static_cast<int>(amount);
		std::cout << "ScavTrap " << name << ": has repaired " << amount << " HP, now has " << hp << " HP." << std::endl;
        --ep;
    }
	else if (hp <= 0)
		std::cout << "ScavTrap " << name << ": has no HP and cannot be repaired. " << std::endl;
	else if (ep <= 0)
		std::cout << "ScavTrap " << name << ": has no EP and cannot be repaired. " << std::endl;
	else
		std::cout << "ScavTrap " << name << ": cannot be repaired to have more than 100 HP. " << std::endl;
}

void ScavTrap::guardGate(void)
{
    std::cout << "ScavTrap " << name << ": is now in Gate Keeper mode." << std::endl;
}