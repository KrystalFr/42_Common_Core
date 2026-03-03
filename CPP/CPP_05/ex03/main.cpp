/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 16:10:20 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 17:58:16 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Intern.hpp"
#include "includes/Bureaucrat.hpp"
#include "includes/AForm.hpp"

int	main(void)
{
	try
	{
		Bureaucrat	good("Jean Parfait", 1);
		Bureaucrat	mid("Jean Padeursup", 74);
		Bureaucrat	bad("Jean Mediocre", 149);

		std::cout << std::endl;

		Intern	someRandomIntern;

		AForm*	shrub;
		AForm*	robo;
		AForm*	pardon;
		AForm*	unknown;

		shrub = someRandomIntern.makeForm("shrubbery creation", "bush");
		robo = someRandomIntern.makeForm("robotomy request", "Jean Mediocre");
		pardon = someRandomIntern.makeForm("presidential pardon", "Jean Padeursup");
		unknown = someRandomIntern.makeForm("something else", "target");

		std::cout << std::endl;
		if (shrub)
			std::cout << *shrub << std::endl;
		if (robo)
			std::cout << *robo << std::endl;
		if (pardon)
			std::cout << *pardon << std::endl;

		std::cout << std::endl;
		bad.signForm(*shrub);
		bad.ExecuteForm(*shrub);

		std::cout << std::endl;
		mid.signForm(*shrub);
		mid.signForm(*shrub);

		std::cout << std::endl;
		bad.ExecuteForm(*shrub);
		mid.ExecuteForm(*shrub);

		std::cout << std::endl;
		mid.signForm(*robo);
		mid.ExecuteForm(*robo);

		std::cout << std::endl;
		good.signForm(*robo);
		good.signForm(*robo);

		std::cout << std::endl;
		mid.ExecuteForm(*robo);
		good.ExecuteForm(*robo);

		std::cout << std::endl;
		good.signForm(*pardon);
		good.signForm(*pardon);
		good.ExecuteForm(*pardon);

		std::cout << std::endl;

		delete shrub;
		delete robo;
		delete pardon;
		delete unknown;
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
		return (1);
	}
	return (0);
}
