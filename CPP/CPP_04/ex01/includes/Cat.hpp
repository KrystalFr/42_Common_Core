/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:41 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 14:03:40 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAT_HPP
# define CAT_HPP

# include "Animal.hpp"
# include "Brain.hpp"

class Cat : public Animal
{
	private:
		Brain* brain;
	
	public:
    	Cat(void);
    	Cat(const Cat& other);
   		Cat& operator=(const Cat& other);
    	virtual~Cat(void);

		virtual void makeSound(void) const;
		Brain* getBrain(void) const;
};

#endif
