/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RobotomyRequestForm.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:19:57 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/06 23:36:23 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/RobotomyRequestForm.hpp"
#include <cstdlib>
#include <ctime>

RobotomyRequestForm::RobotomyRequestForm(const std::string& t)
	: AForm("RobotomyRequestForm", 72, 45)
	, target(t)
{
	std::srand(std::time(NULL));
	// std::cout << "RobotomyRequestForm: Default constructor called" << std::endl;
}

RobotomyRequestForm::RobotomyRequestForm(const RobotomyRequestForm& other)
	:AForm(other)
	, target(other.target)
{
	// std::cout << "RobotomyRequestForm: Copy constructor called" << std::endl;
}

RobotomyRequestForm& RobotomyRequestForm::operator=(const RobotomyRequestForm& other)
{
	// std::cout << "RobotomyRequestForm: Copy assignment operator called" << std::endl;
	if (this != &other)
	{
		AForm::operator=(other);
	}
	return *this;
}

RobotomyRequestForm::~RobotomyRequestForm()
{
	// std::cout << "RobotomyRequestForm: Destructor called" << std::endl;
}

void RobotomyRequestForm::executeAction() const
{
	std::cout << this->target << " *Bzzzzz... BzzZZZZZZZZ.. zz...*" << std::endl;

	if (std::rand() % 2 == 0)
		std::cout << this->target << " has been successfully robotomized !" << std::endl;
	else
		std::cout << this->target << "'s robotomy has failed." << std::endl;
}
