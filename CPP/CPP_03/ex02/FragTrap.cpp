/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FragTrap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 23:46:24 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:19:57 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FragTrap.hpp"

FragTrap::FragTrap(std::string const& name) : ClapTrap(name)
{
	this->hp = 100;
	this->ep = 100;
	this->ad = 30;
	std::cout << "FragTrap " << name << " constructed" << std::endl;
}

FragTrap::FragTrap(const FragTrap& other) : ClapTrap(other)
{
    std::cout << "FragTrap copy constructor called" << std::endl;
}

FragTrap& FragTrap::operator=(const FragTrap& other)
{
    std::cout << "FragTrap copy assignment operator called" << std::endl;
    if (this != &other)
		ClapTrap::operator=(other);
    return *this;
}

FragTrap::~FragTrap(void)
{
    std::cout << "FragTrap " << name << " destructed" << std::endl;
}

void FragTrap::beRepaired(unsigned int amount)
{	
	if (ep > 0 && hp > 0 && hp + static_cast<int>(amount) <= 100)
    {
        hp += static_cast<int>(amount);
		std::cout << "FragTrap " << name << ": has repaired " << amount << " HP, now has " << hp << " HP." << std::endl;
        --ep;
    }
	else if (hp <= 0)
		std::cout << "FragTrap " << name << ": has no HP and cannot be repaired. " << std::endl;
	else if (ep <= 0)
		std::cout << "FragTrap " << name << ": has no EP and cannot be repaired. " << std::endl;
	else
		std::cout << "FragTrap " << name << ": cannot be repaired to have more than 100 HP. " << std::endl;
}


void FragTrap::highFivesGuys(void)
{
	std::cout << "FragTrap " << name << ": You want a high five? *SMACK*" << std::endl;
}
