/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClapTrap.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 13:06:56 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/19 13:56:46 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLAPTRAP_HPP
# define CLAPTRAP_HPP

# include <iostream>
# include <string>

class ClapTrap
{
	private:
		std::string name;
		int			hp;
		int			ep;
		int			ad;

	public:
    	ClapTrap(std::string const& n);
    	ClapTrap(const ClapTrap& other);
   		ClapTrap& operator=(const ClapTrap& other);
    	~ClapTrap(void);
		
		void attack(const std::string& target);
		void takeDamage(unsigned int amount);
		void beRepaired(unsigned int amount);
};

#endif
