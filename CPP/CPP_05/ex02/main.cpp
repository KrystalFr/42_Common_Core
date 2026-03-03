/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 16:10:20 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 17:55:31 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/AForm.hpp"
#include "includes/ShrubberyCreationForm.hpp"
#include "includes/RobotomyRequestForm.hpp"
#include "includes/PresidentialPardonForm.hpp"
#include "includes/Bureaucrat.hpp"

int main()
{
	try
	{	
		Bureaucrat good("Jean Parfait", 1);
		Bureaucrat mid("Jean Padeursup", 74);
		Bureaucrat bad("Jean Mediocre", 149);

		std::cout << std::endl;
		ShrubberyCreationForm shrub("bush");
		RobotomyRequestForm bzz("Jean Mediocre");
		PresidentialPardonForm pardon("Jean Padeursup");

		std::cout << std::endl;
		std::cout << shrub << std::endl;
		std::cout << bzz << std::endl;
		std::cout << pardon << std::endl;

		std::cout << std::endl;
		bad.signForm(shrub);
		bad.ExecuteForm(shrub);

		std::cout << std::endl;
		mid.signForm(shrub);
		mid.signForm(shrub);

		std::cout << std::endl;
		bad.ExecuteForm(shrub);
		mid.ExecuteForm(shrub);
		
		std::cout << std::endl;
		mid.signForm(bzz);
		mid.ExecuteForm(bzz);
		
		std::cout << std::endl;
		good.signForm(bzz);
		good.signForm(bzz);

		std::cout << std::endl;
		mid.ExecuteForm(bzz);
		good.ExecuteForm(bzz);

		std::cout << std::endl;
		good.signForm(pardon);
		good.signForm(pardon);
		good.ExecuteForm(pardon);

		std::cout << std::endl;
		
	}
	catch (const std::exception & e)
	{
		std::cout << "Invalid declaration: " << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}
