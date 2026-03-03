/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Weapon.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 10:38:16 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/28 13:40:38 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Weapon.hpp"

Weapon::Weapon(std::string type) : type(type) { }

std::string Weapon::getType()
{
	return type;
}

void Weapon::setType(std::string newType)
{
	type = newType;
}

Weapon::~Weapon()
{
	// std::cout << type << ": has been destroyed" << std::endl;
}