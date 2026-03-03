/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WrongAnimal.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 03:11:37 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 03:24:46 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WRONGANIMAL_HPP
# define WRONGANIMAL_HPP

# include <string>
# include <iostream>

class WrongAnimal
{
	protected:
    	std::string type;

	public:
    	WrongAnimal(void);
    	WrongAnimal(const WrongAnimal& other);
   		WrongAnimal& operator=(const WrongAnimal& other);
    	~WrongAnimal(void);

	void makeSound(void) const;
    std::string getType(void) const;
};

#endif
