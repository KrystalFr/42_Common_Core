/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClapTrap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 13:06:49 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:21:38 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClapTrap.hpp"

ClapTrap::ClapTrap(std::string const& n) : name(n), hp(10), ep(10), ad(0)
{
    std::cout << "ClapTrap " << name << " constructed" << std::endl;
}

ClapTrap::ClapTrap(const ClapTrap& other)
{
    std::cout << "Copy constructor called" << std::endl;
    this->name = other.name;
    this->hp = other.hp;
    this->ep = other.ep;
    this->ad = other.ad;
}

ClapTrap& ClapTrap::operator=(const ClapTrap& other)
{
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)
    {
        this->name = other.name;
        this->hp = other.hp;
        this->ep = other.ep;
        this->ad = other.ad;
    }
    return *this;
}

ClapTrap::~ClapTrap(void)
{
    std::cout << "ClapTrap " << name << " destructed" << std::endl;
}

void ClapTrap::attack(const std::string& target)
{
	if (hp <= 0)
	{
		std::cout << "ClapTrap " << name << ": has no HP and cannot attack." << std::endl;
		return;
	}
	if (ep <= 0)
	{
		std::cout << "ClapTrap " << name << ": has no EP and cannot attack." << std::endl;
		return;
	}
	--ep;
	std::cout << "ClapTrap " << name << ": attacks " << target << ", dealing " << ad << " damages!" << std::endl; 
}

void ClapTrap::takeDamage(unsigned int amount)
{
    if (hp > static_cast<int>(amount))
        hp -= static_cast<int>(amount);
    else if (hp > 0)
        hp = 0;
    else
	{
        std::cout << "ClapTrap " << name << ": is already dead, stop beating it." << std::endl;
        return;
    }
	std::cout << "ClapTrap " << name << ": takes " << amount << " damages, now has " << hp << " HP." << std::endl; 
}

void ClapTrap::beRepaired(unsigned int amount)
{	
	if (ep > 0 && hp > 0 && hp + static_cast<int>(amount) <= 10)
    {
        hp += static_cast<int>(amount);
		std::cout << "ClapTrap " << name << ": has repaired " << amount << " HP, now has " << hp << " HP." << std::endl;
        --ep;
    }
	else if (hp <= 0)
		std::cout << "ClapTrap " << name << ": has no HP and cannot be repaired. " << std::endl;
	else if (ep <= 0)
		std::cout << "ClapTrap " << name << ": has no EP and cannot be repaired. " << std::endl;
	else
		std::cout << "ClapTrap " << name << ": cannot be repaired to have more than 10 HP. " << std::endl;
}
