/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanB.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 15:43:06 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/16 12:33:58 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUMANB_HPP
#define HUMANB_HPP

#include <string>
#include <iostream>
#include "Weapon.hpp"

class HumanB
{
	private:
		std::string name;
		Weapon* weapon;

	public:
		HumanB(std::string name);
		~HumanB();
		void setWeapon(Weapon& weapon);
		void attack();
};
#endif