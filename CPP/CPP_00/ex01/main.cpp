/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 22:30:33 by krfranco          #+#    #+#             */
/*   Updated: 2025/09/29 14:58:25 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Phonebook.hpp"

int main()
{
	Phonebook phonebook;
	std::string command;

	std::cout << "== Welcome to your Phonebook ==" << std::endl;
	while (true)
	{
		std::cout << "Enter command: ADD/SEARCH/EXIT\n" << std::endl;
		if (!std::getline(std::cin, command))
			break;
		if (command == "EXIT")
		{
			std::cout << "Goodbye !" << std::endl;
			break;
		}
		else if (command == "ADD")
			phonebook.add_contact();
		else if (command == "SEARCH")
			phonebook.search_contact();
		else
			std::cout << "invalid entry" << std::endl;
	}
	return (0);
}