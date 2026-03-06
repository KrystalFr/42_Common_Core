/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 22:55:26 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/07 00:27:57 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Base.hpp"

int main()
{
	Base *base = generate();
	
	std::cout << "identify(Base*): ";
	identify(base);
	std::cout << "identify(Base&): ";
	identify(*base);
		
	Base *base2 = NULL;
	std::cout << "identify(NULL): ";
	identify(base2);

	delete base;
	return 0;
}