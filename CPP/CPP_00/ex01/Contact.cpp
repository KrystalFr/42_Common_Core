/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Contact.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 13:24:49 by krfranco          #+#    #+#             */
/*   Updated: 2025/09/29 13:33:07 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Contact.hpp"

void Contact::set_contact(std::string& input, int info)
{
	switch(info)
	{
		case 0: firstname = input; break;
		case 1: lastname = input; break;
		case 2: nickname = input; break;
		case 3: phonenumber = input; break;
		case 4: darkestsecret = input; break;
	}
}

std::string Contact::get_contact(int info)
{
	switch(info)
	{
		case 0: return firstname;
		case 1: return lastname;
		case 2: return nickname;
		case 3: return phonenumber;
		case 4: return darkestsecret;
		default: return "";
	}
	
}