/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 18:11:45 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/14 13:03:05 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Zombie.hpp"

int main()
{
	randomChump("Stack-Zombie");

	Zombie* z = newZombie("Heap-Zombie");
	z->announce();
	delete z;
	
	return 0;
}