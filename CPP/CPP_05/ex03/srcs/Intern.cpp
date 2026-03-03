/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Intern.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 16:48:40 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 18:03:33 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Intern.hpp"
#include "../includes/AForm.hpp"
#include "../includes/ShrubberyCreationForm.hpp"
#include "../includes/RobotomyRequestForm.hpp"
#include "../includes/PresidentialPardonForm.hpp"

Intern::Intern() {}

Intern::Intern(const Intern& other)
{
	(void) other;
}

Intern& Intern::operator=(const Intern& other)
{
	(void)other;
	return *this;
}

Intern::~Intern() {}

AForm* Intern::makeShrub(const std::string& target) const
{
	return (new ShrubberyCreationForm(target));
}

AForm* Intern::makeRobot(const std::string& target) const
{
	return (new RobotomyRequestForm(target));
}

AForm* Intern::makePres(const std::string& target) const
{
	return (new PresidentialPardonForm(target));
}

AForm* Intern::makeForm(const std::string& formName, const std::string& target) const
{
	const std::string	names[3] = {
		"shrubbery creation",
		"robotomy request",
		"presidential pardon"
	};

	AForm* (Intern::*fonctions[3])(const std::string& target) const = {
		&Intern::makeShrub,
		&Intern::makeRobot,
		&Intern::makePres
	};

	for (int i = 0; i < 3; i++)
	{
		if (formName == names[i])
		{
			std::cout << "Intern creates " << formName << std::endl;
			return ((this->*fonctions[i])(target));
		}
	}
	std::cout << "Intern couldn't create form: " << formName << std::endl;
	return NULL;
}