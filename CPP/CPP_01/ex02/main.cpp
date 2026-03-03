/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 13:57:16 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/14 15:04:32 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>

int main()
{
	std::string brain = "HI THIS IS BRAIN";
	std::string* stringPTR = &brain;
	std::string& stringREF = brain;

	std::cout << "Adress of brain variable: " << &brain << std::endl;
	std::cout << "Adress held by stringPTR: " << stringPTR << std::endl;
	std::cout << "Adress held by stringREF: " << &stringREF << std::endl;
	std::cout << std::endl;
	std::cout <<"Value of brain variable: " << brain << std::endl;
	std::cout <<"Value pointed to by stringPTR: " << *stringPTR << std::endl;
	std::cout <<"Value pointed to by stringREF: " << stringREF << std::endl;

	return 0;
}