/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 18:12:02 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/14 00:13:31 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ZOMBIE_HPP
#define ZOMBIE_HPP

#include <string>
#include <iostream>

class Zombie 
{
	private: 
		std::string name;

	public:
		Zombie(std::string name);
		~Zombie();

		void announce(void);
};

Zombie* newZombie(std::string name);
void	randomChump(std::string name);
#endif