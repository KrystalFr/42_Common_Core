/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FragTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 01:14:57 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:13:20 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRAGTRAP_HPP
# define FRAGTRAP_HPP

# include "ClapTrap.hpp"
# include <iostream>

class FragTrap : public ClapTrap
{
	public:
    	FragTrap(std::string const& name);
    	FragTrap(const FragTrap& other);
   		FragTrap& operator=(const FragTrap& other);
    	~FragTrap(void);

		void beRepaired(unsigned int amount);
		void highFivesGuys(void);
};

#endif
