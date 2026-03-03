/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Phonebook.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 13:21:39 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/27 16:08:59 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Phonebook.hpp"

void Phonebook::add_contact()
{
	std::string input;
	const std::string prompts[5] = {
		"Enter first name: ",
		"Enter last name: ",
		"Enter nickname: ",
		"Enter phone number: ",
		"Enter darkest secret: "
	};
	
	static int next_i = 0;
	if (next_i >= 8)
		next_i = 0;
	
	for (int i = 0; i < 5; i++)
	{
		std::cout << prompts[i] << std::endl;
		if (!std::getline(std::cin, input))
			return;
		std::size_t found = input.find_first_not_of(" \t\n\r\v");
		if (found == std::string::npos)
		{
			std::cout << "Can't have empty field" << std::endl;
			i--;
		}
		else
			contacts[next_i].set_contact(input, i);
	}
	next_i++;
}

void Phonebook::search_contact()
{
	int nb = 0;

	if (contacts[0].get_contact(0).empty())
	{
		std::cout << "No contacts found ! Please add contacts first.\n" << std::endl;
			return;
	}
	
	std::cout << std::setw(10) << std::right << "Index" << "|";
	std::cout << std::setw(10) << std::right << "FirstName" << "|";
	std::cout << std::setw(10) << std::right << "LastName" << "|";
	std::cout << std::setw(10) << std::right << "NickName" << "|" << std::endl;
	
	for (int i = 0; i < 8 && !contacts[i].get_contact(0).empty(); i++)
	{
		std::cout << std::setw(10) << std::right << i + 1 << "|";
		if (contacts[i].get_contact(0).length() > 10)
		{
			std::string cat = contacts[i].get_contact(0).substr(0, 9) + ".";
			std::cout << std::setw(10) << std::right << cat << "|";
		}
		else 
			std::cout << std::setw(10) << std::right << contacts[i].get_contact(0) << "|";

		if (contacts[i].get_contact(1).length() > 10)
		{
			std::string cat1 = contacts[i].get_contact(1).substr(0, 9) + ".";
			std::cout << std::setw(10) << std::right << cat1 << "|";
		}
		else
			std::cout << std::setw(10) << std::right << contacts[i].get_contact(1) << "|";

		if (contacts[i].get_contact(2).length() > 10)
		{
			std::string cat2 = contacts[i].get_contact(2).substr(0, 9) + ".";
			std::cout << std::setw(10) << std::right << cat2 << "|" << std::endl;
		}
		else
			std::cout << std::setw(10) << std::right << contacts[i].get_contact(2) << "|" << std::endl;
	}
	

	std::cout << "Enter index of contact to display" << std::endl;
	
	std::string input;
	while (nb < 1 || nb > 8 || (nb > 0 && contacts[nb - 1].get_contact(0).empty()))
	{
		if (!std::getline(std::cin, input))
			return ;
		if (input.empty() || input.length() > 2)
			nb = 0;
		else
			nb = atoi(input.c_str());
		if (nb < 1 || nb > 8 || (nb > 0 && contacts[nb - 1].get_contact(0).empty()))
			std::cout << "Please enter a valid index" << std::endl;
	}
	
	for (int i = 0; i < 5; i++)
		std::cout << contacts[nb - 1].get_contact(i) << std::endl;
	std::cout << "==========\n" << std::endl;
}
