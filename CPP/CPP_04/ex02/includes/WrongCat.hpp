/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongCat.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 03:11:43 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:12:18 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WRONGCAT_HPP
# define WRONGCAT_HPP

# include "WrongAnimal.hpp"
# include <iostream>

class WrongCat : public WrongAnimal
{
	public:
    	WrongCat(void);
    	WrongCat(const WrongCat& other);
   		WrongCat& operator=(const WrongCat& other);
    	~WrongCat(void);

		void makeSound(void) const;
};

#endif
