/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zombieHorde.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 12:49:27 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/14 13:08:54 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Zombie.hpp"

Zombie* zombieHorde(int N, std::string name)
{
	if (N <= 0)
		return NULL;

	Zombie* horde = new Zombie[N];

	for (int i = 0; i < N; i++)
		horde[i].setName(name);

	return horde;
}