/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dog.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:49 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:12:19 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DOG_HPP
# define DOG_HPP

# include "Animal.hpp"
# include "Brain.hpp"

class Dog : public Animal
{
	private:
		Brain* brain;
	
	public:
    	Dog(void);
    	Dog(const Dog& other);
   		Dog& operator=(const Dog& other);
    	virtual ~Dog(void);

		virtual void makeSound(void) const;
		Brain* getBrain(void) const;
};

#endif
