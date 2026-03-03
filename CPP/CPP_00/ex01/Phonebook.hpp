/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Phonebook.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 13:21:43 by krfranco          #+#    #+#             */
/*   Updated: 2025/09/29 13:34:14 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "Contact.hpp"

class Phonebook
{
	private:
		Contact contacts[8];
	
	public:
		void add_contact();
		void search_contact();
};

#endif