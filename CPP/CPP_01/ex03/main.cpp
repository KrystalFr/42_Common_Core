/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 12:14:26 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/16 12:16:37 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Weapon.hpp"
#include "HumanA.hpp"
#include "HumanB.hpp"

int main()
{
	{
 		Weapon club = Weapon("crude spiked club");
		
 		HumanA bob("Bob", club);
	 	bob.attack();
 		club.setType("some other type of club");
 		bob.attack();
	}
	{
 		Weapon club = Weapon("crude spiked club");
		
 		HumanB jim("Jim");
 		jim.setWeapon(club);
 		jim.attack();
 		club.setType("some other type of club");
 		jim.attack();
	}
 	return 0;
 }