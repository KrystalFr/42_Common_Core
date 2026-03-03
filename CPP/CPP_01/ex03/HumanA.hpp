/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HumanA.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 15:43:29 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/16 12:33:54 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HUMANA_HPP
#define HUMANA_HPP

#include <string>
#include <iostream>
#include "Weapon.hpp"

class HumanA
{
	private:
		std::string name;
		Weapon& weapon;

	public:
		HumanA(std::string name, Weapon& weapon);
		~HumanA();
		void attack();
};
#endif