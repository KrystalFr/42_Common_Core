/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 14:45:37 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/29 09:54:50 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Fixed.hpp"

 int main( void ) 
 {
 	Fixed a;
 	Fixed b( a );
 	Fixed c;

 	c = b;
	
 	std::cout << a.getRawBits() << std::endl;
 	std::cout << b.getRawBits() << std::endl;
 	std::cout << c.getRawBits() << std::endl;
	return 0;
 }