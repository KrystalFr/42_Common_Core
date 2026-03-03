/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Weapon.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 15:42:24 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/16 12:40:50 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <string>
#include <iostream>

class Weapon
{
	private:
		std::string type;
	
	public:
		Weapon(std::string type);
		~Weapon();
		std::string getType();
		void setType(std::string newType);
};
#endif