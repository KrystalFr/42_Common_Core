/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cat.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 02:26:41 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 02:53:04 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAT_HPP
# define CAT_HPP

# include "Animal.hpp"

class Cat : public Animal
{
	public:
    	Cat(void);
    	Cat(const Cat& other);
   		Cat& operator=(const Cat& other);
    	virtual~Cat(void);

		virtual void makeSound(void) const;
};

#endif
