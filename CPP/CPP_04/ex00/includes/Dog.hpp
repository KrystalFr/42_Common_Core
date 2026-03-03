/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:49 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:53:25 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOG_HPP
# define DOG_HPP

# include "Animal.hpp"

class Dog : public Animal
{
	public:
    	Dog(void);
    	Dog(const Dog& other);
   		Dog& operator=(const Dog& other);
    	virtual ~Dog(void);

		virtual void makeSound(void) const;
};

#endif
