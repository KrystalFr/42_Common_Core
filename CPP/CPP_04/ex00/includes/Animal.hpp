/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Animal.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:28 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:44:53 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ANIMAL_HPP
# define ANIMAL_HPP

# include <iostream>

class Animal
{
	protected:
		std::string type;
	public:
    	Animal(void);
    	Animal(const Animal& other);
   		Animal& operator=(const Animal& other);
    	virtual ~Animal(void);

		virtual void makeSound(void) const;
		std::string getType(void) const;
};

#endif
