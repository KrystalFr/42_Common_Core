/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Base.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 22:56:18 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/06 23:27:34 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE_CPP
# define BASE_CPP

#include <cstdlib>
#include <ctime>
#include <iostream>

class Base
{
	public:
		virtual ~Base();
};

Base * generate();
void identify(Base* p);
void identify(Base& p);

#endif