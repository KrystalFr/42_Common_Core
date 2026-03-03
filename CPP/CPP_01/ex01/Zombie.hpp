/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Zombie.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 12:44:20 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/14 13:36:10 by krfranco         ###   ########.fr       */
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
		Zombie();
		Zombie(std::string name);
		~Zombie();

		void announce(void);
		void setName(std::string name);
};

Zombie* zombieHorde(int N, std::string name);
#endif