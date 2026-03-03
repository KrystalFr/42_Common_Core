/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScavTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 21:39:52 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:13:38 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCAVTRAP_HPP
# define SCAVTRAP_HPP

# include "ClapTrap.hpp"
# include <iostream>

class ScavTrap : public ClapTrap
{
	public:
    	ScavTrap(std::string const& name);
    	ScavTrap(const ScavTrap& other);
   		ScavTrap& operator=(const ScavTrap& other);
    	~ScavTrap(void);

		void beRepaired(unsigned int amount);
		void attack(const std::string& target);
        void guardGate(void);
};

#endif
